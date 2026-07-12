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
typedef enum HTTP_API_State
{
  HttpApiState_Init = 0,
  HttpApiState_RtcInitSequence,
  HttpApiState_Idle,
  HttpApiState_PrepareFixedRequest, // stato di TEST --> vado a commentare il task SensorsDataTask
  HttpApiState_Done,                // stato di TEST --> vado a commentare il task SensorsDataTask
  HttpApiState_PrepareTelemetry,
  HttpApiState_OpenConnection,
  HttpApiState_WaitOpen,
  HttpApiState_SendRequest,
  HttpApiState_WaitResponse,
  HttpApiState_CloseConnection,
  HttpApiState_ArmPeriodicTimer,
//  HttpApiState_GoToSleep,

  /* ---pending states --- */
  HttpApiState_OpenPendingCommandsConnection,
  HttpApiState_WaitPendingCommandsOpen,
  HttpApiState_SendPendingCommandsRequest,
  HttpApiState_WaitPendingCommandsResponse,
  HttpApiState_ClosePendingCommandsConnection,
} HTTP_API_State;

/* variabile per gestire il flusso di dati HTTP */
typedef enum
{
  HttpFlow_Telemetry = 0,
  HttpFlow_PendingCommands
} HttpFlow_t;

/* Structure to hold pending commands received from the server */
typedef struct
{
  uint32_t id;
  char type[32];
  char payload[64];
  bool valid;
} PendingCommand_t;

/* Private define ------------------------------------------------------------*/

/* ----------------------------------------*/
/* Configurable parameters*/

/* Uncomment one of the following lines to enable the corresponding low power state */
//#define LOW_POWER_MODE_STOP2
#define LOW_POWER_MODE_SLEEP

/* Select the application sleep time */
//#define APPLICATION_SLEEP_TIME_S        50U     //----------------------------------------- MODIFICATO GIU' PER TIMING DI INVIO MEX UDP
// --- HTTP parameters ---
#define HTTP_SERVER_IP                   "10.68.87.69"  // VPN raspberry IP
#define HTTP_SERVER_PORT                 8080
#define TELEMETRY_PATH                  "/api/v1/telemetry"
//#define TELEMETRY_PATH                  "/post"

#define HTTP_SECURE_ID                  -1

#define HTTP_TIMEOUT_MS                 20000U
#define HTTP_TASK_PRIORITY              3U
#define SENSOR_TASK_PRIORITY            4U

#define HTTP_TASK_STACK_SIZE            (configMINIMAL_STACK_SIZE * 12)
#define SENSOR_TASK_STACK_SIZE          (configMINIMAL_STACK_SIZE * 2)

#define HTTP_JSON_BUFFER_SIZE           384U
#define HTTP_REQUEST_BUFFER_SIZE        512U

#define APP_LOG_PREFIX            "\r\n*** "

/* ----------------------------------------*/

/* Sleep time converted to us */
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

#define ST87_UDP_TASK_MSG_BUF_SIZE      (256U)

#define REQUEST_TIMEOUT_DEFAULT         (10000U) // ms

#define UDP_TASK_PRIORITY               3U
#define SENSOR_TASK_PRIORITY            4U

/*-- event management --*/
#define EVENT_BATCH_SIZE                5U          // size of the event batch to be sent in each UDP packet
#define SENSORS_SAMPLE_INTERVAL_S       20U         // 20 seconds
#define UDP_SEND_INTERVAL_S             (2U * 60U)  // 5 min - send the batch every UDP_SEND_INTERVAL_S seconds
// Ogni 60s il LPTIM genera il CompareMatch → HAL_LPTIM_CompareMatchCallback → queue eEventTimer.
#define APPLICATION_SLEEP_TIME_S        SENSORS_SAMPLE_INTERVAL_S

/* Tasks stack size */
#define UDP_TASK_STACK_SIZE             (configMINIMAL_STACK_SIZE * 12)
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

// Buffer to store the data read from sensors before sending them via UDP
static SensorsData batch[EVENT_BATCH_SIZE];
static uint16_t batch_count = 0;

/* variable to keep track of the age of the batch: if the batch is not empty and either a new sample is added
 * that is PERIODIC_SAMPLE_INTERVAL_S min old or more, or the batch reaches the max age of MAX_BATCH_AGE_S min,
 * the batch will be sent anyway even if it's not full
 */
static uint32_t last_batch_sample_time_s = 0; // quando ho aggiunto l’ultimo evento al batch
static uint32_t last_udp_send_time_s = 0; // quando ho fatto l’ultimo invio UDP.

extern bool send_immediately;  // flag to indicate whether to send the UDP packet immediately after receiving an event, without waiting to fill the batch

/* State machine variable */
volatile HTTP_API_State http_state = HttpApiState_Init;
static volatile HttpFlow_t g_httpFlow = HttpFlow_Telemetry;

/* Sampling period in seconds chosen by the user throgh web dashboard command */
static uint32_t g_sampling_period_s = 20U;

// --- HTTP variables ---
static volatile bool http_response_received = false;
static char g_lastHttpResponse[768];

/* Buffers for HTTP JSON and request data */
static char g_http_json[HTTP_JSON_BUFFER_SIZE];
static char g_http_request[HTTP_REQUEST_BUFFER_SIZE];

/* Variable to hold the telemetry data that is pending to be sent via HTTP */
static SensorsData g_pendingTelemetry;  // struct to hold the telemetry data that is pending to be sent via HTTP
static bool g_hasPendingTelemetry = false;  // flag to indicate if there is pending telemetry data to be sent via HTTP

/* Variable to hold/store the pending command received from the server */
static PendingCommand_t g_pendingCommand;

/* Buffers for HTTP command request and response */
static char g_http_cmd_request[HTTP_REQUEST_BUFFER_SIZE];
static char g_http_cmd_response[HTTP_REQUEST_BUFFER_SIZE];

static ST87EC_Lib_HttpTransferObject_t httpTxObj;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void HTTPTask(void *pvParameters);

static void LedBlinking(ST87EC_Lib_Status_t *eclib_state);

static void HTTPReceiveCallback(char const *const receivedData);
static void GetTimeCallback(char const *const pString);

static const char* GetOrientationString(const SensorsData *data);

/* Helper functions for building HTTP requests and JSON payloads */
static bool BuildHttpPostRequest(const char *json, char *request, size_t request_size);
static void BuildIsoTimestamp(char *out, size_t out_size);
static bool BuildTelemetryJson(const SensorsData *data, char *json, size_t json_size);
static bool BuildFixedTelemetryJson(char *json, size_t json_size);
static bool BuildPendingCommandsGetRequest(char *request, size_t request_size);

/* Helper functions for time & parsing JSON and extracting values */
static uint32_t GetCurrentTimeSeconds(void);
static uint32_t GetLptimPeriodFromSeconds(uint32_t seconds);
static const char* SkipSpaces(const char *p);
static bool ExtractJsonUint32(const char *json, const char *key, uint32_t *value);
static bool ExtractJsonString(const char *json, const char *key, char *out, size_t out_size);
static bool ParsePendingCommand(const char *httpPayload, PendingCommand_t *cmd);

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
  /* Creation of the Queue for 4 Tasks */
  xDataQueue = xQueueCreate(4, sizeof(eEventType_t)); // (LUNGHEZZA CODA, DIMENSIONE ELEMENTO NELLA CODA)
  configASSERT(xDataQueue != NULL);

  BaseType_t xReturned; // var per controllare l'esito di creazione del task

  xReturned = xTaskCreate(HTTPTask, "http_task",
  HTTP_TASK_STACK_SIZE,
                          NULL,
                          HTTP_TASK_PRIORITY,
                          &xHTTPTaskHandle);

  configASSERT(xReturned == pdPASS);  // verifica di corretta creazione del task

// **************** COMMENTO PER FARE TEST DEL TASK HTTP, SENZA I SENSORI ****************
  xReturned = xTaskCreate(SensorsDataTask, "sensors_task",
  SENSOR_TASK_STACK_SIZE,
                          NULL,
                          SENSOR_TASK_PRIORITY,
                          &xSensorsDataTaskHandle);

  configASSERT(xReturned == pdPASS);
// ***************************************************************************************
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
 * @brief UDP task for NB-IoT UDP application.
 *
 * This FreeRTOS task manages the UDP communication over NB-IoT using the ST87EC library.
 * It initializes the modem, waits for network registration, and then enters a state machine
 * to handle sensor data transmission via UDP. The task interacts with other tasks via a message buffer,
 * formats sensor data, and sends it to a predefined UDP server. It also manages modem resets,
 * sleep cycles, and handles the UDP transfer completion via callbacks.
 *
 * State machine overview:
 * - UdpApiState_Init: Waits for network registration and initializes RTC.
 * - UdpApiState_RtcInitSequence: Placeholder for RTC initialization sequence.
 * - UdpApiState_GoToSleep: Prepares the system to enter low-power sleep mode.
 * - UdpApiState_Idle: Waits for sensor data and sends it over UDP when available.
 * - UdpApiState_TransferSequence: Waits for UDP transfer to complete.
 * - UdpApiState_TransferComplete: Handles post-transfer actions and returns to sleep.
 *
 * The task runs indefinitely, periodically checking the modem state and processing sensor data.
 *
 * @param pvParameters Unused parameter.
 */
static void HTTPTask(void *pvParameters)
{
  UNUSED(pvParameters);

  ST87EC_Lib_Result_t result;
  ST87EC_Lib_Status_t eclib_state;

  printf("\r\n\r\n--------------- NB-IoT HTTP application init ---------------\r\n");
  printf("Waiting for network registration...\r\n");

  /* EC lib initialization */
  result = ST87EC_Lib_Init(NULL);
  configASSERT(result == RESULT_OK);

  /* HTTP object initialization */
  memset(&httpTxObj, 0, sizeof(httpTxObj));
  httpTxObj.pHttpRxCallbackFunc = HTTPReceiveCallback;
  httpTxObj.KeepAlive = 0;
  httpTxObj.Timeout = HTTP_TIMEOUT_MS;

  // Initialize the buffers for HTTP JSON and request data
  memset(g_http_json, 0, sizeof(g_http_json));
  memset(g_http_request, 0, sizeof(g_http_request));

  for(;;)
  {
    /* Run EC Lib main sequence */
    result = ST87EC_Lib_Scheduler();

    if(result == RESULT_KO)
    {
      printf("\r\nThere was an error in sequence execution...\r\n");
      printf("\r\nModem reset underway...\r\n");
      ST87EC_Lib_Reset();
      http_state = HttpApiState_Init;
      g_hasPendingTelemetry = false;  // reset pending telemetry flag after modem reset

      //** clear the event buffer to avoid sending stale data after modem reset ******
      void EventBuffer_Clear(void);
      //******************************************************************************

      printf("\r\nModem reset complete...\r\n");
    }

    ST87EC_Lib_GetState(&eclib_state);

    /* Modem is attached to the network and no sequence is currently active */
    if((eclib_state.RegistrationStatus == REGISTERED) && (eclib_state.OnGoingSequence == SEQUENCE_NONE))
    {
      switch(http_state)
      {
        case HttpApiState_Init:
          {
            printf("Registration complete. Attached to NB-IoT network!\r\n\r\n");

            configASSERT(ST87EC_Lib_GetTime(GetTimeCallback, HTTP_TIMEOUT_MS) == RESULT_OK);
            http_state = HttpApiState_RtcInitSequence;
            break;
          }
        case HttpApiState_RtcInitSequence:
          {
            /* Nothing specific to do during RtcInit sequence for now */
            /* State will be changed in GetTimeCallback */
            break;
          }
        case HttpApiState_Idle:
          {
            SensorsData ev;

            // Check if there are pending telemetry events in the queue
            if(!g_hasPendingTelemetry && EventBuffer_Pop(&ev))
            {
              g_pendingTelemetry = ev;  // sensorsData struct to hold the telemetry data that is pending to be sent via HTTP
              g_hasPendingTelemetry = true; // sbloacca tutto appena ho un evento in coda da inviare
              http_state = HttpApiState_PrepareTelemetry;
            }
            break;
          }
//        case HttpApiState_PrepareFixedRequest:  // stato di TEST --> vado a commentare il task SensorsDataTask e lo stato HttpApiState_PrepareTelemetry
//          {
//            // Prepare a fixed HTTP request (for testing purposes)
//            memset(g_http_json, 0, sizeof(g_http_json));
//            memset(g_http_request, 0, sizeof(g_http_request));
//
//            if(!BuildFixedTelemetryJson(g_http_json, sizeof(g_http_json)))
//            {
//              printf("\r\nFailed to build fixed JSON request.\r\n");
//              http_state = HttpApiState_Done;
//              break;
//            }
//
//            if(!BuildHttpPostRequest(g_http_json, g_http_request, sizeof(g_http_request)))
//            {
//              printf("\r\nFailed to build HTTP POST request.\r\n");
//              http_state = HttpApiState_Done;
//              break;
//            }
//
//            printf("\r\nHTTP JSON:\r\n%s\r\n", g_http_json);
//            printf("\r\nHTTP REQUEST:\r\n%s\r\n", g_http_request);
//
//            httpTxObj.pHttpRawInStr = g_http_request;
//            http_response_received = false;
//            http_state = HttpApiState_OpenConnection;
//            break;
//          }
        case HttpApiState_PrepareTelemetry:
          {
            if(!g_hasPendingTelemetry)
            {
              http_state = HttpApiState_Idle;
              break;
            }

            memset(g_http_json, 0, sizeof(g_http_json));
            memset(g_http_request, 0, sizeof(g_http_request));

            // print del tipo di evento (pending) che sto per inviare
            printf("\r\nPreparing telemetry for event type %d...\r\n", g_pendingTelemetry.event_type);

            if(!BuildTelemetryJson(&g_pendingTelemetry, g_http_json, sizeof(g_http_json)))
            {
              // If building the telemetry JSON fails, reset the pending telemetry flag and go back to idle state
              g_hasPendingTelemetry = false;
              http_state = HttpApiState_Done;
              break;
            }

            if(!BuildHttpPostRequest(g_http_json, g_http_request, sizeof(g_http_request)))
            {
              // If building the HTTP POST request fails, reset the pending telemetry flag and go back to idle state
              g_hasPendingTelemetry = false;
              http_state = HttpApiState_Done;
              break;
            }

            printf("\r\nHTTP JSON:\r\n%s\r\n", g_http_json);
            printf("\r\nHTTP REQUEST:\r\n%s\r\n", g_http_request);

            httpTxObj.pHttpRawInStr = g_http_request;
            http_response_received = false;
            http_state = HttpApiState_OpenConnection;
            break;
          }
        case HttpApiState_OpenConnection:
          {
            printf("\r\nOpening HTTP connection...\r\n");
            result = ST87EC_Lib_NBIOT_HttpOpen(HTTP_SERVER_IP, HTTP_SERVER_PORT, HTTP_SECURE_ID, HTTP_TIMEOUT_MS);

            if(result == RESULT_OK)
            {
              http_state = HttpApiState_WaitOpen;
            }
            else
            {
              printf("\r\nFailed to open HTTP connection: %d\r\n", result);
              http_state = HttpApiState_CloseConnection;  // Try to close the connection even if opening failed
            }
            break;
          }
        case HttpApiState_WaitOpen:
          {
            if(eclib_state.HttpConnectionStatus == HTTP_CONNECTED)
            {
              printf("\r\nHTTP connection opened successfully.\r\n");
              vTaskDelay(pdMS_TO_TICKS(300));
              http_state = HttpApiState_SendRequest;
            }
            break;
          }
        case HttpApiState_SendRequest:
          {
//            if(eclib_state.HttpConnectionStatus == HTTP_NOT_CONNECTED)
//            {
//              printf("HTTP connection not open\r\n");
//              http_state = HttpApiState_OpenConnection;
//              break;
//            }

            printf("\r\nSending HTTP POST request...\r\n\n");
            result = ST87EC_Lib_NBIOT_HttpTransfer(&httpTxObj);

            if(result == RESULT_OK)
            {
              http_state = HttpApiState_WaitResponse;
            }
            else
            {
              printf("\r\nFailed to send HTTP request: %d\r\n", result);
              http_state = HttpApiState_CloseConnection;  // Try to close the connection even if sending failed
            }
            break;
          }
        case HttpApiState_WaitResponse:
          {
            /* attende la ricezione della risposta HTTP tramite la callback HTTPReceiveCallback */
            if(http_response_received)
            {
              printf("\r\nHTTP response received.\r\n");
              http_state = HttpApiState_CloseConnection;
            }
            break;
          }
        case HttpApiState_CloseConnection:
          {
            printf("\r\nClosing HTTP connection...\r\n");
            result = ST87EC_Lib_NBIOT_HttpClose(HTTP_TIMEOUT_MS);

            if(result != RESULT_OK)
            {
              printf("\r\nFailed to close HTTP connection: %d\r\n", result);
            }

            if(g_httpFlow == HttpFlow_Telemetry)
            {
              // Reset the pending telemetry flag after closing the connection
              g_hasPendingTelemetry = false;
              http_state = HttpApiState_OpenPendingCommandsConnection;
            }
            else
            {
              /* NON serve resettare il flag g_hasPendingTelemetry qui perché il reset
               * avviene SOLO DOPO l'invio della telemetria, e non dopo l'invio dei comandi pendenti,
               * quindi è già stato fatto, dal momento che il flusso è:
               *
               * idle -> pop evento -> h_hasPendingTelemetry = true -> prepare telemetry ->
               * -> invio POST telemetria -> chiusura connessione -> h_hasPendingTelemetry = false (ECCO) ->
               * -> polling pending commands -> close connection (SIAMO QUA) -> arm periodic timer -> idle
               * */
              http_state = HttpApiState_ArmPeriodicTimer;
            }
            break;
          }
        case HttpApiState_OpenPendingCommandsConnection:
          {
            // preparo la variabile per la richiesta GET dei comandi pendenti
            memset(g_http_cmd_request, 0, sizeof(g_http_cmd_request));
            memset(g_lastHttpResponse, 0, sizeof(g_lastHttpResponse));

            if(!BuildPendingCommandsGetRequest(g_http_cmd_request, sizeof(g_http_cmd_request)))
            {
              printf("\r\nFailed to build pending commands GET request.\r\n");
              http_state = HttpApiState_ArmPeriodicTimer;
              break;
            }

            printf("\r\nPending commands HTTP REQUEST:\r\n%s\r\n", g_http_cmd_request);

            httpTxObj.pHttpRawInStr = g_http_cmd_request;

            /* di solito non serve resettare il flag http_response_received perché lo si fa già
             * nello stato HttpApiState_PrepareTelemetry, MA dato che ora vado a gestire lo stesso
             * oggetto httpTxObj per inviare la richiesta GET dei comandi pendenti, bisogna resettare il flag
             */
            http_response_received = false;
            g_httpFlow = HttpFlow_PendingCommands;

            printf("\r\nOpening HTTP connection for pending commands (GET request)...\r\n");
            result = ST87EC_Lib_NBIOT_HttpOpen(HTTP_SERVER_IP, HTTP_SERVER_PORT, HTTP_SECURE_ID, HTTP_TIMEOUT_MS);

            if(result == RESULT_OK)
            {
              http_state = HttpApiState_WaitPendingCommandsOpen;
            }
            else
            {
              printf("\r\nFailed to open HTTP connection for pending commands: %d\r\n", result);
              http_state = HttpApiState_ArmPeriodicTimer;
            }
            break;
          }
        case HttpApiState_WaitPendingCommandsOpen:
          {
            if(eclib_state.HttpConnectionStatus == HTTP_CONNECTED)
            {
              printf("\r\nHTTP connection for pending commands opened successfully.\r\n");
              vTaskDelay(pdMS_TO_TICKS(200));
              http_state = HttpApiState_SendPendingCommandsRequest;
            }
            break;
          }
        case HttpApiState_SendPendingCommandsRequest:
          {
            printf("\r\nSending HTTP GET request for pending commands...\r\n\n");
            result = ST87EC_Lib_NBIOT_HttpTransfer(&httpTxObj);

            if(result == RESULT_OK)
            {
              http_state = HttpApiState_WaitPendingCommandsResponse;
            }
            else
            {
              printf("\r\nFailed to send HTTP GET request for pending commands: %d\r\n", result);
              http_state = HttpApiState_CloseConnection;
            }
            break;
          }
        case HttpApiState_WaitPendingCommandsResponse:
          {
            /*
             * OSS.: è QUI che si capisce perchè era necessario andare a resettare http_response_received
             * nello stato HttpApiState_OpenPendingCommandsConnection
             *
             * Attende la ricezione della risposta HTTP tramite la callback HTTPReceiveCallback.*/
            if(http_response_received)
            {
              printf("\r\nHTTP response for pending commands received.\r\n");
              printf("\r\nPending commands raw payload:\r\n%s\r\n", g_lastHttpResponse);

              /* Parse the pending command from the HTTP response payload */
              if(ParsePendingCommand(g_lastHttpResponse, &g_pendingCommand))
                  {
                    printf("\r\nPending command parsed successfully!\r\n");
                    printf("Command ID   : %lu\r\n", (unsigned long)g_pendingCommand.id);
                    printf("Command TYPE : %s\r\n", g_pendingCommand.type);
                    printf("Command DATA : %s\r\n", g_pendingCommand.payload);
                  }
                  else
                  {
                    printf("\r\nNo valid pending command found.\r\n");
                  }

              http_state = HttpApiState_CloseConnection;
            }
            break;
          }
        case HttpApiState_ArmPeriodicTimer:
          {
            HAL_LPTIM_TimeOut_Stop_IT(&hlptim1);  // stop any previous timer

            /* --- OLD FIXED TIMER -APPLICATION_SLEEP_TIME_S --- */
//            printf("\r\nArming periodic timer for %ds...\r\n", (unsigned int) APPLICATION_SLEEP_TIME_S);
//            /* Start the LPTIM timer for the next FIXED sleep period */
//            HAL_LPTIM_TimeOut_Start_IT(&hlptim1, LPTIM_PERIOD);

            /* --- NEW USER-DEFINED TIMER - g_sampling_period_s --- */
            printf("\r\nArming periodic timer for %ds...\r\n", (unsigned int) g_sampling_period_s);
            /* Start the LPTIM timer for the next sleep period CHOSEN by the user through web dashboard */
            HAL_LPTIM_TimeOut_Start_IT(&hlptim1, GetLptimPeriodFromSeconds(g_sampling_period_s));

            /* Reset the pending command structure after processing it */
            memset(&g_pendingCommand, 0, sizeof(g_pendingCommand));

            /* switch back to telemetry flow for the next cycle */
            g_httpFlow = HttpFlow_Telemetry;
            http_state = HttpApiState_Idle;
            break;
          }
//        case HttpApiState_Done:
//          {
//            static bool printed = false;
//            if(!printed)
//            {
//              printf("\r\nHTTP TEST request sequence completed.\r\n");
//              printed = true;
//            }
//
//            vTaskSuspend(NULL);
//            break;
//          }
//        case HttpApiState_GoToSleep:
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
//            http_state = HttpApiState_Idle;
//            break;
//          }
        default:
          {
//            http_state = HttpApiState_Init;
            break;
          }
      }
    }
    LedBlinking(&eclib_state);
  }
}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//                                    ******************
//                                    * HTTP functions *
//                                    ******************

/**
 * @brief Callback function for receiving HTTP data.
 *
 * This function is called when HTTP data is received. It processes the received data
 * and updates the application state accordingly. If no data is received, it logs a message indicating that no data was received.
 *
 * @param receivedData Pointer to the received HTTP data. If NULL, no data was received.
 * */
static void HTTPReceiveCallback(char const *const receivedData)
{
  /* Clear the global response buffer before copying new data */
  memset(g_lastHttpResponse, 0, sizeof(g_lastHttpResponse));

  if(receivedData != NULL)
  {
    /* Copy the received data into the global response buffer.
     * OSS.: Servirebbe solo per gli stati Pending
     * */
    snprintf(g_lastHttpResponse, sizeof(g_lastHttpResponse), "%s", receivedData);

    /* Process the received data */
    printf(APP_LOG_PREFIX"Received HTTP data.");
    printf(APP_LOG_PREFIX"Payload: %s\n", receivedData);
  }
  else
  {
    printf(APP_LOG_PREFIX"#HTTPRECV: No data received.\r\n");

    /* Clear the global response buffer if no data was received */
//    g_lastHttpResponse[0] = '\0';
  }

  http_response_received = true;

  // per adesso la gestione della chiusura della connessione la faccio nello stato HttpApiState_WaitResponse
//  http_state = HttpApiState_CloseConnection;
}

/**
 * @brief Builds an ISO 8601 timestamp string from the current RTC time and date.
 *
 * This function retrieves the current time and date from the RTC and formats it into an ISO 8601 timestamp string.
 * The resulting string is stored in the provided output buffer.
 *
 * @param[out] out Pointer to the output buffer where the timestamp string will be stored.
 * @param[in] out_size Size of the output buffer in bytes.
 */
static void BuildIsoTimestamp(char *out, size_t out_size)
{
  RTC_TimeTypeDef time;
  RTC_DateTypeDef date;

  HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

  snprintf(out, out_size, "20%02u-%02u-%02uT%02u:%02u:%02uZ", (unsigned int) date.Year, (unsigned int) date.Month, (unsigned int) date.Date,
           (unsigned int) time.Hours, (unsigned int) time.Minutes, (unsigned int) time.Seconds);
}

/**
 * @brief Returns a string representation of the orientation based on the sensor data.
 *
 *    FUNZIONE MOMENTANEA PER TEST, DA SOSTITUIRE CON FUNZIONE CHE LEGGE IL VALORE DALLA STRUTTURA SENSORS_DATA
 *
 * This function checks the event type and MLC output in the provided sensor data and returns a corresponding string that describes the orientation.
 *
 * @param data Pointer to the SensorsData structure containing the event type and MLC output.
 * @return A string representing the orientation based on the sensor data. If the event type is not eEventMLC1 or the MLC output is unrecognized, it returns "UNKNOWN".
 */
static const char* GetOrientationString(const SensorsData *data)
{
  if(data->event_type == eEventMLC1)
  {
    switch(data->mlc_output)
    {
      case 0x00:
        return "stationary_upright";
      case 0x04:
        return "stationary_not_upright";
      case 0x08:
        return "in_motion";
      case 0x0C:
        return "shaken";
      default:
        return "unknown";
    }
  }

  return "shaken"; // Default orientation if event type is not eEventMLC1
}

/**
 * @brief Builds a JSON string containing telemetry data from the provided sensor data.
 *
 * This function formats the sensor data into a JSON string, including device ID, timestamp, temperature, humidity, pressure, battery level, and orientation.
 *
 * @param data Pointer to the SensorsData structure containing the sensor readings.
 * @param json Pointer to the output buffer where the JSON string will be stored.
 * @param json_size Size of the output buffer in bytes.
 * @return true if the JSON string was successfully built and fits within the provided buffer; false if the buffer is too small.
 */
static bool BuildTelemetryJson(const SensorsData *data, char *json, size_t json_size)
{
  char timestamp[32];
  const char *orientation = GetOrientationString(data);

  BuildIsoTimestamp(timestamp, sizeof(timestamp));

  uint32_t battery = 100U;   // placeholder iniziale

  int written = snprintf(json, json_size, "{"
                         "\"deviceId\": \"%s\","
                         "\"timestamp\": \"%s\","
                         "\"temperature\": %.2f,"
                         "\"humidity\": %.2f,"
                         "\"pressure\": %.2f,"
                         "\"battery\": %u,"
                         "\"orientation\": \"%s\""
                         "}",
                         "DEV001", timestamp, data->sensor_hum_and_temp.temp, data->sensor_hum_and_temp.hum, data->sensor_barometer.pres,
                         (unsigned int) battery, orientation);

  if((written < 0) || ((size_t) written >= json_size))
  {
    printf("BuildTelemetryJson failed: buffer too small\r\n");
    return false;
  }

  return true;
}

/**
 * @brief Builds a fixed JSON string for telemetry data.
 *
 *    ***** FUNCTION TEST WITH FIXED VALUES, TO BE REPLACED WITH BuildTelemetryJson() *****
 *
 * This function creates a JSON string with predefined telemetry values, including device ID, timestamp, temperature, humidity, pressure, battery level, and orientation.
 *
 * @param json Pointer to the output buffer where the JSON string will be stored.
 * @param json_size Size of the output buffer in bytes.
 * @return true if the JSON string was successfully built and fits within the provided buffer; false if the buffer is too small.
 */
static bool BuildFixedTelemetryJson(char *json, size_t json_size)
{
  int written = snprintf(json, json_size, "{"
                         "\"deviceId\": \"DEV001\","
                         "\"timestamp\": \"2026-07-10T12:40:30Z\","
                         "\"temperature\" :30.60,"
                         "\"humidity\" :37.85,"
                         "\"pressure\" :992.06,"
                         "\"battery\" :100,"
                         "\"orientation\" :\"shaken\""
                         "}");

  return (written >= 0) && ((size_t) written < json_size);
}

/**
 * @brief Builds an HTTP POST request string with the provided JSON payload.
 *
 * This function formats the HTTP POST request, including headers and the JSON payload, into the provided request buffer.
 *
 * @param json Pointer to the JSON payload string to be included in the POST request.
 * @param request Pointer to the output buffer where the HTTP POST request string will be stored.
 * @param request_size Size of the output buffer in bytes.
 * @return true if the HTTP POST request string was successfully built and fits within the provided buffer; false if the buffer is too small.
 */
static bool BuildHttpPostRequest(const char *json, char *request, size_t request_size)
{
  uint32_t content_length = (uint32_t) strlen(json);

//  int written = snprintf(request, request_size, "POST %s HTTP/1.1\r\n"
//                         "Host: %s:%u\r\n"
//                         "User-Agent: ST87EC/1.0\r\n"
//                         "Accept: */*\r\n"
//                         "Content-Type: application/json\r\n"
//                         "Content-Length: %u\r\n"
//                         "Connection: close\r\n"
//                         "\r\n"
//                         "%s",
//                         TELEMETRY_PATH,
//                         HTTP_SERVER_IP,
//                         (unsigned int) HTTP_SERVER_PORT, (unsigned int) content_length, json);

  int written = snprintf(request, request_size, "POST %s HTTP/1.1\r\n"
//                         "Host: %s\r\n"
//                         "User-Agent: ST87EC/1.0\r\n"
//                         "Accept: */*\r\n"
      "Content-Type: application/json\r\n"
//                         "Content-Length: %u\r\n"
//                         "Connection: close\r\n"
      "\r\n"
      "%s",
      TELEMETRY_PATH,
//                         HTTP_SERVER_IP,
//                         (unsigned int) content_length,
      json);

  if((written < 0) || ((size_t) written >= request_size))
  {
    printf("BuildHttpPostRequest failed: buffer too small\r\n");
    return false;
  }

  return true;
}

/**
 * @brief Builds an HTTP GET request string for retrieving pending commands.
 *
 * This function formats the HTTP GET request to retrieve pending commands for a specific device.
 * The request is stored in the provided request buffer.
 *
 * @param request Pointer to the output buffer where the HTTP GET request string will be stored.
 * @param request_size Size of the output buffer in bytes.
 * @return true if the HTTP GET request string was successfully built and fits within the provided buffer; false if the buffer is too small.
 */
static bool BuildPendingCommandsGetRequest(char *request, size_t request_size)
{
  const char *deviceId = "DEV001";  // placeholder for the device ID, to be replaced with the actual device ID if needed

  /* OSS.: il modem ST87 richiede un body non vuoto anche per le richieste GET,
   * altrimenti la richiesta fallisce.
   * Per questo motivo viene inviato un body minimo "{}" come workaround.
   */
  int written = snprintf(request, request_size,
                         "GET /api/v1/devices/%s/commands/pending HTTP/1.1\r\n"
                         "Content-Type: application/json\r\n"
                         "\r\n{}"
                         "\r\n",
                         deviceId);

  if((written < 0) || ((size_t) written >= request_size))
  {
    printf("BuildPendingCommandsGetRequest failed: buffer too small\r\n");
    return false;
  }

  return true;
}

/**
 * @brief Converts a duration in seconds to the corresponding LPTIM period value.
 *
 * This function calculates the LPTIM period value based on the provided duration in seconds.
 * The calculated period is limited to a maximum value of 65535 to ensure it fits within the LPTIM register.
 *
 * @param seconds The duration in seconds for which to calculate the LPTIM period.
 * @return The calculated LPTIM period value, limited to a maximum of 65535.
 */
static uint32_t GetLptimPeriodFromSeconds(uint32_t seconds)
{
  uint32_t period = (seconds * 1000000U) / LPTIM_TICK_TIME_US;

  if(period > 65535U)
  {
    period = 65535U;
  }

  return period;
}

/**
 * @brief Skips whitespace characters in a string.
 *
 * This function takes a pointer to a string and advances the pointer past any leading
 * whitespace characters (spaces, tabs, carriage returns, or newlines). It returns a pointer
 * to the first non-whitespace character in the string, or NULL if the input pointer is NULL.
 *
 * @param p Pointer to the input string.
 * @return Pointer to the first non-whitespace character in the string, or NULL if the input is NULL.
 */
static const char* SkipSpaces(const char *p)
{
  while((p != NULL) && ((*p == ' ') || (*p == '\t') || (*p == '\r') || (*p == '\n')))
  {
    p++;
  }
  return p;
}

/**
 * @brief Extracts a uint32_t value from a JSON string based on a specified key.
 *
 * This function searches for a specified key in a JSON string and extracts the corresponding
 * uint32_t value. It assumes that the value is represented as an unsigned integer in the JSON.
 *
 * @param json Pointer to the input JSON string.
 * @param key Pointer to the key whose value is to be extracted.
 * @param value Pointer to the variable where the extracted uint32_t value will be stored.
 * @return true if the key was found and the value was successfully extracted; false otherwise.
 */
static bool ExtractJsonUint32(const char *json, const char *key, uint32_t *value)
{
  char pattern[32];
  snprintf(pattern, sizeof(pattern), "\"%s\":", key);

  const char *p = strstr(json, pattern);
  if(p == NULL)
  {
    return false;
  }

  p += strlen(pattern);
  p = SkipSpaces(p);

  if((p == NULL) || (*p < '0') || (*p > '9'))
  {
    return false;
  }

  *value = (uint32_t)strtoul(p, NULL, 10);
  return true;
}

/**
 * @brief Extracts a string value from a JSON string based on a specified key.
 *
 * This function searches for a specified key in a JSON string and extracts the corresponding
 * string value. It assumes that the value is enclosed in double quotes in the JSON.
 *
 * @param json Pointer to the input JSON string.
 * @param key Pointer to the key whose value is to be extracted.
 * @param out Pointer to the output buffer where the extracted string will be stored.
 * @param out_size Size of the output buffer in bytes.
 * @return true if the key was found and the string value was successfully extracted; false otherwise.
 */
static bool ExtractJsonString(const char *json, const char *key, char *out, size_t out_size)
{
  char pattern[32];
  snprintf(pattern, sizeof(pattern), "\"%s\":", key);

  const char *p = strstr(json, pattern);
  if(p == NULL)
  {
    return false;
  }

  p += strlen(pattern);
  p = SkipSpaces(p);

  if((p == NULL) || (*p != '\"'))
  {
    return false;
  }

  p++; // skip opening quote

  const char *end = strchr(p, '\"');
  if(end == NULL)
  {
    return false;
  }

  size_t len = (size_t)(end - p);
  if(len >= out_size)
  {
    len = out_size - 1U;
  }

  memcpy(out, p, len);
  out[len] = '\0';

  return true;
}

/**
 * @brief Parses a pending command from an HTTP payload.
 *
 * This function extracts the command ID, type, and payload from the provided HTTP payload,
 * which is expected to be in JSON format. It populates the provided PendingCommand_t structure
 * with the extracted values and sets the valid flag to true if the parsing is successful.
 *
 * @param httpPayload Pointer to the HTTP payload string containing the command data.
 * @param cmd Pointer to the PendingCommand_t structure where the parsed command will be stored.
 * @return true if the command was successfully parsed; false otherwise.
 */
static bool ParsePendingCommand(const char *httpPayload, PendingCommand_t *cmd)
{
  if((httpPayload == NULL) || (cmd == NULL))
  {
    return false;
  }

  memset(cmd, 0, sizeof(PendingCommand_t));
  cmd->valid = false;

  /* Cerca l'inizio del body JSON: la risposta contiene anche gli header HTTP */
  const char *json = strstr(httpPayload, "\r\n\r\n");
  if(json != NULL)
  {
    json += 4; // skip "\r\n\r\n"
  }
  else
  {
    json = httpPayload;
  }

  json = SkipSpaces(json);

  if(json == NULL)
  {
    return false;
  }

  /* Nessun comando pendente */
  if((strncmp(json, "[]", 2) == 0))
  {
    return false;
  }

  /* Per ora assumiamo al massimo il primo elemento dell'array */
  if(strchr(json, '{') == NULL)
  {
    return false;
  }

  bool ok_id = ExtractJsonUint32(json, "id", &cmd->id);
  bool ok_type = ExtractJsonString(json, "type", cmd->type, sizeof(cmd->type));

  /* payload può anche essere stringa vuota */
  bool ok_payload = ExtractJsonString(json, "payload", cmd->payload, sizeof(cmd->payload));
  if(!ok_payload)
  {
    cmd->payload[0] = '\0';
  }

  if(ok_id && ok_type)
  {
    cmd->valid = true;
    return true;
  }

  return false;
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

  // ** Check if the parsed time is valid **************************************
  bool valid_time = true;

  if((result.tm_year < 124) ||   // 2024 = 124 in tm_year
      (result.tm_mon < 0) || (result.tm_mon > 11) || (result.tm_mday < 1) || (result.tm_mday > 31))
  {
    valid_time = false;
  }

  if(!valid_time)
  {
    printf("Invalid modem time received, waiting before retry...\r\n");
    vTaskDelay(pdMS_TO_TICKS(2000));
    http_state = HttpApiState_Init;
    return;
  }
  // *************************************************************************

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

// ************ COMMENTO PER FARE TEST DEL TASK HTTP, SENZA I SENSORI ************
//  uint32_t now = GetCurrentTimeSeconds();
//  last_batch_sample_time_s = now;
////  last_udp_send_time_s = now;
//
  /* Wakeup the sensors data task */
  eEventType_t event = eEventFirst;
  configASSERT(xDataQueue != NULL);
  xQueueSend(xDataQueue, &event, portMAX_DELAY);
//*******************************************************************************

//    http_state = HttpApiState_PrepareFixedRequest;  // In test mode, go to PrepareFixedRequest state

//************ COMMENTO PER FARE TEST DEL TASK HTTP, SENZA I SENSORI ************
  http_state = HttpApiState_Idle;  // In normal mode, go to Idle state after setting the time
//*******************************************************************************
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

