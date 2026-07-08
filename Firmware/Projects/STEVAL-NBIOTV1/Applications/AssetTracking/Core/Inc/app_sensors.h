/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.h
  * Description        : FreeRTOS applicative header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_FREERTOS_H__
#define __APP_FREERTOS_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sensor_context.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef enum eEventType_t
{
  eEventTimer = 0,
  eEventTilt,
  eEventGNSS
} eEventType_t;

typedef struct HumidityAndTemperature
{
  float hum;
  float temp;
} HumidityAndTemperature;

typedef struct AccelerometerData
{
  float x;
  float y;
  float z;
} AccelerometerData;

typedef struct BarometerData
{
  float pres;
} BarometerData;

typedef struct SensorsData
{
  HumidityAndTemperature sensor_hum_and_temp;
  AccelerometerData sensor_accelerometer;
  BarometerData sensor_barometer;
  eEventType_t event_type;
} SensorsData;

typedef enum Sensors_Init_State {
  SensorsNotYetInitialized = 0,
  SensorAccInitDone = 1,
  SensorBarInitDone = 2,
  SensorHtInitDone = 4,
  SensorsInitialized = SensorAccInitDone | SensorBarInitDone | SensorHtInitDone
} Sensors_Init_State;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Exported macro -------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void SensorsDataTask(void *pvParameters);

/* USER CODE END FunctionPrototypes */

#ifdef __cplusplus
}
#endif
#endif /* __APP_FREERTOS_H__ */
