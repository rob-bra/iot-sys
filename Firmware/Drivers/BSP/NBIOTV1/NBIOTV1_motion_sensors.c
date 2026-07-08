/**
 ******************************************************************************
 * @file    NBIOTV1_motion_sensors.c
 * @author  SRA
 * @brief   This file provides BSP Motion Sensors interface
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
#include "NBIOTV1_motion_sensors.h"

EXTI_HandleTypeDef H_EXTI_INT1_LIS2DUXS12 = {.Line = LIS2DUXS12_INT1_EXTI_LINE};
EXTI_HandleTypeDef H_EXTI_INT2_LIS2DUXS12 = {.Line = LIS2DUXS12_INT2_EXTI_LINE};

extern void *MotionCompObj[MOTION_INSTANCES_NBR]; /* This "redundant" line is here to fulfil MISRA C-2012 rule 8.4 */
void *MotionCompObj[MOTION_INSTANCES_NBR];

/* We define a jump table in order to get the correct index from the desired function. */
/* This table should have a size equal to the maximum value of a function plus 1.      */
static uint32_t FunctionIndex[5] = { 0, 0, 1, 1, 2 };
static MOTION_SENSOR_FuncDrv_t *MotionFuncDrv[MOTION_INSTANCES_NBR][MOTION_FUNCTIONS_NBR];
static MOTION_SENSOR_CommonDrv_t *MotionDrv[MOTION_INSTANCES_NBR];
static MOTION_SENSOR_Ctx_t MotionCtx[MOTION_INSTANCES_NBR];

#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
static int32_t LIS2DUXS12_Probe(uint32_t Functions);
static int32_t BSP_LIS2DUXS12_Init(void);
static int32_t BSP_LIS2DUXS12_DeInit(void);
static int32_t BSP_LIS2DUXS12_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
static int32_t BSP_LIS2DUXS12_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len);
#endif

/**
 * @brief  Initializes the motion sensors
 * @param  Instance Motion sensor instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Init(uint32_t Instance, uint32_t Functions)
{
  int32_t ret = BSP_ERROR_NONE;
  uint32_t function = MOTION_ACCELERO;
  uint32_t i;
  uint32_t component_functions = 0;
  MOTION_SENSOR_Capabilities_t cap;

  switch(Instance)
  {
#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)
    case LIS2DUXS12:
      if(LIS2DUXS12_Probe(Functions) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_NO_INIT;
      }
      if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], (void*) &cap) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_UNKNOWN_COMPONENT;
      }
      if(cap.Acc == 1U)
      {
        component_functions |= MOTION_ACCELERO;
      }
#endif

    default:
      ret = BSP_ERROR_WRONG_PARAM;
      break;
  }

  if(ret != BSP_ERROR_NONE)
  {
    return ret;
  }

  for(i = 0; i < MOTION_FUNCTIONS_NBR; i++)
  {
    if(((Functions & function) == function) && ((component_functions & function) == function))
    {
      if(MotionFuncDrv[Instance][FunctionIndex[function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
      {
        return BSP_ERROR_COMPONENT_FAILURE;
      }
    }
    function = function << 1;
  }

  return ret;
}

/**
 * @brief  Deinitialize Motion sensor
 * @param  Instance Motion sensor instance
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_DeInit(uint32_t Instance)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MotionDrv[Instance]->DeInit(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Get motion sensor instance capabilities
 * @param  Instance Motion sensor instance
 * @param  Capabilities pointer to motion sensor capabilities
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetCapabilities(uint32_t Instance, MOTION_SENSOR_Capabilities_t *Capabilities)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MotionDrv[Instance]->GetCapabilities(MotionCompObj[Instance], Capabilities) != BSP_ERROR_NONE)
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
 * @param  Instance Motion sensor instance
 * @param  Id WHOAMI value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if(MotionDrv[Instance]->ReadID(MotionCompObj[Instance], Id) != BSP_ERROR_NONE)
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
 * @brief  Enable Motion sensor
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Enable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->Enable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Disable Motion sensor
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_Disable(uint32_t Instance, uint32_t Function)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->Disable(MotionCompObj[Instance]) != BSP_ERROR_NONE)
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
 * @brief  Get accelero axes data
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Axes pointer to axes data structure
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetAxes(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_Axes_t *Axes)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxes(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
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
 * @brief  Get accelero axes raw data
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Axes pointer to axes raw data structure
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetAxesRaw(uint32_t Instance, uint32_t Function, BSP_MOTION_SENSOR_AxesRaw_t *Axes)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetAxesRaw(MotionCompObj[Instance], Axes) != BSP_ERROR_NONE)
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
 * @brief  Get accelero sensitivity
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Sensitivity pointer to sensitivity read value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetSensitivity(uint32_t Instance, uint32_t Function, float_t *Sensitivity)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetSensitivity(MotionCompObj[Instance], Sensitivity) != BSP_ERROR_NONE)
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
 * @brief  Get accelero Output Data Rate
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Odr pointer to Output Data Rate read value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetOutputDataRate(uint32_t Instance, uint32_t Function, float_t *Odr)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetOutputDataRate(MotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
 * @brief  Get accelero Full Scale
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Fullscale pointer to Fullscale read value
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_GetFullScale(uint32_t Instance, uint32_t Function, int32_t *Fullscale)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->GetFullScale(MotionCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
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
 * @brief  Set accelero Output Data Rate
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Odr Output Data Rate value to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_SetOutputDataRate(uint32_t Instance, uint32_t Function, float_t Odr)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->SetOutputDataRate(MotionCompObj[Instance], Odr) != BSP_ERROR_NONE)
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
 * @brief  Set accelero Full Scale
 * @param  Instance Motion sensor instance
 * @param  Function Motion sensor function. Could be :
 *         - MOTION_GYRO
 *         - MOTION_ACCELERO
 *         - MOTION_MAGNETO
 * @param  Fullscale Fullscale value to be set
 * @retval BSP status
 */
int32_t BSP_MOTION_SENSOR_SetFullScale(uint32_t Instance, uint32_t Function, int32_t Fullscale)
{
  int32_t ret;

  if(Instance >= MOTION_INSTANCES_NBR)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if((MotionCtx[Instance].Functions & Function) == Function)
    {
      if(MotionFuncDrv[Instance][FunctionIndex[Function]]->SetFullScale(MotionCompObj[Instance], Fullscale) != BSP_ERROR_NONE)
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

#if (USE_MOTION_SENSOR_LIS2DUXS12 == 1)

/**
 * @brief  Register Bus IOs for instance 0 if component ID is OK
 * @retval BSP status
 */
static int32_t LIS2DUXS12_Probe(uint32_t Functions)
{
  LIS2DUXS12_IO_t io_ctx;
  uint8_t id;
  static LIS2DUXS12_Object_t lis2duxs12_obj;
  LIS2DUXS12_Capabilities_t cap;
  int32_t ret = BSP_ERROR_NONE;

  /* Configure the accelero driver */
  io_ctx.BusType = LIS2DUXS12_SPI_4WIRES_BUS; /* SPI 4-Wires */
  io_ctx.Address = 0x0;
  io_ctx.Init = BSP_LIS2DUXS12_Init;
  io_ctx.DeInit = BSP_LIS2DUXS12_DeInit;
  io_ctx.ReadReg = BSP_LIS2DUXS12_ReadReg;
  io_ctx.WriteReg = BSP_LIS2DUXS12_WriteReg;
  io_ctx.GetTick = BSP_GetTick;

  if(LIS2DUXS12_RegisterBusIO(&lis2duxs12_obj, &io_ctx) != LIS2DUXS12_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(LIS2DUXS12_ReadID(&lis2duxs12_obj, &id) != LIS2DUXS12_OK)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else if(id != LIS2DUXS12_ID)
  {
    ret = BSP_ERROR_UNKNOWN_COMPONENT;
  }
  else
  {
    (void) LIS2DUXS12_GetCapabilities(&lis2duxs12_obj, &cap);
    MotionCtx[LIS2DUXS12].Functions = ((uint32_t) cap.Gyro) | ((uint32_t) cap.Acc << 1) | ((uint32_t) cap.Magneto << 2);

    MotionCompObj[LIS2DUXS12] = &lis2duxs12_obj;
    /* The second cast (void *) is added to bypass Misra R11.3 rule */
    MotionDrv[LIS2DUXS12] = (MOTION_SENSOR_CommonDrv_t*) (void*) &LIS2DUXS12_COMMON_Driver;

    if(((Functions & MOTION_ACCELERO) == MOTION_ACCELERO) && (cap.Acc == 1U))
    {
      /* The second cast (void *) is added to bypass Misra R11.3 rule */
      MotionFuncDrv[LIS2DUXS12][FunctionIndex[MOTION_ACCELERO]] = (MOTION_SENSOR_FuncDrv_t*) (void*) &LIS2DUXS12_ACC_Driver;

      if(MotionDrv[LIS2DUXS12]->Init(MotionCompObj[LIS2DUXS12]) != LIS2DUXS12_OK)
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

static int32_t BSP_LIS2DUXS12_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  /* Configure LIS2DUXS12 INT1 pin */
  BSP_LIS2DUXS12_INT1_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_LIS2DUXS12_INT1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_LIS2DUXS12_INT1_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_LIS2DUXS12_INT1_EXTI_IRQn, BSP_LIS2DUXS12_INT1_EXTI_IRQ_PP, BSP_LIS2DUXS12_INT1_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_LIS2DUXS12_INT1_EXTI_IRQn);

  /* Configure LIS2DUXS12 INT2 pin */
  BSP_LIS2DUXS12_INT2_GPIO_CLK_ENABLE();
  GPIO_InitStruct.Pin = BSP_LIS2DUXS12_INT2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_LIS2DUXS12_INT2_PORT, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  /* Enable and set EXTI Interrupt priority */
  HAL_NVIC_SetPriority(BSP_LIS2DUXS12_INT2_EXTI_IRQn, BSP_LIS2DUXS12_INT2_EXTI_IRQ_PP, BSP_LIS2DUXS12_INT2_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_LIS2DUXS12_INT2_EXTI_IRQn);

  /* Configure IIS2ICLX CS pin */
  HAL_GPIO_WritePin(BSP_LIS2DUXS12_CS_PORT, BSP_LIS2DUXS12_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_LIS2DUXS12_CS_PIN;
  HAL_GPIO_Init(BSP_LIS2DUXS12_CS_PORT, &GPIO_InitStruct);

  if(BSP_LIS2DUXS12_SPI_INIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_LIS2DUXS12_DeInit(void)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(BSP_LIS2DUXS12_SPI_DEINIT() == BSP_ERROR_NONE)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

static int32_t BSP_LIS2DUXS12_WriteReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LIS2DUXS12_CS_PORT, BSP_LIS2DUXS12_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_LIS2DUXS12_SPI_SEND(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_LIS2DUXS12_SPI_SEND(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LIS2DUXS12_CS_PORT, BSP_LIS2DUXS12_CS_PIN, GPIO_PIN_SET);

  return ret;
}

static int32_t BSP_LIS2DUXS12_ReadReg(uint16_t Addr, uint16_t Reg, uint8_t *pdata, uint16_t len)
{
  UNUSED(Addr);
  int32_t ret = BSP_ERROR_NONE;
  uint8_t dataReg = (uint8_t) Reg;

  dataReg |= 0x80U;

  /* CS Enable */
  HAL_GPIO_WritePin(BSP_LIS2DUXS12_CS_PORT, BSP_LIS2DUXS12_CS_PIN, GPIO_PIN_RESET);

  ret = BSP_LIS2DUXS12_SPI_SEND(&dataReg, 1);

  if(ret == BSP_ERROR_NONE)
  {
    ret = BSP_LIS2DUXS12_SPI_RECV(pdata, len);
  }

  /* CS Disable */
  HAL_GPIO_WritePin(BSP_LIS2DUXS12_CS_PORT, BSP_LIS2DUXS12_CS_PIN, GPIO_PIN_SET);

  return ret;
}
#endif
