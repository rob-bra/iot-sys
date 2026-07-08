/**
 ******************************************************************************
 * @file    NBIOTV1_env_sensors_ex.c
 * @author  SRA
 * @brief   This file provides Extended BSP Environmental Sensors interface
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
#include "NBIOTV1_env_sensors_ex.h"

extern void *EnvCompObj[ENV_INSTANCES_NBR];

int32_t BSP_ENV_SENSOR_Enable_DRDY_Interrupt(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_SHT40 == 1)
    case SHT40:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if (LPS22DF_Enable_DRDY_Interrupt(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @param  Function Environmental sensor function. Could be:
 *         - ENV_PRESSURE or ENV_TEMPERATURE for instance LPS22DF
 *         - ENV_TEMPERATURE or ENV_HUMIDITY for instance SHT40
 * @param  Status the pointer to the status
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Get_DRDY_Status(uint32_t Instance, uint32_t Function, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if ((Function & ENV_PRESSURE) == ENV_PRESSURE)
      {
        if (LPS22DF_PRESS_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & ENV_TEMPERATURE) == ENV_TEMPERATURE)
      {
        if (LPS22DF_TEMP_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
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

#if (USE_ENV_SENSOR_SHT40 == 1)
    case SHT40:
      if ((Function & ENV_HUMIDITY) == ENV_HUMIDITY)
      {
        if (SHT40AD1B_HUM_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
        {
          ret = BSP_ERROR_COMPONENT_FAILURE;
        }
        else
        {
          ret = BSP_ERROR_NONE;
        }
      }
      else if ((Function & ENV_TEMPERATURE) == ENV_TEMPERATURE)
      {
        if (SHT40AD1B_TEMP_Get_DRDY_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
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
 * @brief  Get the register value (available only for LPS22HH, STTS751 sensors)
 * @param  Instance the device instance
 * @param  Reg address to be read
 * @param  Data pointer where the value is written to
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Read_Register(uint32_t Instance, uint8_t Reg, uint8_t *Data)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if (LPS22DF_Read_Reg(EnvCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_SHT40 == 1)
    case SHT40:
      ret = BSP_ERROR_COMPONENT_FAILURE;
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
int32_t BSP_ENV_SENSOR_Write_Register(uint32_t Instance, uint8_t Reg, uint8_t Data)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if (LPS22DF_Write_Reg(EnvCompObj[Instance], Reg, Data) != BSP_ERROR_NONE)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
      break;
#endif

#if (USE_ENV_SENSOR_SHT40 == 1)
    case SHT40:
      ret = BSP_ERROR_COMPONENT_FAILURE;
      break;
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  return ret;
}

/**
 * @brief  Set environmental sensor one shot mode
 * @param  Instance environmental sensor instance to be used
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Set_One_Shot(uint32_t Instance)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if(LPS22DF_Set_One_Shot(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Get environmental sensor one shot status
 * @param  Instance environmental sensor instance to be used
 * @param  Status pointer to the one shot status (1 means measurements available, 0 means measurements not available yet)
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Get_One_Shot_Status(uint32_t Instance, uint8_t *Status)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if(LPS22DF_Get_One_Shot_Status(EnvCompObj[Instance], Status) != BSP_ERROR_NONE)
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

int32_t BSP_ENV_SENSOR_Set_AVG(uint32_t Instance, uint8_t Avg)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if(LPS22DF_Set_AVG(EnvCompObj[Instance], Avg) != BSP_ERROR_NONE)
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

int32_t BSP_ENV_SENSOR_Set_LPF(uint32_t Instance, uint8_t Lpf)
{
  int32_t ret;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if(LPS22DF_Set_LPF(EnvCompObj[Instance], Lpf) != BSP_ERROR_NONE)
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
