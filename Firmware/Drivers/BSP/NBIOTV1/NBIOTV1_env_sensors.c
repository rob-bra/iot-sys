/**
 ******************************************************************************
 * @file    NBIOTV1_env_sensors.c
 * @author  SRA
 * @brief   This file provides BSP Environmental Sensors interface
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
#include "NBIOTV1_env_sensors.h"

EXTI_HandleTypeDef H_EXTI_INT_LPS22DF = {.Line = LPS22DF_INT_EXTI_LINE};

extern void *EnvCompObj[ENV_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *EnvCompObj[ENV_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] ={0, 0, 1, 1, 2};
static ENV_SENSOR_FuncDrv_t *EnvFuncDrv[ENV_INSTANCES_NBR][ENV_FUNCTIONS_NBR];
static ENV_SENSOR_CommonDrv_t *EnvDrv[ENV_INSTANCES_NBR];
static ENV_SENSOR_Ctx_t EnvCtx[ENV_INSTANCES_NBR];

#if (USE_ENV_SENSOR_LPS22DF == 1)
static int32_t LPS22DF_Probe(uint32_t Functions);
static int32_t BSP_LPS22DF_Init(void);
static int32_t BSP_LPS22DF_DeInit(void);
static int32_t BSP_LPS22DF_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_LPS22DF_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

#if (USE_ENV_SENSOR_SHT40 == 1)
static int32_t SHT40_Probe(uint32_t Functions);
static int32_t BSP_SHT40_Init(void);
static int32_t BSP_SHT40_DeInit(void);
#endif

/* Private function prototypes -----------------------------------------------*/
static void BSP_LPS22DF_GPIO_EXTI_Callback(void);

/**
 * @brief  Initializes the env sensors
 * @param  Instance Environmental sensor instance
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = ENV_TEMPERATURE;
  uint32_t i;
  uint32_t component_functions = 0;
  ENV_SENSOR_Capabilities_t cap;

  switch(Instance)
  {
#if (USE_ENV_SENSOR_LPS22DF == 1)
    case LPS22DF:
      if(LPS22DF_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if(cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      if(cap.Humidity == 1U)
      {
        component_functions |= ENV_HUMIDITY;
      }
      break;
#endif
#if (USE_ENV_SENSOR_SHT40 == 1)
    case SHT40:
      if(SHT40_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Temperature == 1U)
      {
        component_functions |= ENV_TEMPERATURE;
      }
      if(cap.Pressure == 1U)
      {
        component_functions |= ENV_PRESSURE;
      }
      if(cap.Humidity == 1U)
      {
        component_functions |= ENV_HUMIDITY;
      }
      break;
#endif      
    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if(ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for(i = 0; i < ENV_FUNCTIONS_NBR; i++)
  {
    if(((Functions & function) == function) && ((component_functions & function) == function))
    {
      if(EnvFuncDrv[Instance][FunctionIndex[function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

/**
 * @brief  Deinitialize environmental sensor sensor
 * @param  Instance environmental sensor instance to be used
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv[Instance]->DeInit(EnvCompObj[Instance]) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Get environmental sensor instance capabilities
 * @param  Instance Environmental sensor instance
 * @param  Capabilities pointer to Environmental sensor capabilities
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_GetCapabilities(uint32_t Instance, ENV_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv[Instance]->GetCapabilities(EnvCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Get WHOAMI value
 * @param  Instance environmental sensor instance to be used
 * @param  Id WHOAMI value
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(EnvDrv[Instance]->ReadID(EnvCompObj[Instance], Id) != BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Enable environmental sensor sensor
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->Enable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Disable environmental sensor sensor
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->Disable(EnvCompObj[Instance]) != BSP_ERROR_NONE)
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
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get environmental sensor Output Data Rate
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @param  Odr pointer to Output Data Rate read value
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Set environmental sensor Output Data Rate
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @param  Odr Output Data Rate value to be set
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(EnvCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

/**
 * @brief  Get environmental sensor value
 * @param  Instance environmental sensor instance to be used
 * @param  Function Environmental sensor function. Could be :
 *         - ENV_TEMPERATURE
 *         - ENV_PRESSURE
 * @param  Value pointer to environmental sensor value
 * @retval BSP status
 */
int32_t BSP_ENV_SENSOR_GetValue(uint32_t Instance, uint32_t Function, float_t *Value)
{
  int32_t ret;

  if(Instance >= ENV_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((EnvCtx[Instance].Functions & Function) == Function)
    {
      if(EnvFuncDrv[Instance][FunctionIndex[Function]]->GetValue(EnvCompObj[Instance], Value) != BSP_ERROR_NONE)
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
      ret = BSP_ERROR_WRONG_PARAM;
    }
  }

  return ret;
}

#if (USE_ENV_SENSOR_LPS22DF == 1)

/**
 * @brief  Register Bus IOs for instance 0 if LPS22DF ID is OK
 * @param  Functions Environmental sensor functions. Could be :
 *         - ENV_PRESSURE and/or ENV_TEMPERATURE
 * @retval BSP status
 */
static int32_t LPS22DF_Probe(uint32_t Functions)
{
  LPS22DF_IO_t io_ctx;
  uint8_t id;
  int32_t ret = BSP_ERROR_NONE;
  static LPS22DF_Object_t lps22df_obj;
  LPS22DF_Capabilities_t cap;

  /* Configure the environmental sensor driver */
  io_ctx.BusType = LPS22DF_SPI_4WIRES_BUS; /* SPI */
  io_ctx.Address = 0; // not used
  io_ctx.Init = BSP_LPS22DF_Init;
  io_ctx.DeInit = BSP_LPS22DF_DeInit;
  io_ctx.ReadReg = BSP_LPS22DF_ReadReg;
  io_ctx.WriteReg = BSP_LPS22DF_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(LPS22DF_RegisterBusIO(&lps22df_obj, &io_ctx) != LPS22DF_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(LPS22DF_ReadID(&lps22df_obj, &id) != LPS22DF_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != LPS22DF_ID) //0xB4
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) LPS22DF_GetCapabilities(&lps22df_obj, &cap);
    EnvCtx[LPS22DF].Functions = ((uint32_t) cap.Temperature) | ((uint32_t) cap.Pressure << 1) | ((uint32_t) cap.Humidity << 2);

    EnvCompObj[LPS22DF] = &lps22df_obj;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[LPS22DF] = (ENV_SENSOR_CommonDrv_t*) (void*) &LPS22DF_COMMON_Driver;

    if(((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[LPS22DF][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t*) (void*) &LPS22DF_TEMP_Driver;

      if(EnvDrv[LPS22DF]->Init(EnvCompObj[LPS22DF]) != LPS22DF_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & ENV_PRESSURE) == ENV_PRESSURE) && (cap.Pressure == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[LPS22DF][FunctionIndex[ENV_PRESSURE]] = (ENV_SENSOR_FuncDrv_t*) (void*) &LPS22DF_PRESS_Driver;

      if(EnvDrv[LPS22DF]->Init(EnvCompObj[LPS22DF]) != LPS22DF_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }

  return ret;
}

static int32_t BSP_LPS22DF_Init(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  BSP_LPS22DF_INT_GPIO_CLK_ENABLE();

  /*Configure GPIO pin : PC11 */
  GPIO_InitStruct.Pin = BSP_LPS22DF_INT_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_LPS22DF_INT_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_LPS22DF_INT_EXTI_IRQn, BSP_LPS22DF_INT_EXTI_IRQ_PP, BSP_LPS22DF_INT_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_LPS22DF_INT_EXTI_IRQn);

  if (HAL_EXTI_RegisterCallback(&H_EXTI_INT_LPS22DF,  HAL_EXTI_COMMON_CB_ID, BSP_LPS22DF_GPIO_EXTI_Callback) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }

  /* Configure LPS22DF CS pin */
  HAL_GPIO_WritePin(BSP_LPS22DF_CS_PORT, BSP_LPS22DF_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_LPS22DF_CS_PIN;
  HAL_GPIO_Init(BSP_LPS22DF_CS_PORT, &GPIO_InitStruct);

  if(BSP_LPS22DF_SPI_INIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_LPS22DF_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_LPS22DF_SPI_DEINIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_LPS22DF_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LPS22DF_CS_PORT, BSP_LPS22DF_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_LPS22DF_SPI_SEND(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_LPS22DF_SPI_SEND(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LPS22DF_CS_PORT, BSP_LPS22DF_CS_PIN, GPIO_PIN_SET);

  return ret;
}

static int32_t BSP_LPS22DF_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  dataReg |= 0x80U;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LPS22DF_CS_PORT, BSP_LPS22DF_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_LPS22DF_SPI_SEND(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_LPS22DF_SPI_RECV(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LPS22DF_CS_PORT, BSP_LPS22DF_CS_PIN, GPIO_PIN_SET);

  return ret;
}

/**
  * @brief  User EXTI line detection callbacks.
  * @retval None
  */
static void BSP_LPS22DF_GPIO_EXTI_Callback(void)
{
  BSP_LPS22DF_GPIO_Callback();
}

/**
  * @brief  BSP LPS22DF GPIO callback
  * @param  Node
  * @retval None.
  */
__weak void BSP_LPS22DF_GPIO_Callback(void)
{
  /* This function should be implemented by the user application.
     It is called into this driver when an event is triggered. */
}

#endif

#if (USE_ENV_SENSOR_SHT40 == 1)
/**
 * @brief  Register Bus IOs for instance 2 if component ID is OK
 * @param  Functions Environmental sensor functions. Could be :
 *         - ENV_TEMPERATURE and ENV_HUMIDITY
 * @retval BSP status
 */
static int32_t SHT40_Probe(uint32_t Functions)
{
  SHT40AD1B_IO_t io_ctx;
  uint8_t id;
  int32_t ret = BSP_ERROR_NONE;
  static SHT40AD1B_Object_t sht40_obj_0;
  SHT40AD1B_Capabilities_t cap;

  /* Configure the pressure driver */
  io_ctx.BusType = SHT40AD1B_I2C_BUS; /* I2C */

  io_ctx.Address = BSP_SHT40_I2C_DEV_ADDR;
  io_ctx.Init = BSP_SHT40_Init;
  io_ctx.DeInit = BSP_SHT40_DeInit;
  io_ctx.Read = BSP_SHT40_I2C_READ;
  io_ctx.Write = BSP_SHT40_I2C_WRITE;
  io_ctx.GetTick = BSP_GetTick;

  if(SHT40AD1B_RegisterBusIO(&sht40_obj_0, &io_ctx) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(SHT40AD1B_ReadID(&sht40_obj_0, &id) != SHT40AD1B_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != SHT40AD1B_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) SHT40AD1B_GetCapabilities(&sht40_obj_0, &cap);

    EnvCtx[SHT40].Functions = ((uint32_t) cap.Temperature) | ((uint32_t) cap.Pressure << 1) | ((uint32_t) cap.Humidity << 2);

    EnvCompObj[SHT40] = &sht40_obj_0;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    EnvDrv[SHT40] = (ENV_SENSOR_CommonDrv_t*) (void*) &SHT40AD1B_COMMON_Driver;

    if((ret == BSP_ERROR_NONE) && ((Functions & ENV_TEMPERATURE) == ENV_TEMPERATURE) && (cap.Temperature == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[SHT40][FunctionIndex[ENV_TEMPERATURE]] = (ENV_SENSOR_FuncDrv_t*) (void*) &SHT40AD1B_TEMP_Driver;

      if(EnvDrv[SHT40]->Init(EnvCompObj[SHT40]) != SHT40AD1B_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
    if(((Functions & ENV_HUMIDITY) == ENV_HUMIDITY) && (cap.Humidity == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      EnvFuncDrv[SHT40][FunctionIndex[ENV_HUMIDITY]] = (ENV_SENSOR_FuncDrv_t*) (void*) &SHT40AD1B_HUM_Driver;

      if(EnvDrv[SHT40]->Init(EnvCompObj[SHT40]) != SHT40AD1B_OK)
      {
        ret = BSP_ERROR_COMPONENT_FAILURE;
      }
      else
      {
        ret = BSP_ERROR_NONE;
      }
    }
  }
  return ret;
}

static int32_t BSP_SHT40_Init(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_SHT40_I2C_INIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_SHT40_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_SHT40_I2C_DEINIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

#endif
