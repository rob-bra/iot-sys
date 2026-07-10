/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_sensors.c
 * @brief   This file contains the implementation of sensor-related
 *          functionalities for the Asset Tracking application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_sensors.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "message_buffer.h"

#include "spi.h"
#include "i2c.h"

#include "sensor_context.h"
#include "lis2duxs12_reg.h"
#include "sht40ad1b_reg.h"

// add asset tracking library - GitHub
#include "lis2duxs12_asset_tracking.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define EVENT_BUFFER_SIZE   20U
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile Sensors_Init_State sensors_init_state = SensorsNotYetInitialized;

//extern MessageBufferHandle_t xSensorDataMBHandle;   // usata se si sceglie di inviare i dati tramite Message Buffer, ora uso array circolare
extern QueueHandle_t xDataQueue;

const char *eventNames[EVENT_TYPE_COUNT] =
{
    "FIRST",
    "TIMER",
    "TILT ",
    "MLC1 ",
    "FSM  " };

/* event buffer - circular array */
static SensorsData s_eventBuffer[EVENT_BUFFER_SIZE];
static volatile uint16_t s_eventFront = 0;  // posizione di scrittura
static volatile uint16_t s_eventRear = 0;  // posizione di lettura

// variabile per tenere traccia dell'ultimo evento processato, in modo da evitare di pushare eventi consecutivi dello stesso tipo nel buffer
static SensorsData last_event_data;

// variabile per mandare immediatamente il messaggio UDP
bool send_immediately = false;

/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

static uint32_t HumidityAndTemp_Init(stmdev_ctx_t *dev_ctx);
static void HumidityAndTemp_Read(stmdev_ctx_t *dev_ctx, HumidityAndTemperature *values);

static uint32_t Accelerometer_Init(stmdev_ctx_t *dev_ctx, lis2duxs12_md_t *md);
static void Accelerometer_Read(stmdev_ctx_t *dev_ctx, AccelerometerData *values, lis2duxs12_md_t *md);

static uint32_t Barometer_Init(stmdev_ctx_t *dev_ctx);
static void Barometer_Read(stmdev_ctx_t *dev_ctx, BarometerData *values);

/* USER CODE END FunctionPrototypes */

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
 * @brief Task function to handle sensor data processing.
 *
 * This function is responsible for managing the sensor data collection,
 * processing, and any related operations. It is designed to be run as a 
 * FreeRTOS task.
 *
 * @param pvParameters Pointer to the parameters passed to the task.
 */
void SensorsDataTask(void *pvParameters)
{
  UNUSED(pvParameters);

  stmdev_ctx_t dev_ctx_ht;
  stmdev_ctx_t dev_ctx_bar;
  stmdev_ctx_t dev_ctx_accel;
  SensorsData data;

  Bus_Sensor ht_sensor =
  {
      .handle = &hi2c1,
      .GPIOx = NULL,
      .GPIO_Pin = 0,
      .AddressI2C = SHT40AD1B_I2C_ADDRESS };
  Bus_Sensor bar_sensor =
  {
      .handle = &hspi2,
      .GPIOx = LPS22DF_CS_GPIO_Port,
      .GPIO_Pin = LPS22DF_CS_Pin,
      .AddressI2C = 0 };
  Bus_Sensor accel_sensor =
  {
      .handle = &hspi2,
      .GPIOx =
      LIS2DUXS12_CS_GPIO_Port,
      .GPIO_Pin = LIS2DUXS12_CS_Pin,
      .AddressI2C = 0 };
  lis2duxs12_md_t md =
  {
      .fs = LIS2DUXS12_4g,
      .odr = LIS2DUXS12_200Hz_HP,
      .bw = LIS2DUXS12_ODR_div_2 };

  /* Enable power supply for sensors */
  HAL_GPIO_WritePin(STM32_VSENSOR_GPIO_Port, STM32_VSENSOR_Pin, GPIO_PIN_SET);
  vTaskDelay(pdMS_TO_TICKS(100));

  /* Initialize Humidity and Temperature Sensor */
  printf("Humidity and Temperature sensor initialization ");
  sensor_context_init(&dev_ctx_ht, (void*) &ht_sensor);
  if(HumidityAndTemp_Init(&dev_ctx_ht) == 0)
  {
    sensors_init_state |= SensorHtInitDone;
    printf("OK!\r\n");
  }
  else
  {
    printf("FAILED!\r\n");
  }

  /* Initialize Barometer */
  sensor_context_init(&dev_ctx_bar, (void*) &bar_sensor);
  printf("Barometer initialization ");
  if(Barometer_Init(&dev_ctx_bar) == 0)
  {
    sensors_init_state |= SensorBarInitDone;
    printf("OK!\r\n");
  }
  else
  {
    printf("FAILED!\r\n");
  }
  /* Initialize Accelerometer */
  sensor_context_init(&dev_ctx_accel, (void*) &accel_sensor);
  printf("Accelerometer initialization ");
  if(Accelerometer_Init(&dev_ctx_accel, &md) == 0)
  {
    sensors_init_state |= SensorAccInitDone;
    printf("OK!\r\n");
  }
  else
  {
    printf("FAILED!\r\n");
  }

  eEventType_t event;

  for(;;)
  {
    xQueueReceive(xDataQueue, &event, portMAX_DELAY);

    // initialize data struct with default values
    data.event_type = event;
    data.mlc_output = 0;
    data.fsm_impact = false;
    data.fsm_free_fall = false;

    /*-- MLC event --*/
    if(event == eEventMLC1)
    {
      printf("\r\nMLC event detected!\r\n");

      uint8_t mlc_src;

      /* Lettura registro MLC1_src (0x34) */
      uint32_t ret = lis2duxs12_mlc_out_get(&dev_ctx_accel, &mlc_src);

      if(ret == 0)  // 0 == OK
      {
        data.mlc_output = mlc_src;  // aggiungo val nella struct SensorsData per includerlo nel payload del pacchetto UDP

        /* States mapping based on MLC1_SRC register value */
        const char *state_str = "Unknown";
        switch(mlc_src)
        {
          case 0x00:
            state_str = "Stationary - Upright";
            break;
          case 0x04:
            state_str = "Stationary - Not upright";
            break;
          case 0x08:
            state_str = "In motion";
            break;
          case 0x0C:
            state_str = "Shaken";
            break;
        }
        printf("MLC1_SRC (0x34) = 0x%02X -> <%s>\r\n", mlc_src, state_str);
      }
      else
      {
        printf("\r\n~~~~~~~~~~ Error reading MLC output register!\r\n");
      }
    }

    /*-- FSM event --*/
    if(event == eEventFSM)
    {
      printf("\r\nFSM event detected!\r\n");
      send_immediately = true;  // setto flag per inviare subito il messaggio UDP senza aspettare il prossimo evento o il timer periodico

      lis2duxs12_fsm_status_mainpage_t fsm_status;
      int32_t ret = lis2duxs12_fsm_status_get(&dev_ctx_accel, &fsm_status);

      if(ret == 0)  // 0 == OK
      {
        printf("FSM_STATUS: fsm1(impact)=%d, fsm2(free-fall)=%d\r\n", fsm_status.is_fsm1, fsm_status.is_fsm2);

        bool impact = (fsm_status.is_fsm1 != 0);
        bool free_fall = (fsm_status.is_fsm2 != 0);

        if(impact && free_fall)
        {
          printf("FSM_OUTS1 (FSM1) -> <Impact>\r\n");
          printf("FSM_OUTS1 (FSM2) -> <Free-Fall>\r\n");
          // aggiungo info nella struct SensorsData per includerla nel payload del pacchetto UDP
          data.fsm_impact = true;
          data.fsm_free_fall = true;
        }
        else if(impact)
        {
          printf("FSM_OUTS1 (FSM1) -> <Impact>\r\n");
          data.fsm_impact = true;  // aggiungo info nella struct SensorsData per includerla nel payload del pacchetto UDP
        }
        else if(free_fall)
        {
          printf("FSM_OUTS1 (FSM2) -> <Free-Fall>\r\n");
          data.fsm_free_fall = true;
        }
        else
        {
          printf("FSM_OUTS1 -> <No event>\r\n");
        }
      }
      else
      {
        printf("\r\n~~~~~~~~~~ Error reading FSM status/outs! ret=%ld\r\n", ret);
      }
    }

    printf("\r\nResuming Sensor Task...\r\n");

    /* Read data from Humidity and Temperature Sensor */
    HumidityAndTemp_Read(&dev_ctx_ht, &data.sensor_hum_and_temp);

    /* Read data from Barometer Sensor */
    Barometer_Read(&dev_ctx_bar, &data.sensor_barometer);

    /* Read data from Accelerometer Sensor */
    Accelerometer_Read(&dev_ctx_accel, &data.sensor_accelerometer, &md);

    /* a) Send the collected data to the message buffer */
//    xMessageBufferSend(xSensorDataMBHandle, (void* )&data, sizeof(SensorsData), portMAX_DELAY);
    /* b) Pushing data into event buffer (circular array) */
    bool sameEvent = true;

    // --- se voglio far sì che eEventTimer venga sempre pushato: --------------------------
//    if(event == eEventTimer)
//    {
//      sameEvent = false;    // forza il push per gli eventi timer
//    }
//    else
//    {
//      if(EventBuffer_IsEmpty() || last_event_data.event_type != data.event_type)
//        ... aggiungo la roba sotto
//    }
    // -------------------------------------------------------------------------------------

    /* se:
     * 1) il buffer è vuoto (non ci sono eventi precedenti con cui confrontare) o se l'ultimo evento è di tipo diverso da quello corrente (es. ultimo evento MLC, nuovo evento FSM)
     * 2) l'ultimo evento è dello stesso tipo MLC ma con valori diversi (es. MLC precedente "Stationary Upright", MLC nuovo "In Motion")
     * 3) l'ultimo evento è dello stesso tipo FSM ma con valori diversi (es. FSM precedente "free-fall", FSM nuovo "impact")
     * */
    if(EventBuffer_IsEmpty() || last_event_data.event_type != data.event_type)
    {
      sameEvent = false;
    }
    else if(data.event_type == eEventMLC1 && last_event_data.mlc_output != data.mlc_output)
    {
      sameEvent = false;
    }
    else if(data.event_type == eEventFSM && (last_event_data.fsm_impact != data.fsm_impact || last_event_data.fsm_free_fall != data.fsm_free_fall))
    {
      sameEvent = false;
    }

    if(!sameEvent)
    {
      // provo a pushare l'evento nel buffer
      if(!EventBuffer_Push(&data))
      {
        printf("Event buffer full, dropping event type %d\r\n", data.event_type);
      }
      else
      {
        last_event_data = data;  // aggiorno ultimo evento con quello appena pushato
      }
    }
    else
    {
      printf("Event type %d is the same as the last event, not pushing to buffer to avoid duplicate UDP packets\r\n", data.event_type);
    }

    PrintEventBufferContents();
    printf("Event pushed. Buffer count = %u\r\n", EventBuffer_Count());

    // print buffer content for debugging
//    printf("s_eventBuffer values: \r\n");
//    for(uint16_t i = 0; i < EVENT_BUFFER_SIZE; i++)
//    {
//      printf("Index %d: Event Type = %d, MLC Output = 0x%02X\r\n", i, s_eventBuffer[i].event_type, (int)s_eventBuffer[i].mlc_output);
//    }
  }
}

/**
 * @brief Initialize the accelerometer sensor.
 *
 * This function initializes the accelerometer sensor with the given device context and mode settings.
 *
 * @param dev_ctx Pointer to the device context.
 * @param md Pointer to the mode settings structure.
 * @return uint32_t Returns 0 on success, or an error code on failure.
 */
static uint32_t Accelerometer_Init(stmdev_ctx_t *dev_ctx, lis2duxs12_md_t *md)
{
  int32_t ret;
  lis2duxs12_status_t status;
  lis2duxs12_emb_pin_int_route_t int1_route;
  lis2duxs12_int_config_t int_mode;

  ret = lis2duxs12_exit_deep_power_down(dev_ctx);
  if(ret != 0)
    return -1;

  vTaskDelay(pdMS_TO_TICKS(100));

  ret = lis2duxs12_init_set(dev_ctx, LIS2DUXS12_RESET);
  do
  {
    lis2duxs12_status_get(dev_ctx, &status);
  }
  while(status.sw_reset);

  /* Load the UCF for the detection of Impact (FSM #1) and Free-fall (FSM #2).
   * The accelerometer is configured with ±16 g full-scale and 25 Hz output data rate in low-power mode.
   *
   * In this example, the threshold for impact detection is set to 0.5 g, and the angle for upright is set to 26°.
   **/
  for(uint32_t i = 0; i < MEMS_CONF_ARRAY_LEN(lis2duxs12_asset_tracking_conf_0); i++)
  {
    const struct mems_conf_op *op = &lis2duxs12_asset_tracking_conf_0[i];
    switch(op->type)
    {
      case MEMS_CONF_OP_TYPE_WRITE:
        ret = lis2duxs12_write_reg(dev_ctx, op->address, (uint8_t*) &op->data, 1);
        if(ret != 0)
        {
          return 1;
        }
        break;
      case MEMS_CONF_OP_TYPE_DELAY:
        vTaskDelay(pdMS_TO_TICKS(op->data));
        break;
      case MEMS_CONF_OP_TYPE_POLL_SET:
      case MEMS_CONF_OP_TYPE_POLL_RESET:
      default:
        return 1;
    }
  }

  /*  ret = lis2duxs12_init_set(dev_ctx, LIS2DUXS12_SENSOR_EMB_FUNC_ON);
   if (ret != 0)
   {
   return -1;
   }
   vTaskDelay(pdMS_TO_TICKS(10));

   lis2duxs12_tilt_mode_set(dev_ctx, 1);

   // Configure interrupt pins
   int1_route.tilt   = PROPERTY_ENABLE;
   lis2duxs12_emb_pin_int1_route_set(dev_ctx, &int1_route);

   lis2duxs12_embedded_int_cfg_set(dev_ctx, LIS2DUXS12_EMBEDDED_INT_LATCHED);

   int_mode.int_cfg = LIS2DUXS12_INT_LEVEL;
   lis2duxs12_int_config_set(dev_ctx, &int_mode);

   lis2duxs12_mode_set(dev_ctx, md);
   */

  return 0;
}

/**
 * @brief Reads data from the accelerometer sensor.
 *
 * This function reads the accelerometer data from the specified device context
 * and stores the values in the provided AccelerometerData structure.
 *
 * @param dev_ctx Pointer to the device context.
 * @param values Pointer to the structure where the accelerometer data will be stored.
 * @param md Pointer to the measurement data structure.
 */
static void Accelerometer_Read(stmdev_ctx_t *dev_ctx, AccelerometerData *values, lis2duxs12_md_t *md)
{
  lis2duxs12_xl_data_t data_xl;
  lis2duxs12_status_t status;
  lis2duxs12_embedded_status_t emb_status;

  /* Read INT flag */
  lis2duxs12_embedded_status_get(dev_ctx, &emb_status);

  lis2duxs12_status_get(dev_ctx, &status);
  if(status.drdy)
  {
    lis2duxs12_xl_data_get(dev_ctx, md, &data_xl);

    values->x = data_xl.mg[0];
    values->y = data_xl.mg[1];
    values->z = data_xl.mg[2];

    printf("Accelerometer [mg]: x=%4.2f\ty=%4.2f\tz=%4.2f\r\n", values->x, values->y, values->z);
  }
  else
  {
    printf("\r\nRead error!\r\n");
  }
}

/**
 * @brief  Initializes the barometer sensor.
 * @param  dev_ctx: Pointer to the device context.
 * @retval uint32_t: Status of the initialization (0 for success, non-zero for error).
 */
static uint32_t Barometer_Init(stmdev_ctx_t *dev_ctx)
{
  lps22df_id_t id;
  lps22df_bus_mode_t bus_mode;
  lps22df_md_t md;
  lps22df_pin_int_route_t int_route;
  int32_t ret;

  while(id.whoami != LPS22DF_ID)
  {
    lps22df_id_get(dev_ctx, &id);
  }

  ret = lps22df_init_set(dev_ctx, LPS22DF_BOOT);
  if(ret != 0)
    return -1;

  vTaskDelay(pdMS_TO_TICKS(100));

  ret = lps22df_init_set(dev_ctx, LPS22DF_RESET);
  if(ret != 0)
    return -1;

  ret = lps22df_init_set(dev_ctx, LPS22DF_DRV_RDY);
  if(ret != 0)
    return -1;

  bus_mode.filter = LPS22DF_FILTER_AUTO;
  bus_mode.interface = LPS22DF_SEL_BY_HW;
  lps22df_bus_mode_set(dev_ctx, &bus_mode);

  md.odr = LPS22DF_4Hz;
  md.avg = LPS22DF_16_AVG;
  md.lpf = LPS22DF_LPF_ODR_DIV_4;
  lps22df_mode_set(dev_ctx, &md);

  lps22df_pin_int_route_get(dev_ctx, &int_route);
  int_route.drdy_pres = PROPERTY_DISABLE;
  lps22df_pin_int_route_set(dev_ctx, &int_route);

  sensors_init_state |= SensorBarInitDone;
  return 0;
}

/**
 * @brief Reads barometer data from the sensor.
 *
 * This function reads the barometer data from the sensor and stores the
 * values in the provided BarometerData structure.
 *
 * @param dev_ctx Pointer to the device context.
 * @param values Pointer to the structure where the barometer data will be stored.
 */
static void Barometer_Read(stmdev_ctx_t *dev_ctx, BarometerData *values)
{
  lps22df_all_sources_t all_sources;
  lps22df_data_t data;

  lps22df_all_sources_get(dev_ctx, &all_sources);

  if(all_sources.drdy_pres)
  {
    lps22df_data_get(dev_ctx, &data);

    values->pres = data.pressure.hpa;

    printf("Pressure [hPa]: %6.2f\r\n", values->pres);
  }
  else
  {
    printf("\r\nRead error!\r\n");
  }
}

/**
 * @brief Initializes the humidity and temperature sensor.
 *
 * This function sets up the humidity and temperature sensor for operation.
 *
 * @param dev_ctx Pointer to the device context.
 * @return uint32_t Returns 0 on success, or an error code on failure.
 */
static uint32_t HumidityAndTemp_Init(stmdev_ctx_t *dev_ctx)
{
  uint32_t serial;

  if(sht40ad1b_serial_get(dev_ctx, &serial) != SHT40AD1B_ID)
  {
    return -1;
  }

  sensors_init_state |= SensorHtInitDone;
  return 0;
}

/**
 * @brief Reads humidity and temperature values from the sensor.
 *
 * This function reads the humidity and temperature values from  the sensor
 * and stores them in the provided HumidityAndTemperature structure.
 *
 * @param dev_ctx Pointer to the device context.
 * @param values Pointer to the structure where the humidity and temperature
 *               values will be stored.
 */
static void HumidityAndTemp_Read(stmdev_ctx_t *dev_ctx, HumidityAndTemperature *values)
{
  int32_t ret = sht40ad1b_data_get(dev_ctx, (float_t*) values);

  if(ret != 0)
  {
    printf("\r\nRead error!\r\n");
  }
  else
  {
    printf("Temperature [C]: %0.2f\r\nHumidity [%%RH]: %0.2f\r\n", values->temp, values->hum);
  }
}

//// Function to clear the event buffer     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void clearBuffer(SensorsData arr[], int *count) {
//    // Reset all elements to 0
//    memset(arr, 0, EVENT_BUFFER_SIZE * sizeof(SensorsData));
//    // Reset logical size
//    *count = 0;
//}df

/**
 * @brief Checks if the event buffer is empty.
 *
 * This function checks if the event buffer is empty by comparing the front and rear indices.
 *
 * @return true if the event buffer is empty, false otherwise.
 */
bool EventBuffer_IsEmpty(void)
{
  return (s_eventFront == s_eventRear);
}

/**
 * @brief Checks if the event buffer is full.
 *
 * This function checks if the event buffer is full by comparing the front and rear indices.
 *
 * @return true if the event buffer is full, false otherwise.
 */
bool EventBuffer_IsFull(void)
{
  /* Controlla se la coda corrisponde alla posizione immediatamente successiva alla testa della lista */
  return ((s_eventFront + 1U) % EVENT_BUFFER_SIZE == s_eventRear);
}

/**
 * @brief Pushes a new event into the event buffer.
 *
 * This function adds a new event to the event buffer if there is space available.
 *
 * @param pData Pointer to the SensorsData structure containing the event data to be added.
 * @return true if the event was successfully added to the buffer, false if the buffer is full.
 */
bool EventBuffer_Push(const SensorsData *pData)
{
  uint16_t nextFront = (s_eventFront + 1U) % EVENT_BUFFER_SIZE;

  if(nextFront == s_eventRear)
  {
    // Se è un evento critico (FSM), scarto il più vecchio per fare spazio
    if(pData->event_type == eEventFSM)
    {
      printf("Event buffer full, dropping oldest event to store new FSM event\r\n");
      // avanzo il rear di uno: butto via l'evento più vecchio
      s_eventRear = (s_eventRear + 1U) % EVENT_BUFFER_SIZE;
    }
    else
    {
      // per eventi non critici non sovrascrivo
      return false;
    }
  }
  s_eventBuffer[s_eventFront] = *pData;
  s_eventFront = nextFront;
  return true;
}

/**
 * @brief Pops an event from the event buffer.
 *
 * This function retrieves and removes the oldest event from the event buffer if it is not empty.
 *
 * @param pData Pointer to the SensorsData structure where the popped event data will be stored.
 * @return true if an event was successfully popped from the buffer, false if the buffer is empty.
 * */
bool EventBuffer_Pop(SensorsData *pData)
{
  if(EventBuffer_IsEmpty())
  {
    return false;
  }
  *pData = s_eventBuffer[s_eventRear];
  printf("\r\nPopped event type %d from buffer index %u\r\n", pData->event_type, (unsigned) s_eventRear);
  s_eventRear = (s_eventRear + 1U) % EVENT_BUFFER_SIZE;
  return true;

}

void PrintEventBufferContents(void)
{
  printf("\r\nEvent Buffer Contents:\r\n");

  if(EventBuffer_IsEmpty())
  {
    printf("Event buffer is empty!!\r\n");
    return;
  }

  uint16_t idx = s_eventRear;

  while(idx != s_eventFront)
  {
    const SensorsData *ev = &s_eventBuffer[idx];
    printf("Index %u: Event Type = %s, MLC Output = 0x%02X, FSM_IMPACT=%d, FSM_FF=%d\r\n", (unsigned) idx, eventNames[ev->event_type], (int) ev->mlc_output,
           ev->fsm_impact, ev->fsm_free_fall);

    idx = (idx + 1U) % EVENT_BUFFER_SIZE;
  }
}

/**
 * @brief Counts the number of events currently stored in the event buffer.
 *
 * This function calculates the number of events in the buffer by comparing the front and rear indices.
 *
 * @return The number of events currently stored in the event buffer.
 */
uint16_t EventBuffer_Count(void)
{
  if(s_eventFront >= s_eventRear)
  {
    return (uint16_t) (s_eventFront - s_eventRear);
  }
  else
  {
    return (uint16_t) (EVENT_BUFFER_SIZE - s_eventRear + s_eventFront);
  }
}

/* USER CODE END Application */

