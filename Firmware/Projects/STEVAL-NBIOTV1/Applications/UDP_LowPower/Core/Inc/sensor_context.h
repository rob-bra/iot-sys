/**
  ******************************************************************************
  * @file           : sensor_context.h
  * @brief          : Sensor context initialization. It depends on MCU and board
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
#ifndef __SENSOR_CONTEXT_H
#define __SENSOR_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private includes ----------------------------------------------------------*/
#include "stm32u5xx.h"
#include "lis2duxs12_reg.h"
#include "lps22df_reg.h"
/* Exported types ------------------------------------------------------------*/
typedef struct
{
  void *handle;
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;
  uint8_t AddressI2C;
} Bus_Sensor;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void sensor_context_init(stmdev_ctx_t *dev_ctx, void *handle);
/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __SENSOR_CONTEXT_H */
