/**
 ******************************************************************************
 * @file    NBIOTV1_motion_sensors_ex.h
 * @author  SRA
 * @brief   This file contains definitions for NBIOTV1_motion_sensors_ex.c
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
#ifndef NBIOTV1_MOTION_SENSORS_EX_H__
#define NBIOTV1_MOTION_SENSORS_EX_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "NBIOTV1_motion_sensors.h"

typedef enum
{
  MOTION_SENSOR_INT1_PIN = 0,
  MOTION_SENSOR_INT2_PIN
} MOTION_SENSOR_IntPin_t;

typedef struct
{
  unsigned int FreeFallStatus :1;
  unsigned int TapStatus :1;
  unsigned int DoubleTapStatus :1;
  unsigned int WakeUpStatus :1;
  unsigned int StepStatus :1;
  unsigned int TiltStatus :1;
  unsigned int D6DOrientationStatus :1;
  unsigned int SleepStatus :1;
} MOTION_SENSOR_Event_Status_t;

int32_t BSP_MOTION_SENSOR_Enable_DRDY_Interrupt(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Disable_DRDY_Interrupt(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Set_Interrupt_Latch(uint32_t Instance, uint8_t Status);
int32_t BSP_MOTION_SENSOR_ExitDeepPowerDownSPI(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Enable_Wake_Up_Detection(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin);
int32_t BSP_MOTION_SENSOR_Disable_Wake_Up_Detection(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Threshold(uint32_t Instance, uint8_t Threshold);
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Duration(uint32_t Instance, uint8_t Duration);
int32_t BSP_MOTION_SENSOR_Enable_6D_Orientation(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin);
int32_t BSP_MOTION_SENSOR_Disable_6D_Orientation(uint32_t Instance);
int32_t BSP_MOTION_SENSOR_Set_6D_Orientation_Threshold(uint32_t Instance, uint8_t Threshold);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XL(uint32_t Instance, uint8_t *xl);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XH(uint32_t Instance, uint8_t *xh);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YL(uint32_t Instance, uint8_t *yl);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YH(uint32_t Instance, uint8_t *yh);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZL(uint32_t Instance, uint8_t *zl);
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZH(uint32_t Instance, uint8_t *zh);
int32_t BSP_MOTION_SENSOR_Get_Event_Status(uint32_t Instance, MOTION_SENSOR_Event_Status_t *Status);
int32_t BSP_MOTION_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status);
int32_t BSP_MOTION_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data);
int32_t BSP_MOTION_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data);

#ifdef __cplusplus
}
#endif

#endif /* NBIOTV1_MOTION_SENSORS_EX_H__ */
