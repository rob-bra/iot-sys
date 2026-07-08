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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "message_buffer.h"

#include "spi.h"
#include "i2c.h"

#include "sensor_context.h"
#include "lis2duxs12_reg.h"
#include "sht40ad1b_reg.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile Sensors_Init_State sensors_init_state = SensorsNotYetInitialized;

extern MessageBufferHandle_t xSensorDataMBHandle;
extern QueueHandle_t xDataQueue;

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

  Bus_Sensor ht_sensor = {
    .handle = &hi2c1,
    .GPIOx = NULL,
    .GPIO_Pin = 0,
    .AddressI2C = SHT40AD1B_I2C_ADDRESS
  };
  Bus_Sensor bar_sensor = {
    .handle = &hspi2,
    .GPIOx = LPS22DF_CS_GPIO_Port,
    .GPIO_Pin = LPS22DF_CS_Pin,
    .AddressI2C = 0
  };
  Bus_Sensor accel_sensor = {
    .handle = &hspi2,
    .GPIOx = LIS2DUXS12_CS_GPIO_Port,
    .GPIO_Pin = LIS2DUXS12_CS_Pin,
    .AddressI2C = 0
  };
  lis2duxs12_md_t md = {
    .fs = LIS2DUXS12_4g,
    .odr = LIS2DUXS12_200Hz_HP,
    .bw = LIS2DUXS12_ODR_div_2
  };

  // Initialize Humidity and Temperature Sensor
  printf("Humidity and Temperature sensor initialization ");
  sensor_context_init(&dev_ctx_ht, (void*)&ht_sensor);
  if(HumidityAndTemp_Init(&dev_ctx_ht) == 0)
  {
    sensors_init_state |= SensorHtInitDone;
    printf("OK!\r\n");
  }
  else
  {
    printf("FAILED!\r\n");
  }

  // Initialize Barometer Sensor
  sensor_context_init(&dev_ctx_bar, (void*)&bar_sensor);
  printf("Barometer and Temperature sensor initialization ");
  if(Barometer_Init(&dev_ctx_bar) == 0)
  {
    sensors_init_state |= SensorBarInitDone;
    printf("OK!\r\n");
  }
  else
  {
    printf("FAILED!\r\n");
  }
  // Initialize Accelerometer Sensor
  sensor_context_init(&dev_ctx_accel, (void*)&accel_sensor);
  printf("Accelerometer sensor initialization ");
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

    if(event == eEventTilt)
    {
      printf("\r\nTilt detected!\r\n");
    }
    printf("\r\nResuming Sensor Task...\r\n");

    /* Read data from Humidity and Temperature Sensor */
    HumidityAndTemp_Read(&dev_ctx_ht, &data.sensor_hum_and_temp);

    /* Read data from Barometer Sensor */
    Barometer_Read(&dev_ctx_bar, &data.sensor_barometer);

    /* Read data from Accelerometer Sensor */
    Accelerometer_Read(&dev_ctx_accel, &data.sensor_accelerometer, &md);

    data.event_type = event;

    /* Send the collected data to the message buffer */
    xMessageBufferSend(xSensorDataMBHandle, (void*)&data, sizeof(SensorsData), portMAX_DELAY);

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
  if (ret != 0)
    return -1;

  vTaskDelay(pdMS_TO_TICKS(100));

  ret = lis2duxs12_init_set(dev_ctx, LIS2DUXS12_RESET);
  do {
      lis2duxs12_status_get(dev_ctx, &status);
  } while (status.sw_reset);

  ret = lis2duxs12_init_set(dev_ctx, LIS2DUXS12_SENSOR_EMB_FUNC_ON);
  if (ret != 0)
  {
    return -1;
  }
  vTaskDelay(pdMS_TO_TICKS(10));

  lis2duxs12_tilt_mode_set(dev_ctx, 1);

  /* Configure interrupt pins */
  int1_route.tilt   = PROPERTY_ENABLE;
  lis2duxs12_emb_pin_int1_route_set(dev_ctx, &int1_route);

  lis2duxs12_embedded_int_cfg_set(dev_ctx, LIS2DUXS12_EMBEDDED_INT_LATCHED);

  int_mode.int_cfg = LIS2DUXS12_INT_LEVEL;
  lis2duxs12_int_config_set(dev_ctx, &int_mode);

  lis2duxs12_mode_set(dev_ctx, md);

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
  if (status.drdy)
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

  if (all_sources.drdy_pres)
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

  if (sht40ad1b_serial_get(dev_ctx, &serial) != SHT40AD1B_ID)
  {
    return -1;
  }

  sensors_init_state |= SensorHtInitDone;
  return 0;
}


/**
 * @brief Reads humidity and temperature values from the sensor.
 *
 * This function reads the humidity and temperature values from the sensor
 * and stores them in the provided HumidityAndTemperature structure.
 *
 * @param dev_ctx Pointer to the device context.
 * @param values Pointer to the structure where the humidity and temperature
 *               values will be stored.
 */
static void HumidityAndTemp_Read(stmdev_ctx_t *dev_ctx, HumidityAndTemperature *values)
{
  int32_t ret = sht40ad1b_data_get(dev_ctx, (float_t*)values);

  if (ret != 0)
  {
    printf("\r\nRead error!\r\n");
  }
  else
  {
    printf("Temperature [C]: %0.2f\r\nHumidity [%%RH]: %0.2f\r\n", values->temp, values->hum);
  }
}

/* USER CODE END Application */

