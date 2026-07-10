/**
 ******************************************************************************
 * @file    application.c
 * @brief   This file contains the implementation for the Asset Tracking
 *          application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "application.h"
#include "app_sensors.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <time.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "message_buffer.h"

#include "gpio.h"
#include "usart.h"
#include "lptim.h"
#include "rtc.h"

#include "st87ec_lib.h"
#include "st87ec_engine.h"
#include "st87ec_wrapper.h"

/* Private typedef -----------------------------------------------------------*/

/* Application states */
typedef enum APP_State_t
{
  AppState_Init = 0,
  AppState_RtcInitSequence,
  AppState_Idle,
  AppState_PrepareTelemetry,
  AppState_SendTelemetry,
  AppState_HTTP_Open,
  AppState_HTTP_WaitOpen,
  AppState_HTTP_Send,
  AppState_HTTP_WaitResponse,
  AppState_HTTP_Close,
//  AppState_PollCommands,
//  AppState_SendAck,
  AppState_GoToSleep
} APP_State_t;

/* struct per il payload dei dati telemetrici da inviare al server */
typedef struct
{
  char deviceId[32];
  char timestamp[32];
  float temperature;
  float humidity;
  float pressure;
  uint8_t battery;
  char orientation[32];
} TelemetryPayload_t;

/* struct per il payload dei comandi pendenti da inviare al server */
typedef struct
{
  int id;
  char device_id[32];
  char type[32];
  char payload[64];
  char status[16];
  char created_at[32];
} PendingCommand_t;

/* struct per il payload di ack dei comandi eseguiti da inviare al server */
typedef struct
{
  char status[16];
  char ackAt[32];
  char resultMessage[128];
} CommandAckPayload_t;

/* Private define ------------------------------------------------------------*/

/* ----------------------------------------*/
/* Configurable parameters*/

/* Uncomment one of the following lines to enable the corresponding low power state */
//#define LOW_POWER_MODE_STOP2
#define LOW_POWER_MODE_SLEEP
#define APP_LOG_PREFIX            "\r\n*** "

/* Select the application sleep time */
//#define APPLICATION_SLEEP_TIME_S        50U     //----------------------------------------- MODIFICATO GIU' PER TIMING DI INVIO MEX UDP
//#define HTTP_SERVER_IP                "httpbin.org"
#define HTTP_SERVER_IP                  "10.68.87.69"
#define HTTP_SERVER_PORT                8080
#define TELEMETRY_PATH                  "/api/v1/telemetry"
#define COMMANDS_PENDING_PATH           "/api/v1/devices/%s/commands/pending" //fmt =
#define COMMAND_ACK_PATH                "/api/v1/devices/%s/commands/%d/ack"

#define HTTP_SECURE_ID                  -1    // -1 for HTTP, 0..15 for HTTPS
#define HTTP_KEEP_ALIVE                  0
#define MAX_HTTP_RESPONSE_WAIT_MS       20000U
#define HTTP_TIMEOUT                    20000U
#define HTTP_POLL_DELAY_MS              100U

#define HTTP_RAW_REQUEST_BUF_SIZE       768U
#define HTTP_RESPONSE_BUF_SIZE          768U
#define JSON_PAYLOAD_BUF_SIZE           384U

#define DEVICE_ID                       "DEV001"

/* ----------------------------------------*/

/* Sleep time converted to us (microseconds) */
#define APPLICATION_SLEEP_TIME_US       (APPLICATION_SLEEP_TIME_S*1000000)

#define APP_LPTIM                       (&hlptim1)
/* Timer tick period according to hardware configuration
 * LSE = 32768 Hz, Prescaler = 128, --> LPTIM clock = 32768/128 = 256 Hz --> LPTIM tick time = 1/256 = 3906 us
 * LSI = 32000 Hz, Prescaler = 128, --> LPTIM clock = 32000/128 = 250 Hz --> LPTIM tick time = 1/250 = 4000 us
 */
#define LPTIM_TICK_TIME_US              3906U
/* Timer Period */
#define LPTIM_PERIOD                    (APPLICATION_SLEEP_TIME_US/LPTIM_TICK_TIME_US)

#if (LPTIM_PERIOD>65535)
#error "LPTIM_PERIOD too big. Try reducing APPLICATION_SLEEP_TIME_S."
#endif

#define HTTP_OPEN_WAIT_TIMEOUT_MS       15000U
#define HTTP_RESPONSE_WAIT_TIMEOUT_MS   15000U

#define HTTP_TASK_PRIORITY              3U
#define SENSOR_TASK_PRIORITY            4U

/*-- event management --*/
#define EVENT_BATCH_SIZE                5U          // size of the event batch to be sent in each UDP packet
#define SENSORS_SAMPLE_INTERVAL_S       60U         // 1 minute
#define TELEMETRY_SEND_INTERVAL_S       (1U * 60U)  // 1 min - send the batch every UDP_SEND_INTERVAL_S seconds
#define MAX_POST_PER_CYCLE              3U  // max number of HTTP POST requests to send in one cycle (to avoid sending too many requests at once)

// Ogni 60s il LPTIM genera il CompareMatch → HAL_LPTIM_CompareMatchCallback → queue eEventTimer.
#define APPLICATION_SLEEP_TIME_S        SENSORS_SAMPLE_INTERVAL_S

/* Tasks stack size */
#define HTTP_TASK_STACK_SIZE            (configMINIMAL_STACK_SIZE * 12)
#define SENSOR_TASK_STACK_SIZE          (configMINIMAL_STACK_SIZE * 2)
#define LED_TASK_STACK_SIZE             (configMINIMAL_STACK_SIZE * 2)

/* LED Timing when not connected */
#define LED_TIME_ON_IDLE       1000U
#define LED_TIME_OFF_IDLE      1000U

/* LED Timing when connected */
#define LED_TIME_ON_ATTACHED   100U
#define LED_TIME_OFF_ATTACHED  900U

/* LED Timing when executing a sequence */
#define LED_TIME_ON_SEQUENCE   100U
#define LED_TIME_OFF_SEQUENCE  100U

/* Private variables ---------------------------------------------------------*/

/* Tasks handlers */
static TaskHandle_t xHTTPTaskHandle = NULL;
static TaskHandle_t xSensorsDataTaskHandle = NULL;

//MessageBufferHandle_t xSensorDataMBHandle = NULL; // not used if I send data through circular array
QueueHandle_t xDataQueue = NULL;

/* State machine variable */
volatile APP_State_t app_state = AppState_Init;

/* HTTP Object  */
ST87EC_Lib_HttpTransferObject_t httpTxObj;

// variabili per la gestione della connessione HTTP
char *pHost = HTTP_SERVER_IP;
char *pPath = TELEMETRY_PATH;
uint32_t PortNb = HTTP_SERVER_PORT;
uint32_t SecureId = HTTP_SECURE_ID;
uint32_t Timeout = HTTP_TIMEOUT;

static uint32_t last_http_send_time_s = 0; // timestamp of the last HTTP send operation
extern bool send_immediately; // flag to indicate if an immediate send is required (set by FSM events)

/* Variabili introdotte per HTTPTask */
static uint32_t g_httpWaitStartTick = 0;
static bool g_httpTransferStarted = false;

/* variabili per tenere traccia del numero di POST inviati in un ciclo, per evitare di inviare troppi POST in un breve periodo */
static bool g_sleepTimerArmed = false; // flag to indicate if the sleep timer is armed, used to manage the sleep cycle after sending HTTP POST requests
static uint8_t g_postsSentThisCycle = 0; // counter for the number of HTTP POST requests sent in the current cycle
static bool g_forceImmediateCycle = false; // flag to force an immediate cycle of HTTP POST requests, set when a FSM event occurs
static bool g_firstBootSampleSent = false; // flag to indicate if the first boot sample has been sent, used to ensure that the first sample is sent even if no events occur
static uint32_t g_lastErrorResetTimeMs = 0; // timestamp of the last modem reset due to an error, used to avoid too frequent resets

static char httpRawRequest[HTTP_RAW_REQUEST_BUF_SIZE];
static char httpResponseBuffer[HTTP_RESPONSE_BUF_SIZE];
static char jsonPayloadBuffer[JSON_PAYLOAD_BUF_SIZE];

static volatile bool httpResponseReady = false;
//static PendingCommand_t g_pendingCommand;
//static bool g_hasPendingCommand = false;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void HTTPTask(void *pvParameters);

static void LedBlinking(ST87EC_Lib_Status_t *eclib_state);

static void GetTimeCallback(char const *const pString);
static uint32_t GetCurrentTimeSeconds(void);

static void HttpReceiveCallback(char const *const receivedData);
static void GetCurrentTimestamp(char *buffer, size_t len);
static const char* MapOrientationFromEvent(const SensorsData *data);

// funzione per attendere la risposta HTTP con timeout
static bool WaitHttpResponse(uint32_t timeoutMs);

static bool BuildTelemetryJson(char *buffer, size_t len, const TelemetryPayload_t *payload);
static bool BuildAckJson(char *buffer, size_t len, const CommandAckPayload_t *ack);

static bool BuildHttpPostRequest(char *out, size_t outLen, const char *host, const char *path, const char *jsonBody);
static bool BuildHttpGetRequest(char *out, size_t outLen, const char *host, const char *path);

//static bool HttpOpenConnection(void);
//static bool HttpCloseConnection(void);
static bool HttpSendRawRequest(const char *rawRequest);

static bool ParseForceMeasurementCommand(const char *response, PendingCommand_t *cmd);

/**
 * @brief Initializes the application.
 * 
 * This function sets up the necessary components and configurations 
 * required for the application to run. It should be called at the 
 * beginning of the program to ensure all initializations are properly 
 * handled.
 */
void AppInit(void)
{
  /* Creation of the Queue for 2 Tasks */
  xDataQueue = xQueueCreate(2, sizeof(eEventType_t)); // (LUNGHEZZA CODA, DIMENSIONE ELEMENTO NELLA CODA)
  configASSERT(xDataQueue != NULL);

  BaseType_t xReturned; // var per controllare l'esito di creazione del task

  xReturned = xTaskCreate(HTTPTask, "http_task",
  HTTP_TASK_STACK_SIZE,
                          NULL,
                          HTTP_TASK_PRIORITY,
                          &xHTTPTaskHandle);

  configASSERT(xReturned == pdPASS);  // verifica di corretta creazione del task

  xReturned = xTaskCreate(SensorsDataTask, "sensors_task",
  SENSOR_TASK_STACK_SIZE,
                          NULL,
                          SENSOR_TASK_PRIORITY,
                          &xSensorsDataTaskHandle);

  configASSERT(xReturned == pdPASS);

  /* a) Creation of the Message Buffer */
//  xSensorDataMBHandle = xMessageBufferCreate(sizeof(SensorsData) * 2);
//  configASSERT(xSensorDataMBHandle != NULL);
  /* b) Creation of the Message Buffer using a Circular Array
   *
   * Inizializzata in "app_sensors.c".
   * Non serve la xSensorDataMBHandle se uso l'approccio con circular array,
   * in quanto non uso le API di FreeRTOS per la gestione del buffer, ma gestisco direttamente
   * la lettura/scrittura sulla struct statica s_eventBuffer.
   *
   */
}

/**
 * @brief HTTP task for NB-IoT HTTP application.
 *
 * This FreeRTOS task manages the HTTP communication over NB-IoT using the ST87EC library.
 * It initializes the modem, waits for network registration, and then enters a state machine
 * to handle sensor data transmission via HTTP. The task interacts with other tasks via a message buffer,
 * formats sensor data, and sends it to a predefined HTTP server. It also manages modem resets,
 * sleep cycles, and handles the HTTP transfer completion via callbacks.
 *
 * State machine overview:
 * - AppState_Init: Waits for network registration and initializes RTC.
 * - AppState_RtcInitSequence: Placeholder for RTC initialization sequence.
 * - AppState_Idle: Waits for sensor data and sends it over HTTP when available.
 * - AppState_SendTelemetry: Prepares and sends telemetry data to the server.
 * - AppState_PollCommands: Polls the server for any pending commands.
 * - AppState_SendAck: Sends acknowledgment for executed commands.
 * - AppState_GoToSleep: Prepares the system to enter low-power sleep mode.
 * The task runs indefinitely, periodically checking the modem state and processing sensor data.
 *
 * @param pvParameters Unused parameter.
 */
static void HTTPTask(void *pvParameters)
{
  UNUSED(pvParameters);

  ST87EC_Lib_Result_t result;
  ST87EC_Lib_Status_t eclib_state;

  SensorsData data;

  printf("\r\n\r\n--------------- NB-IoT HTTP Telemetry application init ---------------\r\n");
  printf("Waiting for network registration...\r\n");

  /* EC lib initialization */
  result = ST87EC_Lib_Init(NULL);
  configASSERT(result == RESULT_OK);

//  memset(&g_currentData, 0, sizeof(g_currentData));
//  memset(&g_currentTelemetry, 0, sizeof(g_currentTelemetry));

  for(;;)
  {
    /* Run EC Lib main sequence */
    result = ST87EC_Lib_Scheduler();

    if(result == RESULT_KO)
    {
      printf("\r\nThere was an error in sequence execution...\r\n");
      printf("\r\nModem reset underway...\r\n");

      // buffer per la risposta HTTP azzerato per evitare di leggere dati vecchi in caso di reset del modem
      memset(httpResponseBuffer, 0, sizeof(httpResponseBuffer));
      memset(httpRawRequest, 0, sizeof(httpRawRequest));
      memset(jsonPayloadBuffer, 0, sizeof(jsonPayloadBuffer));

      // resetto le variabili di stato per evitare di rimanere bloccati in uno stato errato dopo il reset del modem
      httpResponseReady = false;
      g_httpTransferStarted = false;
      g_sleepTimerArmed = false;
      g_postsSentThisCycle = 0;
      g_forceImmediateCycle = false;
      send_immediately = false;
      g_lastErrorResetTimeMs = HAL_GetTick();
      //*********************************************

      ST87EC_Lib_Reset();
      app_state = AppState_Init;

      printf("\r\nModem reset complete...\r\n");
    }

    ST87EC_Lib_GetState(&eclib_state);

    /* Modem is attached to the network and no sequence is currently active */
    if((eclib_state.RegistrationStatus == REGISTERED) && (eclib_state.OnGoingSequence == SEQUENCE_NONE))
    {
      switch(app_state)
      {
        case AppState_Init:
          {
            printf("Registration complete. Attached to NB-IoT network!\r\n\r\n");

            configASSERT(ST87EC_Lib_GetTime(GetTimeCallback, HTTP_TIMEOUT) == RESULT_OK);
            app_state = AppState_RtcInitSequence;
            break;
          }
        case AppState_RtcInitSequence:
          {
            /* Nothing specific to do during RtcInit sequence for now */
            /* State will be changed in GetTimeCallback */
            break;
          }
        case AppState_Idle:
          {
            uint32_t now = GetCurrentTimeSeconds();
            uint16_t buffered = EventBuffer_Count(); // numero di eventi attualmente presenti nel buffer
            bool timeout_send = false;

            /* Cooldown dopo reset modem: non riprovare subito */
              if((HAL_GetTick() - g_lastErrorResetTimeMs) < 5000U)
              {
                vTaskDelay(pdMS_TO_TICKS(200));
                break;
              }
              //*************************************************

            // se sono passati più di TELEMETRY_SEND_INTERVAL_S secondi dall'ultimo invio HTTP, imposto il flag timeout_send a true
            if((now - last_http_send_time_s) >= TELEMETRY_SEND_INTERVAL_S)
            {
              timeout_send = true;
            }

            // se ci sono eventi critici (FSM) nel buffer (buffer non vuoto e send_immediately=true)
            if(send_immediately && buffered > 0)
            {
              printf("Immediate event detected -> start send cycle\r\n");
              g_forceImmediateCycle = true;
              g_postsSentThisCycle = 0;
              app_state = AppState_PrepareTelemetry;
            }
            else if(buffered >= EVENT_BATCH_SIZE) // se il buffer è pieno (numero di eventi >= EVENT_BATCH_SIZE)
            {
              printf("Batch full (%u events) -> start send cycle\r\n", buffered);
              g_forceImmediateCycle = false;
              g_postsSentThisCycle = 0;
              app_state = AppState_PrepareTelemetry;
            }
            else if(timeout_send && buffered > 0) // se è scaduto il timeout di invio e ci sono eventi nel buffer
            {
              printf("Timeout expired with %u buffered events -> start send cycle\r\n", buffered);
              g_forceImmediateCycle = false;
              g_postsSentThisCycle = 0;
              app_state = AppState_PrepareTelemetry;
            }
            else
            {
              vTaskDelay(pdMS_TO_TICKS(200));
            }
            break;
          }
        case AppState_PrepareTelemetry:
          {
            memset(&data, 0, sizeof(data)); // azzera la struct data prima di riempirla con i dati correnti

            // se non ci sono eventi nel buffer, NON invio niente
            if(!EventBuffer_Pop(&data))
            {
              printf("\r\nNo telemetry data available in event buffer\r\n");
              app_state = AppState_GoToSleep;
              break;
            }

            TelemetryPayload_t payload;
            memset(&payload, 0, sizeof(payload));

            // inizializzazioni generate
            memset(httpRawRequest, 0, sizeof(httpRawRequest));
            memset(jsonPayloadBuffer, 0, sizeof(jsonPayloadBuffer));

            /* popolo payload JSON */
            strcpy(payload.deviceId, DEVICE_ID);
            GetCurrentTimestamp(payload.timestamp, sizeof(payload.timestamp));
            payload.temperature = data.sensor_hum_and_temp.temp;
            payload.humidity = data.sensor_hum_and_temp.hum;
            payload.pressure = data.sensor_barometer.pres;
            payload.battery = 100; /* Placeholder momentaneo, anche se mi sa che lascero' cosi' */
            strncpy(payload.orientation, MapOrientationFromEvent(&data), sizeof(payload.orientation) - 1);
            payload.orientation[sizeof(payload.orientation) - 1] = '\0';

            httpResponseReady = false;

            /* Costruzione del payload JSON */
            if(!BuildTelemetryJson(jsonPayloadBuffer, sizeof(jsonPayloadBuffer), &payload))
            {
              /* se il payload JSON non viene costruito correttamente, vado a sleep senza inviare nulla */
              printf("\r\nFailed to build telemetry JSON\r\n");
              app_state = AppState_GoToSleep;
              break;
            }

            /* Costruzione della richiesta HTTP POST - metodo di httpTxObj */
            if(!BuildHttpPostRequest(httpRawRequest, sizeof(httpRawRequest), HTTP_SERVER_IP, TELEMETRY_PATH, jsonPayloadBuffer))
            {
              /* se la richiesta HTTP non viene costruita correttamente, vado a sleep senza inviare nulla */
              printf("\r\nFailed to build HTTP POST request for telemetry\r\n");
              app_state = AppState_GoToSleep;
              break;
            }

            printf("\r\nPrepared telemetry HTTP POST:\r\n%s\r\n", httpRawRequest);

            app_state = AppState_HTTP_Open;
            break;
          }
        case AppState_HTTP_Open:
          {
            /* Open HTTP connection */
            printf(APP_LOG_PREFIX"Opening HTTP connection...");
            result = ST87EC_Lib_NBIOT_HttpOpen(pHost, PortNb, SecureId, Timeout);

            if(result == RESULT_OK)
            {
              g_httpWaitStartTick = HAL_GetTick();
              app_state = AppState_HTTP_WaitOpen;
            }
            else
            {
              printf(APP_LOG_PREFIX"Failed to start HTTP open sequence: %d\r\n", result);
              app_state = AppState_GoToSleep;
            }
            break;
          }
        case AppState_HTTP_WaitOpen:
          {
            if(eclib_state.HttpConnectionStatus != HTTP_NOT_CONNECTED)
            {
              printf(APP_LOG_PREFIX"HTTP connection is open.\r\n");
              app_state = AppState_HTTP_Send;
            }
            else if((HAL_GetTick() - g_httpWaitStartTick) > HTTP_OPEN_WAIT_TIMEOUT_MS)
            {
              printf(APP_LOG_PREFIX"Timeout waiting for HTTP connection open.\r\n");
              app_state = AppState_HTTP_Close;
            }
            break;
          }
        case AppState_HTTP_Send:
          {
            /* Prepare HTTP request data */
            memset(&httpTxObj, 0, sizeof(httpTxObj));
            httpTxObj.pHttpRxCallbackFunc = HttpReceiveCallback;
            httpTxObj.KeepAlive = HTTP_KEEP_ALIVE;
            httpTxObj.Timeout = HTTP_TIMEOUT;
            httpTxObj.pHttpRawInStr = httpRawRequest;

            printf("\r\nSending HTTP request...\r\n");
            printf("\r\nRequest:\r\n%s\r\n", httpRawRequest);
            result = ST87EC_Lib_NBIOT_HttpTransfer(&httpTxObj);

            if(result == RESULT_OK)
            {
              g_httpTransferStarted = true;
              g_httpWaitStartTick = HAL_GetTick();
              app_state = AppState_HTTP_WaitResponse;
            }
            else
            {
              printf(APP_LOG_PREFIX"Failed to start HTTP transfer: %d\r\n", result);
              app_state = AppState_HTTP_Close;
            }

            break;
          }
        case AppState_HTTP_WaitResponse:
          {
            if(httpResponseReady)
            {
              printf(APP_LOG_PREFIX"HTTP response received.\r\n");
              printf(APP_LOG_PREFIX"Response payload: %s\r\n", httpResponseBuffer);

              last_http_send_time_s = GetCurrentTimeSeconds();

              app_state = AppState_HTTP_Close;
            }
            else if((HAL_GetTick() - g_httpWaitStartTick) > HTTP_RESPONSE_WAIT_TIMEOUT_MS)
            {
              printf(APP_LOG_PREFIX"Timeout reached for waiting HTTP response.\r\n");
              app_state = AppState_HTTP_Close;
            }
            break;
          }
        case AppState_HTTP_Close:
          {
            printf(APP_LOG_PREFIX"Closing HTTP connection...\r\n");

            result = ST87EC_Lib_NBIOT_HttpClose(HTTP_TIMEOUT);

            if(result != RESULT_OK)
            {
              printf(APP_LOG_PREFIX"Failed to close HTTP connection: %d\r\n", result);
            }

            g_httpTransferStarted = false;
            g_postsSentThisCycle++; // incremento il contatore dei POST inviati in questo ciclo

            /* Se era un invio immediato per evento critico, invio solo un evento e poi termino il ciclo */
            if(g_forceImmediateCycle)
            {
              send_immediately = false;
              g_forceImmediateCycle = false;
              g_postsSentThisCycle = 0;
              app_state = AppState_GoToSleep;
            }
            else
            {
              /* batch/timeout mode: continua a smaltire al massimo MAX_POST_PER_CYCLE eventi */
              if((EventBuffer_Count() > 0) && (g_postsSentThisCycle < MAX_POST_PER_CYCLE))
              {
                app_state = AppState_PrepareTelemetry;
              }
              else
              {
                send_immediately = false;
                g_postsSentThisCycle = 0;
                app_state = AppState_GoToSleep;
              }
            }
            break;
          }

        case AppState_GoToSleep:
          {
            if(!g_sleepTimerArmed)
            {
              printf("\r\nApplication is ready to go to sleep for %ds...\r\n", APPLICATION_SLEEP_TIME_S);
              printf("Waiting for ST87M01 to go to sleep as well...\r\n");

              __HAL_RCC_LPTIM1_CLKAM_ENABLE();
              __HAL_RCC_RTCAPB_CLKAM_ENABLE();

              HAL_LPTIM_TimeOut_Start_IT(&hlptim1, LPTIM_PERIOD);
              g_sleepTimerArmed = true;
            }

            app_state = AppState_Idle;
            break;
          }

//        case AppState_GoToSleep:
//          {
//            printf("\r\nApplication is ready to go to sleep for %ds...\r\n", APPLICATION_SLEEP_TIME_S);
//            printf("Waiting for ST87M01 to go to sleep as well...\r\n");
//
//            /* Enable LPTIM1 running even when STOP2 mode is active */
//            /* ToDo: move this out of main loop?? */
//            __HAL_RCC_LPTIM1_CLKAM_ENABLE();
//            __HAL_RCC_RTCAPB_CLKAM_ENABLE();
//
//            HAL_LPTIM_TimeOut_Start_IT(&hlptim1, LPTIM_PERIOD);
//
//            app_state = AppState_Idle;
//            break;
//          }
//        case AppState_HTTP_Open:
//          {
//            /* Open HTTP connection */
//            printf(APP_LOG_PREFIX"Opening HTTP connection...");
//            result = ST87EC_Lib_NBIOT_HttpOpen(pHost, PortNb, SecureId, Timeout);
//
//            if(result == RESULT_OK)
//            {
//              app_state = AppState_HTTP_SendData;
//            }
//            else
//            {
//              printf(APP_LOG_PREFIX"Failed to open HTTP connection: %d\r\n", result);
//              app_state = AppState_GoToSleep;
//            }
//            break;
//          }
//        case AppState_HTTP_SendData:
//          {
//            /* Connection not open */
//            if(eclib_state.HttpConnectionStatus == HTTP_NOT_CONNECTED)
//            {
//              printf(APP_LOG_PREFIX"HTTP connection not open.\r\n");
//              app_state = AppState_HTTP_Open;
//              break;
//            }
//
//            memset(&httpTxObj, 0, sizeof(httpTxObj));
//            httpTxObj.pHttpRxCallbackFunc = HttpReceiveCallback;
//            httpTxObj.KeepAlive = HTTP_KEEP_ALIVE;
//            httpTxObj.Timeout = HTTP_TIMEOUT;
//            httpTxObj.pHttpRawInStr = httpRawRequest;
//
//            printf(APP_LOG_PREFIX"Starting HTTP transfer...\r\n");
//            result = ST87EC_Lib_NBIOT_HttpTransfer(&httpTxObj);
//
//            if(result == RESULT_OK)
//            {
//              printf(APP_LOG_PREFIX"HTTP transfer started successfully.\r\n");
//              httpTransferStarted = true;
//              app_state = AppState_HTTP_WaitResponse;
//            }
//            else
//            {
//              printf(APP_LOG_PREFIX"Failed to start HTTP transfer: %d\r\n", result);
//              app_state = AppState_HTTP_Close;
//
//            }
//            break;
//          }
//        case AppState_HTTP_WaitResponse:
//          {
//            if(httpResponseReady)
//            {
//              printf(APP_LOG_PREFIX"HTTP response received.\r\n");
//              last_http_send_time_s = GetCurrentTimeSeconds();
//              send_immediately = false;
//              app_state = AppState_HTTP_Close;
//            }
//            break;
//          }
//        case AppState_HTTP_Close:
//          {
//            /* Close HTTP connection */
//            printf(APP_LOG_PREFIX"Closing HTTP connection...\r\n");
//            result = ST87EC_Lib_NBIOT_HttpClose(HTTP_TIMEOUT);
//
//            if(result != RESULT_OK)
//            {
//              printf(APP_LOG_PREFIX"Failed to close HTTP connection: %d\r\n", result);
//
//            }
//
//            httpTransferStarted = false;
//            app_state = AppState_GoToSleep;
//            break;
//          }
//        case AppState_PollCommands:
//        {
//          char commandPath[128];
//
//          snprintf(commandPath, sizeof(commandPath),
//                   "/api/v1/devices/%s/commands/pending",
//                   DEVICE_ID);
//
//          if(!BuildHttpGetRequest(httpRawRequest, sizeof(httpRawRequest), HTTP_SERVER_IP, commandPath))
//          {
//            printf("\r\nFailed to build HTTP GET request for command polling\r\n");
//            app_state = AppState_GoToSleep;
//            break;
//          }
//
//          printf("\r\nPolling commands...\r\n");
//          printf("\r\nRequest:\r\n%s\r\n", httpRawRequest);
//
//          g_hasPendingCommand = false;
//
//          if(HttpOpenConnection())
//          {
//            if(HttpSendRawRequest(httpRawRequest))
//            {
//              if(ParseForceMeasurementCommand(httpResponseBuffer, &g_pendingCommand))
//              {
//                g_hasPendingCommand = true;
//                printf("\r\nPending command detected: %s (id=%d)\r\n",
//                       g_pendingCommand.type, g_pendingCommand.id);
//              }
//            }
//            HttpCloseConnection();
//          }
//          else
//          {
//            printf("\r\nHTTP open connection failed for command polling\r\n");
//          }
//
//          if(g_hasPendingCommand)
//          {
//            app_state = AppState_SendAck;
//          }
//          else
//          {
//            app_state = AppState_GoToSleep;
//          }
//
//          break;
//        }
//
//        case UdpApiState_TransferComplete:
//          {
//            // TEST ////////////////////////////////////////////////////////
////            printf("[UDPTask] State = TransferComplete\r\n");
//            /////////////////////////////////////////////////////////////
//
//            printf("\r\nTransfer complete...\r\n");
//            udp_state = UdpApiState_GoToSleep;
//            break;
//          }
        default:
          {
            app_state = AppState_GoToSleep;
            break;
          }
      }
    }
    LedBlinking(&eclib_state);
  }
}


/**
 * @brief Prepares the system for entering sleep mode.
 *
 * This function is called before the system enters sleep mode. It performs
 * any necessary preparations to ensure the system can enter and wake up
 * from sleep mode correctly.
 *
 * it supports two low power modes:
 * - Sleep mode: the CPU is stopped, peripherals are kept running. Wake up is done by any interrupt.
 * - Stop2 mode: most of the system is powered down, only LPTIM and RTC are kept running.
 *               Wake up is done by LPTIM or RTC alarm or external interrupt.
 *
 * @param ulExpectedIdleTime The expected duration of the idle time in milliseconds.
 */
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
  if(sensors_init_state == SensorsInitialized)
  {
#ifdef LOW_POWER_MODE_SLEEP
    __WFI();
#endif
#ifdef LOW_POWER_MODE_STOP2
    printf("\r\nGoing to sleep mode...\r\n");
    /* HAL_GPIO_WritePin(STM32_VSENSOR_GPIO_Port, STM32_VSENSOR_Pin, GPIO_PIN_RESET); */

    HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin, GPIO_PIN_RESET);

    HAL_SuspendTick();
    HAL_NVIC_ClearPendingIRQ(TIM7_IRQn);

    /* to enable LPTIM1 running even when STOP2 mode is active */
    __HAL_RCC_LPTIM1_CLKAM_ENABLE();
    __HAL_RCC_RTCAPB_CLKAM_ENABLE();

    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
#endif
  }
}

/**
 * @brief Perform post-sleep processing.
 *
 * This function is called after the system wakes up from low power modes.
 *
 * @param ulExpectedIdleTime The expected idle time in milliseconds.
 */
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
  if(sensors_init_state == SensorsInitialized)
  {
#ifdef LOW_POWER_MODE_STOP2
    SystemClock_Config();
    HAL_ResumeTick();
    printf("\r\nWake up complete...\r\n");
#endif
  }
}

/**
 * @brief  Compare match callback in non blocking mode
 *         This function is called when the LPTIM compare match event occurs.
 *         The duration is defined by the LPTIM period value and it can be
 *         set by the user by modifying the APPLICATION_SLEEP_TIME_S define.
 * @param  hlptim: LPTIM handle
 * @retval None
 */
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Timeout was reached, resume the task */
  if(hlptim == APP_LPTIM)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    HAL_LPTIM_TimeOut_Stop_IT(&hlptim1);

    g_sleepTimerArmed = false; // value di g_sleepTimerArmed a false per evitare che il timer venga riattivato in modo ricorsivo

    printf(">>> LPTIM CompareMatch: sending eEventTimer to queue\r\n");

    eEventType_t event = eEventTimer;
    configASSERT(xDataQueue != NULL);
    xQueueSendFromISR(xDataQueue, &event, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/**
 * INT1 is used for MLC output (board position),
 * while INT2 is used for FSM output (impact/free fall).
 *
 * OSS.: "event = eEventTilt;"    // deleted; Tilt event not used anymore
 *
 * hlptim1: handler of LPTIM1 timer.
 * .instance: pointer to the base register of the LPTIM timer (timer event),
 *            if !0 --> timer active, so let's stop it (pin raised --> stopped/raised again)
 *
 * @brief  EXTI line rising edge detection callback.
 * @param  GPIO_Pin: Specifies the pins connected EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  bool valid_pin = (GPIO_Pin == LIS2DUXS12_INT1_Pin) || (GPIO_Pin == LIS2DUXS12_INT2_Pin);
  eEventType_t event;

  if(GPIO_Pin == LIS2DUXS12_INT1_Pin)
  {
    event = eEventMLC1; // event generated by INT1 (MLC1_SRC)
  }

  if(GPIO_Pin == LIS2DUXS12_INT2_Pin)
  {
    event = eEventFSM; // event generated by INT2 (FSM_OUTS)
  }

  if(hlptim1.Instance != 0 && valid_pin)
  {
    HAL_LPTIM_TimeOut_Stop_IT(&hlptim1);  // stop dell'interrupt del timer

    g_sleepTimerArmed = false;  // value di g_sleepTimerArmed a false per evitare che il timer venga riattivato in modo ricorsivo

    configASSERT(xDataQueue != NULL);
    xQueueSendFromISR(xDataQueue, &event, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/**
 * @brief Retrieves the current time in seconds since the Unix epoch.
 *
 * This function reads the current time from the RTC, converts it to a `struct tm`,
 * and then calculates the number of seconds since January 1, 1970 (Unix epoch).
 *
 * @return The current time in seconds since the Unix epoch.
 */
uint32_t GetCurrentTimeSeconds(void)
{
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;

  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

  struct tm t;
  t.tm_sec = time.Seconds;
  t.tm_min = time.Minutes;
  t.tm_hour = time.Hours;
  t.tm_mday = date.Date;
  t.tm_mon = date.Month - 1;
  t.tm_year = date.Year + 100;

  time_t epoch = mktime(&t);
  return (uint32_t) epoch;
}

/**
 * @brief Callback function called by the EC library when the time is retrieved the ST87 module.
 *
 * @param[in] pString Pointer to a constant character string containing the time information.
 */
static void GetTimeCallback(char const *const pString)
{
  /* Example response from the modem "24/10/22,10:17:44+08" */
  char tmp_str[] = "xx/xx/xx,xx:xx:xx";
  struct tm result;

  printf("Current time: %s\r\n", pString);

  /* copying strlen(tmp_str) characters skips +-hh timezone information
   copying from pString + 1 skips '"' character */
  memcpy(tmp_str, pString + 1, strlen(tmp_str));

  strptime(tmp_str, "%y/%m/%d,%H:%M:%S", &result);

  RTC_TimeTypeDef time =
  {
      0 };
  time.Hours = result.tm_hour;
  time.Minutes = result.tm_min;
  time.Seconds = result.tm_sec;
  time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  time.StoreOperation = RTC_STOREOPERATION_RESET;
  configASSERT((HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK));

  RTC_DateTypeDef date =
  {
      0 };
  date.WeekDay = (result.tm_wday == 0) ? RTC_WEEKDAY_SUNDAY : result.tm_wday; // HAL library Sunday is 7, tm_wday Sunday is 0
  date.Month = result.tm_mon + 1;
  date.Date = result.tm_mday;
  date.Year = result.tm_year - 100;

  configASSERT((HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) == HAL_OK));

  uint32_t now = GetCurrentTimeSeconds();
  last_http_send_time_s = now;

  /* Wakeup the sensors data task */
//  eEventType_t event = eEventFirst;
//  configASSERT(xDataQueue != NULL);
//  xQueueSend(xDataQueue, &event, portMAX_DELAY);

  /* Wakeup the sensors data task only once at first boot */
  if(!g_firstBootSampleSent)
  {
    eEventType_t event = eEventFirst;
    configASSERT(xDataQueue != NULL);
    xQueueSend(xDataQueue, &event, portMAX_DELAY);
    g_firstBootSampleSent = true;
  }

  app_state = AppState_Idle;
}

/**
 * @brief Controls the blinking of a LED based on the application status.
 */
static void LedBlinking(ST87EC_Lib_Status_t *eclib_state)
{
  static uint32_t start_time = 0;
  static bool led_on = false;
  uint32_t on_time, off_time;

  if(eclib_state->RegistrationStatus != REGISTERED)
  {
    on_time = LED_TIME_ON_IDLE;
    off_time = LED_TIME_OFF_IDLE;
  }
  else if(eclib_state->OnGoingSequence == SEQUENCE_NONE)
  {
    on_time = LED_TIME_ON_ATTACHED;
    off_time = LED_TIME_OFF_ATTACHED;
  }
  else
  {
    on_time = LED_TIME_ON_SEQUENCE;
    off_time = LED_TIME_OFF_SEQUENCE;
  }

  if(led_on)
  {
    if(HAL_GetTick() - start_time > on_time)
    {
      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
      led_on = false;
      start_time = HAL_GetTick();
    }
  }
  else
  {
    if(HAL_GetTick() - start_time > off_time)
    {
      HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
      led_on = true;
      start_time = HAL_GetTick();
    }
  }
}

/******************************************************************************
 * HTTP SERVER CONFIGURATION - IoT MONITORING SYS
 ******************************************************************************/
/**
 * Callback function executed when an HTTP packet is received.
 *
 * This function is called by the HTTP library when an HTTP response is received.
 * It copies the received data into a buffer, sets a flag indicating that the response is ready,
 * and prints the received payload to the console. If no data is received, it clears the buffer and sets the response ready flag.
 *
 * @param receivedData Pointer to the received HTTP data. If NULL, no data was received.
 * */
static void HttpReceiveCallback(char const *const receivedData)
{
  if(receivedData != NULL)
  {
    // Copy the received data into the httpResponseBuffer, ensuring it is null-terminated
    strncpy(httpResponseBuffer, receivedData, sizeof(httpResponseBuffer) - 1);
    httpResponseBuffer[sizeof(httpResponseBuffer) - 1] = '\0';
    httpResponseReady = true;

    /* Process the received data */
    printf(APP_LOG_PREFIX"[HTTP RX] - Received HTTP data.");
    printf(APP_LOG_PREFIX"Payload: %s\n", httpResponseBuffer);
  }
  else
  {
    httpResponseBuffer[0] = '\0';
    httpResponseReady = true;
    printf(APP_LOG_PREFIX"[HTTP RX] - No data received.\r\n");
  }

//  app_state = AppState_HTTP_Close;
}

/**
 * helper function to wait for an HTTP response within a specified timeout.
 *
 * @param timeoutMs The maximum time to wait for the HTTP response in milliseconds.
 * @return true if the HTTP response is ready within the timeout, false otherwise.
 * */
static bool WaitHttpResponse(uint32_t timeoutMs)
{
  uint32_t start = HAL_GetTick();

  while((HAL_GetTick() - start) < timeoutMs)
  {
    if(httpResponseReady)
    {
      return true;
    }
    vTaskDelay(pdMS_TO_TICKS(HTTP_POLL_DELAY_MS));
  }

  return false;
}

/**
 * function to get the current timestamp in ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
 *
 * @param buffer Pointer to the buffer where the timestamp will be written.
 * @param len Length of the buffer.
 * */
static void GetCurrentTimestamp(char *buffer, size_t len)
{
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;

  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

  snprintf(buffer, len, "20%02u-%02u-%02uT%02u:%02u:%02uZ", date.Year, date.Month, date.Date, time.Hours, time.Minutes, time.Seconds);
}

/**
 * function to map the orientation from the event data to a string representation.
 *
 * @param data Pointer to the SensorsData structure containing the event data.
 * @return A string representing the orientation based on the event data.
 * */
static const char* MapOrientationFromEvent(const SensorsData *data)
{
  if(data->fsm_impact)
  {
    return "IMPACT";
  }
  if(data->fsm_free_fall)
  {
    return "FREE_FALL";
  }

  switch(data->mlc_output)
  {
    case 0x00:
      return "STATIONARY_UPRIGHT";
    case 0x04:
      return "STATIONARY_NOT_UPRIGHT";
    case 0x08:
      return "IN_MOTION";
    case 0x0C:
      return "SHAKEN";
    default:
      return "UNKNOWN";
  }
}

/**
 * Function to build a JSON string for telemetry data.
 *
 * @param buffer Pointer to the buffer where the JSON string will be written.
 * @param len Length of the buffer.
 * @param payload Pointer to the TelemetryPayload_t structure containing the telemetry data.
 * @return true if the JSON string was successfully built and fits in the buffer, false otherwise.
 * */
static bool BuildTelemetryJson(char *buffer, size_t len, const TelemetryPayload_t *payload)
{
  int written = snprintf(buffer, len, "{"
                         "\"deviceId\":\"%s\","
                         "\"timestamp\":\"%s\","
                         "\"temperature\":%.2f,"
                         "\"humidity\":%.2f,"
                         "\"pressure\":%.2f,"
                         "\"battery\":%u,"
                         "\"orientation\":\"%s\""
                         "}",
                         payload->deviceId, payload->timestamp, payload->temperature, payload->humidity, payload->pressure, payload->battery,
                         payload->orientation);

  return (written > 0) && ((size_t) written < len);
}

/**
 * function to build a JSON string for command acknowledgment.
 *
 * @param buffer Pointer to the buffer where the JSON string will be written.
 * @param len Length of the buffer.
 * @param ack Pointer to the CommandAckPayload_t structure containing the acknowledgment data.
 * @return true if the JSON string was successfully built and fits in the buffer, false otherwise.
 * */
static bool BuildAckJson(char *buffer, size_t len, const CommandAckPayload_t *ack)
{
  int written = snprintf(buffer, len, "{"
                         "\"status\":\"%s\","
                         "\"ackAt\":\"%s\","
                         "\"resultMessage\":\"%s\""
                         "}",
                         ack->status, ack->ackAt, ack->resultMessage);

  return (written > 0) && ((size_t) written < len);
}

/**
 * @brief Builds an HTTP POST request with a JSON body.
 *
 * This function formats an HTTP POST request string with the specified host, path, and JSON body.
 *
 * @param[out] out Pointer to the output buffer where the HTTP request will be written.
 * @param[in] outLen Length of the output buffer.
 * @param[in] host The host name or IP address of the server.
 * @param[in] path The path of the resource on the server.
 * @param[in] jsonBody The JSON body to be included in the POST request.
 * @return true if the HTTP request was successfully built and fits in the output buffer, false otherwise.
 * */
static bool BuildHttpPostRequest(char *out, size_t outLen, const char *host, const char *path, const char *jsonBody)
{
  size_t bodyLen = strlen(jsonBody);

  int written = snprintf(out, outLen, "POST %s HTTP/1.1\r\n"
                         "Host: %s\r\n"
                         "User-Agent: ST87EC/1.0\r\n"
                         "Accept: */*\r\n"
                         "Content-Type: application/json\r\n"
                         "Content-Length: %u\r\n"
                         "Connection: close\r\n"
                         "\r\n"
                         "%s",
                         path, host, (unsigned) bodyLen, jsonBody);

  return (written > 0) && ((size_t) written < outLen);
}

/**
 * @brief Builds an HTTP GET request.
 *
 * This function formats an HTTP GET request string with the specified host and path.
 *
 * @param[out] out Pointer to the output buffer where the HTTP request will be written.
 * @param[in] outLen Length of the output buffer.
 * @param[in] host The host name or IP address of the server.
 * @param[in] path The path of the resource on the server.
 * @return true if the HTTP request was successfully built and fits in the output buffer, false otherwise.
 * */
static bool BuildHttpGetRequest(char *out, size_t outLen, const char *host, const char *path)
{
  int written = snprintf(out, outLen, "GET %s HTTP/1.1\r\n"
                         "Host: %s\r\n"
                         "Connection: close\r\n"
                         "\r\n",
                         path, host);

  return (written > 0) && ((size_t) written < outLen);
}

//static bool HttpOpenConnection(void)
//{
//  ST87EC_Lib_Result_t result = ST87EC_Lib_NBIOT_HttpOpen(HTTP_SERVER_IP,
//  HTTP_SERVER_PORT,
//                                                         HTTP_SECURE_ID, HTTP_TIMEOUT);
//  return (result == RESULT_OK);
//}
//
//static bool HttpCloseConnection(void)
//{
//  ST87EC_Lib_Result_t result = ST87EC_Lib_NBIOT_HttpClose(HTTP_TIMEOUT);
//  return (result == RESULT_OK);
//}

/**
 * @brief Sends a raw HTTP request using the ST87EC library.
 *
 * This function prepares the HTTP transfer object with the provided raw request string and initiates the HTTP transfer using the ST87EC library. It also sets up a callback function to handle the HTTP response.
 *
 * @param[in] rawRequest Pointer to the raw HTTP request string to be sent.
 * @return true if the HTTP transfer was initiated successfully, false otherwise.
 * */
static bool HttpSendRawRequest(const char *rawRequest)
{
  memset(&httpTxObj, 0, sizeof(httpTxObj));
  httpTxObj.pHttpRxCallbackFunc = HttpReceiveCallback;
  httpTxObj.KeepAlive = HTTP_KEEP_ALIVE;
  httpTxObj.Timeout = HTTP_TIMEOUT;
  httpTxObj.pHttpRawInStr = rawRequest;

  httpResponseReady = false;
  memset(httpResponseBuffer, 0, sizeof(httpResponseBuffer));

  return (ST87EC_Lib_NBIOT_HttpTransfer(&httpTxObj) == RESULT_OK);
}

/**
 * @brief Parses a FORCE_MEASUREMENT command from the HTTP response.
 *
 * This function checks if the HTTP response contains a FORCE_MEASUREMENT command and extracts the relevant information into a PendingCommand_t structure.
 *
 * @param[in] response Pointer to the HTTP response string.
 * @param[out] cmd Pointer to the PendingCommand_t structure where the parsed command information will be stored.
 * @return true if a FORCE_MEASUREMENT command was found and parsed successfully, false otherwise.
 * */
static bool ParseForceMeasurementCommand(const char *response, PendingCommand_t *cmd)
{
  if(response == NULL || cmd == NULL)
  {
    return false;
  }

  if(strstr(response, "\"type\":\"FORCE_MEASUREMENT\"") == NULL && strstr(response, "\"type\": \"FORCE_MEASUREMENT\"") == NULL)
  {
    return false;
  }

  memset(cmd, 0, sizeof(*cmd));
  cmd->id = 0;
  strcpy(cmd->device_id, DEVICE_ID);
  strcpy(cmd->type, "FORCE_MEASUREMENT");
  strcpy(cmd->payload, "immediate");
  strcpy(cmd->status, "PENDING");

  char *idPos = strstr(response, "\"id\":");
  if(idPos != NULL)
  {
    sscanf(idPos, "\"id\": %d", &cmd->id);
    if(cmd->id == 0)
    {
      sscanf(idPos, "\"id\":%d", &cmd->id);
    }
  }

  return true;
}

