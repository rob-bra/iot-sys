/**
 ******************************************************************************
 * @file    NBIOTV1_env_sensors_ex.h
 * @author  SRA
 * @brief   This file contains definitions for STWIN.box_env_sensors_ex.c
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
#ifndef NBIOTV1_ENV_SENSOR_EX_H
#define NBIOTV1_ENV_SENSOR_EX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "NBIOTV1_env_sensors.h"

int32_t BSP_ENV_SENSOR_Enable_DRDY_Interrupt(uint32_t Instance);
int32_t BSP_ENV_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status);
int32_t BSP_ENV_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data);
int32_t BSP_ENV_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data);
int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance);
int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status);
int32_t BSP_ENV_SENSOR_Set_AVG(uint32_t Instance, uint8_t Avg);
int32_t BSP_ENV_SENSOR_Set_LPF(uint32_t Instance, uint8_t Lpf);

#ifdef __cplusplus
}
#endif

#endif /* NBIOTV1_ENV_SENSOR_EX_H */
