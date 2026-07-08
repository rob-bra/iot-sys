/**
 ******************************************************************************
 * @file    NBIOTV1_env_sensors.h
 * @author  SRA
 * @brief   This file contains definitions for NBIOTV1_env_sensors.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef NBIOTV1_ENV_SENSORS_H__
#define NBIOTV1_ENV_SENSORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "NBIOTV1.h"
#include "env_sensor.h"

#if (USE_ENV_SENSOR_LPS22DF == 1)
#include "lps22df.h"
#endif

#if (USE_ENV_SENSOR_SHT40 == 1)
#include "sht40ad1b.h"
#endif

#if (USE_ENV_SENSOR_LPS22DF == 1)
#define LPS22DF (0)
#endif

#if (USE_ENV_SENSOR_SHT40 == 1)
#define SHT40 (1)
#endif

/* Environmental Sensor instance Info */
typedef struct
{
  uint8_t Temperature;
  uint8_t Pressure;
  uint8_t Humidity;
  uint8_t LowPower;
  float_t HumMaxOdr;
  float_t TempMaxOdr;
  float_t PressMaxOdr;
} ENV_SENSOR_Capabilities_t;

typedef struct
{
  uint32_t Functions;
} ENV_SENSOR_Ctx_t;

#define ENV_TEMPERATURE      1U
#define ENV_PRESSURE         2U
#define ENV_HUMIDITY         3U

#define ENV_FUNCTIONS_NBR    3U
#define ENV_INSTANCES_NBR    (USE_ENV_SENSOR_LPS22DF + USE_ENV_SENSOR_SHT40)

#if (ENV_INSTANCES_NBR == 0)
#error "No environmental sensor instance has been selected"
#endif

int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions);
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance);
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities);
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id);
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function);
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr);
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr);
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value);
void BSP_LPS22DF_GPIO_Callback(void);

#ifdef __cplusplus
}
#endif

#endif /* NBIOTV1_ENV_SENSORS_H__ */
