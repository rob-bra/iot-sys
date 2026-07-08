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
#include "certificates.h"

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

#include "pgeez1_m95p32.h"
#include "m95p32.h"
#include "NBIOTV1_bc.h"

#include "amazon_root_CA3.h"

#define GPS_ENABLED                     0

#define APPLICATION_SLEEP_TIME_S        30

/* Timer tick period according to hardware configuration */
#define LPTIM_TICK_TIME_MS              4

#define SECONDS_TO_LPTIM_PERIOD(x)      ((x)*1000/LPTIM_TICK_TIME_MS)
#define DEFAULT_LPTIM_PERIOD            SECONDS_TO_LPTIM_PERIOD(APPLICATION_SLEEP_TIME_S)

#if (DEFAULT_LPTIM_PERIOD > 65535)
#warning "DEFAULT_LPTIM_PERIOD too big. Try reducing APPLICATION_SLEEP_TIME_S."
#endif

#define HTTP_REQUEST_BUF_SIZE           (1024U)

/* ST Dashboard Definitions
 * Browser URL: https://dsh-assetracking.st.com/
 **/
#define HTTP_HOST                       "jim3rgi6d3.click"
#define HTTP_PORT                       (443U)
#define HTTP_EXPECTED_CERTIFICATE       "0,1,Amazon Root CA 3"
#define HTTP_POST_HEADER                "POST /telemetry HTTP/1.1\r\n"
#define HTTP_AUTH_HEADER                "Authorization: "DASHBOARD_API_KEY"\r\n"
#define HTTP_CONTENT_TYPE_HEADER        "Content-Type: application/json\r\n"

/* Device ID and API Key
 * You should update these values with your own from the ST Dashboard
 **/
#define DASHBOARD_DEVICE_ID             "PierUgo"
#define DASHBOARD_API_KEY               "642981e2-f32e-4558-aefe-c2f89889bda9.45c07a9b-cdec-45d0-91c7-c608c7f1abde"

#define HTTP_REQUEST_TIMEOUT_DEFAULT    (20000U) // ms

/*
 * Enable External VREG for LNA on GPS Antenna
 * AT#REGVEXT=1,0
 **/
#define AT_CMD_SET_REGVEXT              "AT#REGVEXT=1,0"


/* LED Timing when not connected */
#define LED_TIME_ON_IDLE       1000U
#define LED_TIME_OFF_IDLE      1000U

/* LED Timing when connected */
#define LED_TIME_ON_ATTACHED   100U
#define LED_TIME_OFF_ATTACHED  900U

/* LED Timing when executing a sequence */
#define LED_TIME_ON_SEQUENCE   100U
#define LED_TIME_OFF_SEQUENCE  100U

typedef enum HTTP_API_State {
  HttpApiState_FirstAttach = 0,
  HttpApiState_Init,
  HttpApiState_Idle,
  HttpApiState_Open,
  HttpApiState_CheckTls,
  HttpApiState_Send,
  HttpApiState_Close,
  HttpApiState_TransferComplete,
  HttpApiState_PrepareForSleep,
  HttpApiState_VregExtSet,
  HttpApiState_GPSFix,
  HttpApiState_Wait
} HTTP_API_State;

typedef enum
{
  TLS_PROV_IMPORT = 0,
  TLS_PROV_LIST,
  TLS_PROV_LIST_VERIFY,
  TLS_PROV_DELETE
} provisioning_status_t;

typedef struct {
  float gps_lat;
  float gps_lon;
  float gps_ele;
} GpsData_t;

#if GPS_ENABLED == 1
volatile HTTP_API_State http_state = HttpApiState_VregExtSet;
volatile HTTP_API_State http_state_after_wait = HttpApiState_VregExtSet;
#else
volatile HTTP_API_State http_state = HttpApiState_FirstAttach;
volatile HTTP_API_State http_state_after_wait = HttpApiState_Init;

#endif

provisioning_status_t prov_status = TLS_PROV_LIST;
typedef enum {
  TLS_LIST_WAITING = 0,
  TLS_LIST_NONE,
  TLS_LIST_FOUND_AMAZON_ROOT_CA3,
  TLS_LIST_FOUND_OTHER
} eTlsListResult_t;

eTlsListResult_t tls_certificate_found = TLS_LIST_WAITING;

extern volatile Sensors_Init_State sensors_init_state;

static TaskHandle_t xHTTPTaskHandle = NULL;
#define HTTP_TASK_STACK_SIZE        (configMINIMAL_STACK_SIZE*8)

static TaskHandle_t xSensorsDataTaskHandle = NULL;
#define SENSOR_TASK_STACK_SIZE      (configMINIMAL_STACK_SIZE*2)

MessageBufferHandle_t xSensorDataMBHandle = NULL;
QueueHandle_t xDataQueue = NULL;

volatile GpsData_t gps_data = {
  .gps_lat = 0.0f,
  .gps_lon = 0.0f,
  .gps_ele = -1000.0f  /* this value is used to invalid the data */
};

uint32_t non_blocking_timer_timeout;
bool StateMachineDelay_Check(void);
void StateMachineDelay_Start(uint32_t ms, HTTP_API_State next_state);

static void HttpTask(void *pvParameters);
//static void LedTask(void *pvParameters);
static void LedBlinking(ST87EC_Lib_Status_t *eclib_state);

static void DefaultST87ECLibHttpTransferReadCallback(char const * const pString);
static void MyErrorCallback(ST87EC_Lib_SequenceValue_t FailingSequence, int32_t Error);
static void GNSSGetPosCallback(char const * const pString);

static void ST87GetTimeCallback(char const * const pString);

static uint32_t GetUnixTimestampFromRTC(void);

int BuildHttpBody(char *buffer, size_t buffer_size,
                    unsigned long timestamp,
                    float temperature, float humidity, float pressure,
                    float acc_x, float acc_y, float acc_z,
                    float gps_lat, float gps_lon, float gps_ele);

static void HandleHttpApiState_CheckTls(provisioning_status_t *prov_status, eTlsListResult_t *tls_certificate_found);


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
  BaseType_t xReturned;

  /* enable power supply for the sensors */
  HAL_GPIO_WritePin(STM32_VSENSOR_GPIO_Port, STM32_VSENSOR_Pin, GPIO_PIN_SET);

  HAL_Delay(1000);

  /* this task handles the HTTP communication */
  xReturned = xTaskCreate( HttpTask,
                           "http_task",
                           HTTP_TASK_STACK_SIZE,
                           NULL,
                           4,
                           &xHTTPTaskHandle );

  configASSERT( xReturned == pdPASS );

  xReturned = xTaskCreate( SensorsDataTask,
                           "sensors_task",
                           SENSOR_TASK_STACK_SIZE,
                           NULL,
                           3,
                           &xSensorsDataTaskHandle );

  configASSERT( xReturned == pdPASS );

  xDataQueue = xQueueCreate(2, sizeof(eEventType_t));
  configASSERT(xDataQueue != NULL);

  xSensorDataMBHandle = xMessageBufferCreate(sizeof(SensorsData)*2);
  configASSERT( xSensorDataMBHandle != NULL );

}

/* Callback to list Certificates */
static void TlsProvListCallback(int32_t const DataLen, char const *const pString)
{
  if(DataLen == 0)
  {
    tls_certificate_found = TLS_LIST_NONE;
    printf("No certificate present in the slot.");
  }
  else
  {

    /* Check if pString begins with EXPECTED_CERT */
    if(strncmp(pString, HTTP_EXPECTED_CERTIFICATE, strlen(HTTP_EXPECTED_CERTIFICATE)) == 0)
    {
      tls_certificate_found = TLS_LIST_FOUND_AMAZON_ROOT_CA3;
      printf("Expected certificate found in the slot.\r\n");
    }
    else
    {
      tls_certificate_found = TLS_LIST_FOUND_OTHER;
      printf("Unexpected certificate found in the slot.\r\n");
    }
    printf("\r\nCertificate:\t%s", pString);
  }
}


/**
 * @brief Callback function for handling HTTP transfer read events.
 *
 * This function is called when an HTTP transfer read event occurs. It processes
 * the received string data.
 *
 * @param pString Pointer to the received string data.
 */
static void DefaultST87ECLibHttpTransferReadCallback(char const * const pString)
{
  if (pString != NULL)
  {
    printf("\r\n#HTTP response:\r\n");
    printf("%s", (const char *)pString);
    printf("\r\n");
  }
  else
  {
    printf("\r\n#HTTP:\r\n");
  }
  http_state = HttpApiState_TransferComplete;
}

/**
 * @brief Callback function for handling GNSS position retrieval.
 *
 * This function is called when GNSS position data is received. It extracts
 * latitude, longitude, and elevation from the received string and updates
 * the global GPS data structure.
 *
 * @param pString Pointer to the received GNSS position string.
 */
static void GNSSGetPosCallback(char const * const pString)
{
  if (pString != NULL)
  {
    /* extract latitude, longitude, elevation from the string */
    /* #GNSSFIX: 2291,461282763,48.15370,-01.56719,130.2,33.6,00.0,00.0,52.2,2.4,9.5,7.7,5,11,2.4,30,-0.1,20,1.4,09,1.7,06,-0.3 */
    float lat = 0.0f, lon = 0.0f, ele = 0.0f;
    int parsed = sscanf(pString, "#GNSSFIX: %*u,%*u,%f,%f,%f", &lat, &lon, &ele);
    if (parsed == 3)
    {
      gps_data.gps_lat = lat;
      gps_data.gps_lon = lon;
      gps_data.gps_ele = ele;
      printf("GNSS Position: lat=%.6f lon=%.6f ele=%.2f\r\n", lat, lon, ele);
    }
    else
    {
      if(pString[0] == '5')
      {
        printf("No FIX yet, received string: %s\r\n", pString);
      }
      else
      {
        printf("GNSS Position parsing error, received string: %s\r\n", pString);
      }
    }
  }
  else
  {

    printf("Timeout occured. It was not possible to get a GNSS fix\r\n");
    printf("Be sure there is a clear sky view and try again\r\n\r\n");

    http_state = HttpApiState_Idle;

    /* Start LPTIM to restart the application sequence with a little delay
     * This is needed to avoid conflicts when module is going to sleep
     **/
    HAL_LPTIM_TimeOut_Start_IT(&hlptim1, SECONDS_TO_LPTIM_PERIOD(3));
  }
}

/**
 * @brief Callback function for handling time retrieval.
 *
 * This function is called when time data is received. It processes
 * the received string data and updates the system time accordingly.
 *
 * @param pString Pointer to the received time string.
 */
static void MyErrorCallback(ST87EC_Lib_SequenceValue_t FailingSequence, int32_t Error)
{
  /* Log the error information */
  printf("Error Callback Invoked!\n");
  printf("Failing Sequence ID: %d\n", FailingSequence);
  printf("Error Code: %" PRId32 "\n", Error);

  /* Implement additional error handling logic here
   * For example, you might want to reset certain components or notify the user */
  if(FailingSequence != 0)
  {
    printf("Handling error for sequence ID: %d\n", FailingSequence);
  }
  else
  {
    printf("No ongoing sequence. General error handling.\n");
  }
}

/**
 * @brief Prepares the system for entering sleep mode.
 *
 * This function is called before the system enters sleep mode. It performs
 * any necessary preparations to ensure the system can enter and wake up
 * from sleep mode correctly.
 *
 * @param ulExpectedIdleTime The expected duration of the idle time in milliseconds.
 */
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
  /* Enter sleep only if sensors have already been initialized */
  if(sensors_init_state == SensorsInitialized)
  {
    __WFI();
  }
}

/**
 * @brief Perform post-sleep processing.
 *
 * This function is called after the system wakes up from sleep mode.
 *
 * @param ulExpectedIdleTime The expected idle time in milliseconds.
 */
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{

}

/**
  * @brief  Compare match callback in non blocking mode
  * @param  hlptim: LPTIM handle
  * @retval None
  */
void HAL_LPTIM_CompareMatchCallback(LPTIM_HandleTypeDef *hlptim)
{
  /* Timeout was reached, resume the task */
  if(hlptim == &hlptim1)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    eEventType_t event = eEventTimer;
    configASSERT( xDataQueue != NULL );  
    xQueueSendFromISR( xDataQueue, &event, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }
}


/**
 * @brief  EXTI line rising edge detection callback.
 * @param  GPIO_Pin: Specifies the pins connected EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == LIS2DUXS12_INT1_Pin)
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    HAL_LPTIM_TimeOut_Stop_IT(&hlptim1);

    eEventType_t event = eEventTilt;
    configASSERT( xDataQueue != NULL );
    xQueueSendFromISR( xDataQueue, &event, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
  }
  else if(GPIO_Pin == BUTTON_PWR_Pin)
  {
    BSP_BC_Sw_CmdSend(SHIPPING_MODE_ON);
  }
}


/**
 * @brief Starts a non-blocking delay in the state machine.
 *
 * This function sets a software timer for the specified number of milliseconds and
 * updates the state machine to wait in the HttpApiState_Wait state.
 * After the delay expires, the state machine will transition to the specified next state.
 *
 * @param ms         Duration of the delay in milliseconds.
 * @param next_state State to transition to after the delay.
 */
void StateMachineDelay_Start(uint32_t ms, HTTP_API_State next_state)
{
  non_blocking_timer_timeout = xTaskGetTickCount() + pdMS_TO_TICKS(ms);
  http_state_after_wait = next_state;
  http_state = HttpApiState_Wait;
}

/**
 * @brief Checks if the non-blocking delay has expired.
 *
 * This function should be called periodically to determine if the delay
 * started by StateMachineDelay_Start() has elapsed. If the delay has expired,
 * the state machine transitions to the next state.
 *
 * @retval true  Delay has expired and state has transitioned.
 * @retval false Delay is still active.
 */
bool StateMachineDelay_Check(void)
{
  if (xTaskGetTickCount() >= non_blocking_timer_timeout)
  {
    http_state = http_state_after_wait;
    return true;
  }
  else
  {
    return false;
  }
}

/**
 * @brief Task responsible for handling HTTP operations.
 *
 * This function is executed as a FreeRTOS task and is responsible for
 * managing HTTP communications. The specific operations and details
 * of the HTTP handling are defined within the function.
 *
 * After initialization, the task enters an infinite loop where it
 * manages the HTTP state machine, handles network registration,
 * and processes HTTP requests and responses.
 * This task is woken up by events such as timer expirations or sensor data availability.
 *
 * @param pvParameters Pointer to the parameters passed to the task.
 *                     This can be used to pass any required data to the task.
 */
static void HttpTask(void *pvParameters)
{
  UNUSED(pvParameters);

  uint32_t timestamp = 0;

  char http_request_buffer[HTTP_REQUEST_BUF_SIZE];

  SensorsData data;

  ST87EC_Lib_Status_t eclib_state;
  ST87EC_Lib_Result_t result;
  ST87EC_Lib_HttpTransferObject_t http_object = { 0 };
  ST87EC_Lib_AtCmdObject_t at_cmd_object = { 0 };

  printf("\r\n\r\n--------------- NB-IoT Asset Tracking application init ---------------\r\n");
  printf("Waiting for network registration...\r\n");

  /* EC lib initialization */
  result = ST87EC_Lib_Init(MyErrorCallback);
  configASSERT(result == RESULT_OK);

  /* Initialize the http object */
  http_object.pHttpRawInStr = http_request_buffer;
  http_object.pHttpRxCallbackFunc = DefaultST87ECLibHttpTransferReadCallback;
  http_object.KeepAlive = 1;
  http_object.Timeout = HTTP_REQUEST_TIMEOUT_DEFAULT;

  /* Initialize the AT command object to enable external VREG for LNA  */
  at_cmd_object.pCommand = AT_CMD_SET_REGVEXT;
  at_cmd_object.pRspTag = NULL;
  at_cmd_object.RawDataLen = 0;
  at_cmd_object.Timeout = 5000;
  at_cmd_object.pAtRespCallbackFunc = NULL;

  for(;;)
  {
    /* Run EC Lib main sequence */
    result = ST87EC_Lib_Scheduler();

    /* Handle possible errors during sequence execution
     * If an error occurs, reset the modem and restart the HTTP state machine
     * Additional error handling can be added in the MyErrorCallback function
     **/
    if(result == RESULT_KO)
    {
      printf("\r\nThere was an error in sequence execution...\r\n");
      printf("Modem reset underway...\r\n");
      ST87EC_Lib_Reset();
      http_state = HttpApiState_Open;
    }

    ST87EC_Lib_GetState(&eclib_state);

    /*
     * Enter in the State Machine only if the device is attached to the network and no
     * sequence is ongoing
     **/
    if((eclib_state.RegistrationStatus == REGISTERED) && (eclib_state.OnGoingSequence == SEQUENCE_NONE))
    {
      switch(http_state)
      {
        case HttpApiState_FirstAttach:
          {
            printf("Registration complete. Attached to NB-IoT network!\r\n");

            /* Wait 2 seconds before starting the init sequence */
            StateMachineDelay_Start(2000, HttpApiState_Init);
          }
          break;
        case HttpApiState_Wait:
          {
            /* Remain in this state until the non-blocking delay has elapsed */
            StateMachineDelay_Check();
          }
          break;

        case HttpApiState_VregExtSet:
          {
            result = ST87EC_Lib_UnitaryATcommandSend(&at_cmd_object);
            if(result == RESULT_OK)
            {
              printf("\r\nSetting LNA external VREG for GNSS antenna...\r\n");
              http_state = HttpApiState_Init;
            }
            else
            {
              printf("Error on AT command send: %d\r\n", result);
            }
          }
          break;

        case HttpApiState_Init:
          {
            result = ST87EC_Lib_GetTime(ST87GetTimeCallback, HTTP_REQUEST_TIMEOUT_DEFAULT);
            if(result == RESULT_OK)
            {
              /* No need to change state here, the callback will set the next state */
            }
            else
            {
              printf("Error on GetTime: %d\r\n", result);
            }
          }
          break;

        case HttpApiState_CheckTls:
          HandleHttpApiState_CheckTls(&prov_status, &tls_certificate_found);
          break;

        case HttpApiState_Open:
          {
            result = ST87EC_Lib_NBIOT_HttpOpen(HTTP_HOST, HTTP_PORT, 0, HTTP_REQUEST_TIMEOUT_DEFAULT);
            if(result == RESULT_OK)
            {
              /* Set the HTTP object with the current request buffer */
              printf("\r\nInitiating HTTP open sequence...\r\n");
              http_state = HttpApiState_PrepareForSleep;
            }
            else
            {
              printf("Error on HTTP open: %d\r\n", result);
            }
          }
          break;

        case HttpApiState_Send:
          {
            memset(http_request_buffer, 0, HTTP_REQUEST_BUF_SIZE);
            memcpy(http_request_buffer, HTTP_POST_HEADER, strlen(HTTP_POST_HEADER));
            strcat(http_request_buffer, HTTP_AUTH_HEADER);
            strcat(http_request_buffer, HTTP_CONTENT_TYPE_HEADER);
            strcat(http_request_buffer, "\r\n");

            int current_http_request_buffer_len = strlen(http_request_buffer);

            timestamp = GetUnixTimestampFromRTC();

            BuildHttpBody(&http_request_buffer[current_http_request_buffer_len],
                                      HTTP_REQUEST_BUF_SIZE - current_http_request_buffer_len,
                                      timestamp,
                                      data.sensor_hum_and_temp.temp,
                                      data.sensor_hum_and_temp.hum,
                                      data.sensor_barometer.pres,
                                      data.sensor_accelerometer.x,
                                      data.sensor_accelerometer.y,
                                      data.sensor_accelerometer.z,
                                      gps_data.gps_lat,
                                      gps_data.gps_lon,
                                      gps_data.gps_ele);

            printf("\r\nHTTP request:\r\n");
            printf("%s\n", http_request_buffer);

            if (ST87EC_Lib_NBIOT_HttpTransfer(&http_object) == RESULT_OK)
            {
              printf("\r\nInitiating HTTP send request sequence...\r\n");
            }
            else
            {
              printf("\r\nError initiating HTTP send request sequence...\r\n");
            }

            /* No need to change state here, the callback will set the next state */
          }
          break;

        case HttpApiState_Close:
          {
            /* for the moment unused */
          }
          break;

        case HttpApiState_PrepareForSleep:
          {
            printf("\r\nApplication going to sleep for %ds...\r\n", APPLICATION_SLEEP_TIME_S);
            /* Enable LPTIM1 running even when STOP2 mode is active */
            /* ToDo: move this out of main loop?? */
            __HAL_RCC_LPTIM1_CLKAM_ENABLE();
            __HAL_RCC_RTCAPB_CLKAM_ENABLE();

#if GPS_ENABLED == 0
            /* Start LPTIM1 to wakeup the system after the selected period. */
            HAL_LPTIM_TimeOut_Start_IT(&hlptim1, DEFAULT_LPTIM_PERIOD);
            http_state = HttpApiState_Idle;
#else
            /* If GPS is enabled, go to GPSFix state */
            http_state = HttpApiState_GPSFix;
#endif

          }
          break;

        case HttpApiState_Idle:
          {
            /* Wait for new messages from sensor task.
             * Timeout in order to execute the scheduler every 100ms (except when in STOP2)*/
            uint32_t size = xMessageBufferReceive(xSensorDataMBHandle, (void* )&data, sizeof(SensorsData), pdMS_TO_TICKS(100));
            if(size != 0)
            {
              http_state = HttpApiState_Send;
            }
          }
          break;

        case HttpApiState_TransferComplete:
          {
            printf("\r\nHTTP transfer complete...\r\n");

            http_state = HttpApiState_PrepareForSleep;
          }
          break;

        case HttpApiState_GPSFix:
          {
            if (eclib_state.SleepWakeupstatus == STATUS_SLEEP)
            {
                result = ST87EC_Lib_GNSS_GetFix(1, GNSSGetPosCallback, 10000);
                if(result == RESULT_OK)
                {
                  http_state = HttpApiState_Idle;
                }
                else
                {
                  printf("Error on GNSS GetFix: %d\r\n", result);
                }
            }
          }
          break;

        default:
          break;
      }
    }
    LedBlinking(&eclib_state);
  }
}


/**
 * @brief Handles the logic for the HttpApiState_CheckTls state.
 *
 * This function encapsulates the state machine logic previously found in the
 * HttpApiState_CheckTls case, improving readability and maintainability.
 *
 * @param prov_status Pointer to the provisioning status variable.
 * @param tls_certificate_found Pointer to the TLS certificate found result variable.
 */
static void HandleHttpApiState_CheckTls(provisioning_status_t *prov_status, eTlsListResult_t *tls_certificate_found)
{
  ST87EC_Lib_Result_t result;

  switch(*prov_status)
  {
    case TLS_PROV_IMPORT:
      {
        ST87EC_Lib_TlsImportElemObject_t  httpsTlsImportObj = { 0 };
        /* --- TLS Import Object initialization with Amazon Root Certificate */
        httpsTlsImportObj.SecureId = 0;
        httpsTlsImportObj.ElementType = TLS_PROV_ELEM_ROOT_CERT;
        httpsTlsImportObj.EccType = TLS_PROV_ECC_SECP_R1;
        httpsTlsImportObj.InputLength = (uint16_t) amazon_cert_der_len;
        httpsTlsImportObj.pInputData = (char*) amazon_cert_der;
        httpsTlsImportObj.Timeout = 10000;

        result = ST87EC_Lib_TlsProvisioningImportElement(&httpsTlsImportObj);
        if(result == RESULT_OK)
        {
          printf("\r\nImporting Amazon Root Certificate into ST87 module...\r\n");
          *prov_status = TLS_PROV_LIST;
          /* The ST87M01 will reboot after the import, so we need to wait some time
           * before starting the next sequence
           **/
          StateMachineDelay_Start(5000, HttpApiState_FirstAttach);
        }
        else
        {
          printf("Error on TLS Import: %d\r\n", result);
        }
      }
      break;

    case TLS_PROV_LIST:
      {
        ST87EC_Lib_TlsListElemObject_t    httpsTlsListObj = { 0 };
        /* --- TLS List Object initialization --- */
        httpsTlsListObj.SecureId = 0;
        httpsTlsListObj.ElementType = TLS_PROV_ELEM_CERTS;
        httpsTlsListObj.pTlsProvCallbackFunc = TlsProvListCallback;
        httpsTlsListObj.Timeout = 10000;

        result = ST87EC_Lib_TlsProvisioningListElement(&httpsTlsListObj);
        if(result == RESULT_OK)
        {
          printf("\r\nVerifying Amazon Root Certificate presence into ST87 module...\r\n");
          *prov_status = TLS_PROV_LIST_VERIFY;
        }
        else
        {
          printf("Error on TLS List: %d\r\n", result);
        }
      }
      break;

    case TLS_PROV_DELETE:
      {
        ST87EC_Lib_TlsDeleteElemObject_t  httpsTlsDeleteObj = { 0 };
        /* --- TLS Delete Object initialization --- */
        httpsTlsDeleteObj.SecureId = 0;
        httpsTlsDeleteObj.ElementType = TLS_PROV_DEL_ELEM_ALL_CERTS;
        httpsTlsDeleteObj.Timeout = 10000;

        result = ST87EC_Lib_TlsProvisioningDeleteElement(&httpsTlsDeleteObj);
        if(result == RESULT_OK)
        {
          printf("\r\nDeleting all certificates from ST87 module...\r\n");
          *prov_status = TLS_PROV_LIST;
          /* The ST87M01 will reboot after the delete, so we need to wait some time
           * before starting the next sequence
           **/
          StateMachineDelay_Start(5000, HttpApiState_FirstAttach);
        }
        else
        {
          printf("Error on TLS Delete: %d\r\n", result);
        }
      }
      break;

    case TLS_PROV_LIST_VERIFY:
      {
		if (*tls_certificate_found == TLS_LIST_FOUND_AMAZON_ROOT_CA3)
        {
          http_state = HttpApiState_Open;
          break;
        }
        else if(*tls_certificate_found == TLS_LIST_FOUND_OTHER)
        {
          printf("\r\nUnexpected certificate found in ST87 module. Deleting all certificates...\r\n");

          *prov_status = TLS_PROV_DELETE;
        }
        else if(*tls_certificate_found == TLS_LIST_NONE)
        {
          printf("\r\nAmazon Root Certificate not found in ST87 module. Importing...\r\n");

          *prov_status = TLS_PROV_IMPORT;
        }
        else if(*tls_certificate_found == TLS_LIST_WAITING)
        {
          /* still waiting for the listing result, do nothing */
        }
      }
      StateMachineDelay_Start(2000, HttpApiState_CheckTls);
      break;

    default:
      break;
  }
}


/**
 * @brief Builds a JSON-formatted HTTP body containing sensor and GNSS data.
 *
 * This function formats the provided sensor readings and GNSS coordinates into a JSON string,
 * storing the result in the given buffer. The JSON includes device ID, timestamped values for
 * temperature, humidity, pressure, accelerometer (x, y, z), and GNSS (latitude, longitude, elevation).
 *
 * @param buffer        Pointer to the buffer where the JSON string will be written.
 * @param buffer_size   Size of the buffer in bytes.
 * @param timestamp     Timestamp (in seconds) to be used for all sensor readings.
 * @param temperature   Temperature value to include in the JSON.
 * @param humidity      Humidity value to include in the JSON.
 * @param pressure      Pressure value to include in the JSON.
 * @param acc_x         Accelerometer X-axis value.
 * @param acc_y         Accelerometer Y-axis value.
 * @param acc_z         Accelerometer Z-axis value.
 * @param gps_lat       GNSS latitude value.
 * @param gps_lon       GNSS longitude value.
 * @param gps_ele       GNSS elevation value.
 *
 * @return Number of characters written to the buffer (excluding the null terminator),
 *         or a negative value if an encoding error occurs.
 */
int BuildHttpBody(char *buffer, size_t buffer_size,
                    unsigned long timestamp,
                    float temperature, float humidity, float pressure,
                    float acc_x, float acc_y, float acc_z,
                    float gps_lat, float gps_lon, float gps_ele)
{
  int written = -1;

  /* Clear buffer first (optional) */
  buffer[0] = '\0';

  if(gps_ele < -900.0f)
  {
    /* Compose the JSON string without GNSS data */
    int written = snprintf(buffer, buffer_size,
      "{"
          "\"device_id\":\"%s\","
          "\"values\":["
              "{\"ts\":%lu000,\"t\":\"tem\",\"v\":%.2f},"
              "{\"ts\":%lu000,\"t\":\"hum\",\"v\":%.2f},"
              "{\"ts\":%lu000,\"t\":\"pre\",\"v\":%.2f},"
              "{\"ts\":%lu000,\"t\":\"acc\",\"v\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}}"
          "]"
      "}",
      DASHBOARD_DEVICE_ID,
      timestamp, temperature,
      timestamp, humidity,
      timestamp, pressure,
      timestamp, acc_x, acc_y, acc_z
    );
  }
  else
  {
    /* Compose the JSON string with GNSS data */
    int written = snprintf(buffer, buffer_size,
      "{"
          "\"device_id\":\"%s\","
          "\"values\":["
              "{\"ts\":%lu000,\"t\":\"tem\",\"v\":%.2f},"
              "{\"ts\":%lu000,\"t\":\"hum\",\"v\":%.2f},"
              "{\"ts\":%lu000,\"t\":\"pre\",\"v\":%.2f},"
              "{\"ts\":%lu000,\"t\":\"acc\",\"v\":{\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}},"
              "{\"ts\":%lu000,\"t\":\"gnss\",\"v\":{\"lat\":%.6f,\"lon\":%.6f,\"ele\":%.1f}}"
          "]"
      "}",
      DASHBOARD_DEVICE_ID,
      timestamp, temperature,
      timestamp, humidity,
      timestamp, pressure,
      timestamp, acc_x, acc_y, acc_z,
      timestamp, gps_lat, gps_lon, gps_ele
    );
  }

  /* Return number of characters written or negative on error */
  return written;
}

/**
 * @brief Callback function called by the EC library when the time is retrieved the ST87 module.
 *
 * @param[in] pString Pointer to a constant character string containing the time information.
 */
static void ST87GetTimeCallback(char const * const pString)
{
  /* Example response from the modem "24/10/22,10:17:44+08" */
  char tmp_str[] = "xx/xx/xx,xx:xx:xx";
  struct tm result;

  printf("\r\n%s\r\n", pString);

  /* copying strlen(tmp_str) characters skips +-hh timezone information
     copying from pString + 1 skips '"' character */
  memcpy(tmp_str, pString + 1, strlen(tmp_str));

  strptime(tmp_str, "%y/%m/%d,%H:%M:%S", &result);

  RTC_TimeTypeDef time = { 0 };
  time.Hours = result.tm_hour;
  time.Minutes = result.tm_min;
  time.Seconds = result.tm_sec;
  time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  time.StoreOperation = RTC_STOREOPERATION_RESET;
  configASSERT((HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) == HAL_OK));

  RTC_DateTypeDef date = { 0 };
  date.WeekDay = (result.tm_wday == 0) ? RTC_WEEKDAY_SUNDAY : result.tm_wday; // HAL library Sunday is 7, tm_wday Sunday is 0
  date.Month = result.tm_mon + 1;
  date.Date = result.tm_mday;
  date.Year = result.tm_year - 100;

  configASSERT((HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) == HAL_OK));

  StateMachineDelay_Start(5000, HttpApiState_CheckTls);
}

/**
 * @brief Get the current Unix timestamp from the RTC.
 * 
 * This function retrieves the current time from the Real-Time Clock (RTC)
 * and converts it to a Unix timestamp, which is the number of seconds that
 * have elapsed since January 1, 1970 (midnight UTC/GMT).
 * 
 * @return uint32_t The current Unix timestamp.
 */
static uint32_t GetUnixTimestampFromRTC(void)
{
  uint32_t timestamp = 0;

  RTC_TimeTypeDef current_rtc_time;
  RTC_DateTypeDef current_rtc_date;
  struct tm tm;

  HAL_RTC_GetTime(&hrtc, &current_rtc_time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &current_rtc_date, RTC_FORMAT_BIN);

  tm.tm_year = current_rtc_date.Year + 100;
  tm.tm_mday = current_rtc_date.Date;
  tm.tm_mon  = current_rtc_date.Month - 1;

  tm.tm_hour = current_rtc_time.Hours;
  tm.tm_min  = current_rtc_time.Minutes;
  tm.tm_sec  = current_rtc_time.Seconds;

  timestamp = mktime(&tm);

  configASSERT(timestamp != (time_t)-1);

  return timestamp;
}


/**
 * @brief Controls the LED blinking pattern based on the EC library state.
 *
 * This function adjusts the LED blinking behavior according to the
 * registration status and ongoing sequences of the EC library.
 *
 * @param eclib_state Pointer to the current state of the EC library.
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

