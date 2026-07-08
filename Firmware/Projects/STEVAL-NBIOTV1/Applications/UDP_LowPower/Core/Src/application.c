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
typedef enum UDP_API_State
{
  UdpApiState_Init = 0,
  UdpApiState_Idle,
  UdpApiState_GoToSleep,
  UdpApiState_RtcInitSequence,
  UdpApiState_TransferSequence,
  UdpApiState_TransferComplete,
} UDP_API_State;

/* Private define ------------------------------------------------------------*/

/* ----------------------------------------*/
/* Configurable parameters*/

/* Uncomment one of the following lines to enable the corresponding low power state */
//#define LOW_POWER_MODE_STOP2
#define LOW_POWER_MODE_SLEEP

/* Select the application sleep time */
//#define APPLICATION_SLEEP_TIME_S        50U     //----------------------------------------- MODIFICATO GIU' PER TIMING DI INVIO MEX UDP
/* ECHO server IP address and port */
//#define TCP_SERVER_ADDRESS        "domain.name"
#define UDP_SERVER_IP             "31.14.134.210"
#define UDP_SERVER_PORT           1234U

/* No security profile used */
#define SECURE_ID              -1

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
#define SENSORS_SAMPLE_INTERVAL_S       60U         // 1 minute
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
static TaskHandle_t xUDPTaskHandle = NULL;
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
volatile UDP_API_State udp_state = UdpApiState_Init;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void UDPTask(void *pvParameters);

static void LedBlinking(ST87EC_Lib_Status_t *eclib_state);

static void UdpTransferReadCallback(char const *const pString);
static void GetTimeCallback(char const *const pString);
static uint32_t GetCurrentTimeSeconds(void);

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
  BaseType_t xReturned; // var per controllare l'esito di creazione del task

  xReturned = xTaskCreate(UDPTask, "udp_task",
  UDP_TASK_STACK_SIZE,
                          NULL,
                          UDP_TASK_PRIORITY,
                          &xUDPTaskHandle);

  configASSERT(xReturned == pdPASS);  // verifica di corretta creazione del task

  xReturned = xTaskCreate(SensorsDataTask, "sensors_task",
  SENSOR_TASK_STACK_SIZE,
                          NULL,
                          SENSOR_TASK_PRIORITY,
                          &xSensorsDataTaskHandle);

  configASSERT(xReturned == pdPASS);

  /* Creation of the Queue for 2 Tasks */
  xDataQueue = xQueueCreate(2, sizeof(eEventType_t)); // (LUNGHEZZA CODA, DIMENSIONE ELEMENTO NELLA CODA)
  configASSERT(xDataQueue != NULL);

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
static void UDPTask(void *pvParameters)
{
  UNUSED(pvParameters);

  ST87EC_Lib_Result_t result;
  ST87EC_Lib_Status_t eclib_state;
  ST87EC_Lib_UdpTcpObject_t UdpObject;
  char buffer[ST87_UDP_TASK_MSG_BUF_SIZE];
  SensorsData data;

  printf("\r\n\r\n--------------- NB-IoT UDP application init ---------------\r\n");
  printf("Waiting for network registration...\r\n");

  /* EC lib initialization */
  result = ST87EC_Lib_Init(NULL);
  configASSERT(result == RESULT_OK);

#if defined(UDP_SERVER_ADDRESS)
  UdpObject.AddressType = URL;
  UdpObject.pHost = UDP_SERVER_ADDRESS;
#elif defined (UDP_SERVER_IP)
  UdpObject.AddressType = IPV4;
  UdpObject.pHost = UDP_SERVER_IP;
#endif
  UdpObject.PortNb = UDP_SERVER_PORT;
  UdpObject.SecureId = SECURE_ID;
  UdpObject.pDataTx = buffer;
  UdpObject.DataTxLength = 0;
  UdpObject.TimeoutMs = REQUEST_TIMEOUT_DEFAULT;
  UdpObject.LastPacket = LAST_PKT_TRUE;
  UdpObject.pTransferReadCallbackFunc = UdpTransferReadCallback;

  memset(buffer, 0, ST87_UDP_TASK_MSG_BUF_SIZE);  // buffer is the transmitted message, initialized to 0

  for(;;)
  {
    /* Run EC Lib main sequence */
    result = ST87EC_Lib_Scheduler();

    if(result == RESULT_KO)
    {
      printf("\r\nThere was an error in sequence execution...\r\n");
      printf("\r\nModem reset underway...\r\n");
      ST87EC_Lib_Reset();
      udp_state = UdpApiState_Init;
      printf("\r\nModem reset complete...\r\n");
    }

    ST87EC_Lib_GetState(&eclib_state);

    /* Modem is attached to the network and no sequence is currently active */
    if((eclib_state.RegistrationStatus == REGISTERED) && (eclib_state.OnGoingSequence == SEQUENCE_NONE))
    {
      switch(udp_state)
      {
        case UdpApiState_Init:
          {
            printf("Registration complete. Attached to NB-IoT network!\r\n\r\n");

            configASSERT(ST87EC_Lib_GetTime(GetTimeCallback, REQUEST_TIMEOUT_DEFAULT) == RESULT_OK);
            udp_state = UdpApiState_RtcInitSequence;
            break;
          }
        case UdpApiState_RtcInitSequence:
          {
            /* Nothing specific to do during RtcInit sequence for now */
            /* State will be changed in GetTimeCallback */
            break;
          }
        case UdpApiState_GoToSleep:
          {
            printf("\r\nApplication is ready to go to sleep for %ds...\r\n", APPLICATION_SLEEP_TIME_S);
            printf("Waiting for ST87M01 to go to sleep as well...\r\n");

            /* Enable LPTIM1 running even when STOP2 mode is active */
            /* ToDo: move this out of main loop?? */
            __HAL_RCC_LPTIM1_CLKAM_ENABLE();
            __HAL_RCC_RTCAPB_CLKAM_ENABLE();

            HAL_LPTIM_TimeOut_Start_IT(&hlptim1, LPTIM_PERIOD);

            udp_state = UdpApiState_Idle;
            break;
          }
        case UdpApiState_Idle:
//          {
//            if((eclib_state.SleepWakeupstatus == STATUS_SLEEP) || !EventBuffer_IsEmpty())
//            {
//              SensorsData ev;
//              // riempio il batch fino a quando c'è spazio e ci sono eventi in coda
//              while((batch_count < EVENT_BATCH_SIZE) && EventBuffer_Pop(&ev))
//              {
//                batch[batch_count++] = ev;
//
//                if(send_immediately)
//                {
//                  // se è arrivato un evento che richiede invio immediato, esco subito dal ciclo di riempimento del batch per inviare subito il pacchetto UDP, anche se il batch non è ancora pieno
//                  break;
//                }
//
//                // Test log
////                printf("BatchFill: idx=%u, event_type=%d, mlc=0x%02" PRIX32 ", fsm_imp=%d, fsm_ff=%d\r\n", (unsigned) (batch_count - 1), ev.event_type,
////                       ev.mlc_output, ev.fsm_impact, ev.fsm_free_fall);
//              }
//
//              if(send_immediately || batch_count >= EVENT_BATCH_SIZE) // batch riempito
//              {
//                // costruzione del payload (batch)
//                size_t used = 0;  // numero di byte usati finora nel buffer del messaggio
//                size_t remaining = ST87_UDP_TASK_MSG_BUF_SIZE;
//
//                // aggiungo header al pacchetto da trasmettere, per indicare l'inizio del batch
//                used += snprintf(UdpObject.pDataTx + used, remaining - used, "EVENT_BATCH_START\n");
//
//                for(uint16_t i = 0; i < batch_count; i++)
//                {
//                  const SensorsData *pev = &batch[i];
//                  const char *kind_str = "GENERIC";
//
//                  /*-- MLC event --*/
//                  if(pev->event_type == eEventMLC1)
//                  {
//                    const char *state_str = "Unknown";
//                    switch(pev->mlc_output)
//                    {
//                      case 0x00:
//                        state_str = "Stationary_Upright";
//                        break;
//                      case 0x04:
//                        state_str = "Stationary_NotUpright";
//                        break;
//                      case 0x08:
//                        state_str = "InMotion";
//                        break;
//                      case 0x0C:
//                        state_str = "Shaken";
//                        break;
//                    }
//                    kind_str = "MLC";
//                    used += snprintf(UdpObject.pDataTx + used, remaining - used, "TYPE=%s;MLC=0x%02" PRIX32 " (%s)\n", kind_str, pev->mlc_output, state_str);
//                  }
//                  /*-- FSM event --*/
//                  else if(pev->event_type == eEventFSM)
//                  {
//                    const char *fsm_state_str = "Unknown";
//                    if(pev->fsm_impact)
//                      fsm_state_str = "Impact";
//                    else if(pev->fsm_free_fall)
//                      fsm_state_str = "FreeFall";
//
//                    kind_str = "FSM";
//                    used += snprintf(UdpObject.pDataTx + used, remaining - used, "TYPE=%s;FSM_STATE=%s\n", kind_str, fsm_state_str);
//                  }
//                  else
//                  {
//                    kind_str = "PERIODIC";
//                    used += snprintf(UdpObject.pDataTx + used, remaining - used, "TYPE=%s;Temp=%0.2f;Hum=%0.2f;Press=%0.2f\n", kind_str,
//                                     pev->sensor_hum_and_temp.temp, pev->sensor_hum_and_temp.hum, pev->sensor_barometer.pres);
//                  }
//                }
//                // fine del batch
//                used += snprintf(UdpObject.pDataTx + used, remaining - used, "EVENT_BATCH_END\n");
//
//                printf("\r\nSending UDP packet with %u events...\r\n", (unsigned) batch_count);
//                UdpObject.DataTxLength = (uint32_t) used;
//
//                // TEST: mostro il contenuto del payload prima di inviarlo
////                printf("UDP payload length = %lu\r\n", (unsigned long) UdpObject.DataTxLength);
////                printf("UDP payload:\r\n%.*s\r\n", (int) UdpObject.DataTxLength, UdpObject.pDataTx);
//
//                ST87EC_Lib_NBIOT_UdpTransferData(&UdpObject);
//
//                batch_count = 0;  // svuoto il batch
//                send_immediately = false;  // resetto flag di invio immediato dopo aver inviato il batch
//                udp_state = UdpApiState_TransferSequence;
//              }
//            }
//            break;
//          }


          // ------ NEW CODE WITH TIMING --------------------------------------------------------------------------------
          {
            uint32_t now = GetCurrentTimeSeconds();
            bool timeout_send = false;

            // 1) Verifico se è passato abbastanza tempo dall'ultimo invio UDP da giustificare l'invio del batch anche se non è pieno (timeout)
            uint32_t delta_send = now - last_udp_send_time_s;
            if(delta_send >= UDP_SEND_INTERVAL_S)
            {
              printf("\r\nUDP send interval timeout! It's been %lu seconds since last UDP send. Will try to send batch even if it's not full...\r\n",
                     (unsigned long) delta_send);
              timeout_send = true;
            }

            if((eclib_state.SleepWakeupstatus == STATUS_SLEEP) || !EventBuffer_IsEmpty() || timeout_send || send_immediately)
            {
              SensorsData ev;

              // 2) Se ho timeout_send MA il batch è vuoto -> provo a fare un pop
              if(timeout_send && (batch_count == 0))
              {
                if(EventBuffer_Pop(&ev))
                {
                  batch[batch_count++] = ev;
                  last_batch_sample_time_s = now;
                }
                else
                {
                  printf("\r\nBATCH TIMEOUT but NO events in buffer to send! Resetting timeout flag and waiting for next event or next timeout...\r\n");
                  timeout_send = false; // resetto flag di timeout se non ho eventi da inviare, aspetto il prossimo evento o il prossimo timeout
                }
              }

              // 3) Riempio il batch finché c'è spazio e ci sono eventi
              while((batch_count < EVENT_BATCH_SIZE) && EventBuffer_Pop(&ev))
              {
                batch[batch_count++] = ev;
                last_batch_sample_time_s = now; // ultimo evento aggiunto al batch

                if(send_immediately)
                {
                  // (per ora non lo usiamo ancora, lo sfrutteremo per FSM)
                  break;
                }
              }

              // 4) Decide se inviare: batch pieno OPPURE timeout scaduto
              if((batch_count >= EVENT_BATCH_SIZE) || ((timeout_send || send_immediately) && (batch_count > 0)))
              {
                // costruzione del payload
                size_t used = 0;
                size_t remaining = ST87_UDP_TASK_MSG_BUF_SIZE;

                used += snprintf(UdpObject.pDataTx + used, remaining - used, "EVENT_BATCH_START\n");

                for(uint16_t i = 0; i < batch_count; i++)
                {
                  const SensorsData *pev = &batch[i];
                  const char *kind_str = "GENERIC";

                  if(pev->event_type == eEventMLC1)
                  {
                    const char *state_str = "Unknown";
                    switch(pev->mlc_output)
                    {
                      case 0x00:
                        state_str = "Stationary_Upright";
                        break;
                      case 0x04:
                        state_str = "Stationary_NotUpright";
                        break;
                      case 0x08:
                        state_str = "InMotion";
                        break;
                      case 0x0C:
                        state_str = "Shaken";
                        break;
                    }
                    kind_str = "MLC";
                    used += snprintf(UdpObject.pDataTx + used, remaining - used, "TYPE=%s;MLC=0x%02" PRIX32 " (%s)\n", kind_str, pev->mlc_output, state_str);
                  }
                  else if(pev->event_type == eEventFSM)
                  {
                    const char *fsm_state_str = "Unknown";
                    if(pev->fsm_impact)
                      fsm_state_str = "Impact";
                    else if(pev->fsm_free_fall)
                      fsm_state_str = "FreeFall";

                    kind_str = "FSM";
                    used += snprintf(UdpObject.pDataTx + used, remaining - used, "TYPE=%s;FSM_STATE=%s\n", kind_str, fsm_state_str);
                  }
                  else
                  {
                    // tutti gli altri (es. eEventTimer) li tratti come "campione sensori"
                    kind_str = "SAMPLE";
                    used += snprintf(UdpObject.pDataTx + used, remaining - used, "TYPE=%s;Temp=%0.2f;Hum=%0.2f;Press=%0.2f\n", kind_str,
                                     pev->sensor_hum_and_temp.temp, pev->sensor_hum_and_temp.hum, pev->sensor_barometer.pres);
                  }
                }

                used += snprintf(UdpObject.pDataTx + used, remaining - used, "EVENT_BATCH_END\n");

                printf("\r\nSending UDP packet with %u events...\r\n", (unsigned) batch_count);
                UdpObject.DataTxLength = (uint32_t) used;

                ST87EC_Lib_NBIOT_UdpTransferData(&UdpObject);

                batch_count = 0;                   // svuota batch
                send_immediately = false;
                last_udp_send_time_s = now;        // aggiorno ultimo invio
                udp_state = UdpApiState_TransferSequence;
              }
            }
            break;
          }

          // ------ ORIGINAL CODE --------------------------------------------------------------------------------
//        case UdpApiState_Idle:
//          {
//            if((eclib_state.SleepWakeupstatus == STATUS_SLEEP) || !EventBuffer_IsEmpty())
//            {
//              /* Wait for new messages from sensor task. Timeout in order to execute the scheduler */
//              SensorsData data;
//
//              /* Pop data from event buffer */
//              bool pop_result = EventBuffer_Pop(&data);
//
//              if(pop_result) // buffer not empty
//              {
//                uint32_t msg_size;
//
//                if(data.event_type == eEventMLC1)
//                {
//                  // Pacchetto MLC
//                  const char *state_str = "Unknown";
//                  switch(data.mlc_output)
//                  {
//                    case 0x00:
//                      state_str = "Stationary_Upright";
//                      break;
//                    case 0x04:
//                      state_str = "Stationary_NotUpright";
//                      break;
//                    case 0x08:
//                      state_str = "InMotion";
//                      break;
//                    case 0x0C:
//                      state_str = "Shaken";
//                      break;
//                  }
//                  msg_size = (uint32_t) snprintf(UdpObject.pDataTx, ST87_UDP_TASK_MSG_BUF_SIZE, "MLC_STATE=0x%02X (%s)", (unsigned int) data.mlc_output,
//                                                 state_str);
//                }
//                else if(data.event_type == eEventFSM)
//                {
//                  const char *fsm_state_str = "Unknown";
//                  if(data.fsm_impact)
//                  {
//                    fsm_state_str = "Impact";
//                  }
//                  else if(data.fsm_free_fall)
//                  {
//                    fsm_state_str = "FreeFall";
//                  }
//                  msg_size = (uint32_t) snprintf(UdpObject.pDataTx, ST87_UDP_TASK_MSG_BUF_SIZE, "FSM_STATE (%s)", fsm_state_str);
////                  msg_size = (uint32_t) snprintf(UdpObject.pDataTx, ST87_UDP_TASK_MSG_BUF_SIZE, "FSM_IMPACT=%s, FSM_FREE_FALL=%s",
////                                                 data.fsm_impact ? "true" : "false", data.fsm_free_fall ? "true" : "false");
//                }
//                else
//                {
//                  msg_size = (uint32_t) snprintf(UdpObject.pDataTx, ST87_UDP_TASK_MSG_BUF_SIZE, "Temp=%0.2f[C] - Hum=%0.2f[pRH] "
//                                                 "- Press=%0.2f[hPa] - Acc x=%0.3f[mg], y=%0.3f[mg], z=%0.3f[mg]",
//                                                 data.sensor_hum_and_temp.temp, data.sensor_hum_and_temp.hum, data.sensor_barometer.pres,
//                                                 data.sensor_accelerometer.x, data.sensor_accelerometer.y, data.sensor_accelerometer.z);
//                }
//
//                printf("\r\nSending UDP packet...\r\n");
//                // TODO: tolgo commento per mostrare il contenuto del payload
////                printf("\r\nPayload content: \r\n%s\r\n", buffer);
//
//                UdpObject.DataTxLength = msg_size;
//                ST87EC_Lib_NBIOT_UdpTransferData(&UdpObject);
//
//                PrintEventBufferContents();
//
//                udp_state = UdpApiState_TransferSequence;
//              }
//              else
//              {
//                /* Timeout: no data received from sensor task.
//                 * Retain the status but unlock the task in order to call the scheduler */
//              }
//            }
//            break;
//          }
        case UdpApiState_TransferSequence:
          {
            // TEST ////////////////////////////////////////////////////////
//            printf("[UDPTask] State = TransferSequence\r\n");
            /////////////////////////////////////////////////////////////
            /* Nothing specific to, waiting for UDP transfer Callback */
            break;
          }
        case UdpApiState_TransferComplete:
          {
            // TEST ////////////////////////////////////////////////////////
//            printf("[UDPTask] State = TransferComplete\r\n");
            /////////////////////////////////////////////////////////////

            printf("\r\nTransfer complete...\r\n");
            udp_state = UdpApiState_GoToSleep;
            break;
          }
        default:
          break;
      }
    }
    LedBlinking(&eclib_state);
  }
}

/**
 * @brief Callback function for UDP receive completion.
 *
 * This function is called when a UDP transfer is completed. It updates the
 * application state and prints the received data if available.
 *
 * @param pString Pointer to the received string data. If NULL, no data was received.
 */
static void UdpTransferReadCallback(char const *const pString)
{
  udp_state = UdpApiState_TransferComplete;

  printf("\r\n#IPRECV: ");
  if(pString != NULL)
  {
    printf("%s", (const char*) pString);
  }
  printf("\r\n");
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
  last_batch_sample_time_s = now;
//  last_udp_send_time_s = now;

  /* Wakeup the sensors data task */
  eEventType_t event = eEventFirst;
  configASSERT(xDataQueue != NULL);
  xQueueSend(xDataQueue, &event, portMAX_DELAY);

  udp_state = UdpApiState_Idle;
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
