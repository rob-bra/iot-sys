/**
 ******************************************************************************
 * @file    NBIOTV1_motion_sensors_ex.c
 * @author  SRA
 * @brief   This file provides Extended BSP Motion Sensors interface
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

/* Includes ------------------------------------------------------------------*/
#include "NBIOTV1_motion_sensors_ex.h"

extern void *MotionCompObj[MOTION_INSTANCES_NBR];

/**
 * @brief  Enable data-ready interrupt on DRDY pin
 * @param  Instance the device instance
 * @param  Status data-ready interrupt on DRDY pin
 * @retval BSP status   
 */
int32_t BSP_MOTION_SENSOR_Enable_DRDY_Interrupt(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Enable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable data-ready interrupt on DRDY pin
 * @param  Instance the device instance
 * @param  Status data-ready interrupt on DRDY pin
 * @retval BSP status   
 */
int32_t BSP_MOTION_SENSOR_Disable_DRDY_Interrupt(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Disable_DRDY_Interrupt(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the Data-ready mode, latched or pulsed
 * @param  Instance the device instance
 * @param  mode the DRDY mode
 * @retval BSP status   
 */
int32_t BSP_MOTION_SENSOR_Set_Interrupt_Latch(uint32_t Instance, uint8_t Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_Set_Interrupt_Latch(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif      

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Exits the deep power-down mode
 * @param  Instance the device instance
 * @param  none
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_ExitDeepPowerDownSPI(uint32_t Instance)
{
  int32_t ret;

    switch(Instance)
    {
  #if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
      case LIS2DUXS12:
        if(LIS2DUXS12_ExitDeepPowerDownSPI(MotionCompObj[Instance]) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
        break;
  #endif

      default:
        ret = BSP_ERROR_WRONG_PARAM;
        break;
    }

    return ret;
}

/**
 * @brief  Enable the wake up detection
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_Wake_Up_Detection(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin)
{
  UNUSED(IntPin);
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Enable_Wake_Up_Detection(MotionCompObj[Instance], (LIS2DUXS12_SensorIntPin_t) IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable the wake up detection
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_Wake_Up_Detection(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Disable_Wake_Up_Detection(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the wake up detection threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Set_Wake_Up_Threshold(MotionCompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the wake up detection duration
 * @param  Instance the device instance
 * @param  Duration the duration to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_Wake_Up_Duration(uint32_t Instance, uint8_t Duration)
{
  int32_t ret;

  switch(Instance)
  {

#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Set_Wake_Up_Duration(MotionCompObj[Instance], Duration) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Enable 6D Orientation
 * @param  Instance the device instance
 * @param  IntPin the interrupt pin to be used
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable_6D_Orientation(uint32_t Instance, MOTION_SENSOR_IntPin_t IntPin)
{
  UNUSED(IntPin);
  int32_t ret;

  switch(Instance)
  {

#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Enable_6D_Orientation(MotionCompObj[Instance], (LIS2DUXS12_SensorIntPin_t) IntPin) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Disable 6D Orientation
 * @param  Instance the device instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable_6D_Orientation(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Disable_6D_Orientation(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the 6D orientation threshold
 * @param  Instance the device instance
 * @param  Threshold the threshold to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Set_6D_Orientation_Threshold(uint32_t Instance, uint8_t Threshold)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Set_6D_Orientation_Threshold(MotionCompObj[Instance], Threshold) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation XL
 * @param  Instance the device instance
 * @param  xl the pointer to the 6D orientation XL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XL(uint32_t Instance, uint8_t *xl)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_6D_Orientation_XL(MotionCompObj[Instance], xl) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation XH
 * @param  Instance the device instance
 * @param  xh the pointer to the 6D orientation XH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_XH(uint32_t Instance, uint8_t *xh)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_6D_Orientation_XH(MotionCompObj[Instance], xh) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation YL
 * @param  Instance the device instance
 * @param  yl the pointer to the 6D orientation YL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YL(uint32_t Instance, uint8_t *yl)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_6D_Orientation_YL(MotionCompObj[Instance], yl) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation YH
 * @param  Instance the device instance
 * @param  yh the pointer to the 6D orientation YH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_YH(uint32_t Instance, uint8_t *yh)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_6D_Orientation_YH(MotionCompObj[Instance], yh) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation ZL
 * @param  Instance the device instance
 * @param  zl the pointer to the 6D orientation ZL axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZL(uint32_t Instance, uint8_t *zl)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_6D_Orientation_ZL(MotionCompObj[Instance], zl) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get 6D Orientation ZH
 * @param  Instance the device instance
 * @param  zh the pointer to the 6D orientation ZH axis
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_6D_Orientation_ZH(uint32_t Instance, uint8_t *zh)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_6D_Orientation_ZH(MotionCompObj[Instance], zh) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the status of all hardware events
 * @param  Instance the device instance
 * @param  Status the pointer to the status of all hardware events
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_Event_Status(uint32_t Instance, MOTION_SENSOR_Event_Status_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_ACC_Get_Event_Status(MotionCompObj[Instance], (LIS2DUXS12_Event_Status_t*) (void*) Status) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the status of data ready bit
 * @param  Instance the device instance
 * @param  Function Motion sensor function. Could be:
 *         - MOTION_ACCELERO
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if((Function & MOTION_ACCELERO) == MOTION_ACCELERO)
      {
        if(LIS2DUXS12_ACC_Get_DRDY_Status(MotionCompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Get the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data pointer where the value is written to
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_Read_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set the register value
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data value to be written
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_Write_Reg(MotionCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}
