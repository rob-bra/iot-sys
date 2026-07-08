/**
 ******************************************************************************
 * @file    NBIOTV1_bus.c
 * @author  SRA
 * @brief   This file provides BSP BUS IO driver
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

#include "NBIOTV1_bus.h"
#include "NBIOTV1.h"
#include "NBIOTV1_errno.h"

#define TIMEOUT_DURATION 1000

__weak HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hi2c);
__weak HAL_StatusTypeDef MX_I2C2_Init(I2C_HandleTypeDef *hi2c);
__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hi2c);
__weak HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef *hspi);
__weak HAL_StatusTypeDef MX_SPI2_Init(SPI_HandleTypeDef *hspi);
__weak HAL_StatusTypeDef MX_SPI3_Init(SPI_HandleTypeDef *hspi);
__weak HAL_StatusTypeDef MX_USART1_UART_Init(UART_HandleTypeDef *huart);
__weak HAL_StatusTypeDef MX_LPUART1_UART_Init(UART_HandleTypeDef *huart);

static I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef hi2c3;
static SPI_HandleTypeDef hbusspi1;
static SPI_HandleTypeDef hbusspi2;
static UART_HandleTypeDef huart1;
static UART_HandleTypeDef hlpuart1;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
static uint32_t IsI2C1MspCbValid = 0;
static uint32_t IsI2C3MspCbValid = 0;
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */				
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)						
static uint32_t IsSPI1MspCbValid = 0;
static uint32_t IsSPI2MspCbValid = 0;
#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */			

static void I2C1_MspInit(I2C_HandleTypeDef *i2cHandle);
static void I2C1_MspDeInit(I2C_HandleTypeDef *i2cHandle);
static void I2C3_MspInit(I2C_HandleTypeDef *i2cHandle);
static void I2C3_MspDeInit(I2C_HandleTypeDef *i2cHandle);
static void SPI1_MspInit(SPI_HandleTypeDef *spiHandle);
static void SPI1_MspDeInit(SPI_HandleTypeDef *spiHandle);
static void SPI2_MspInit(SPI_HandleTypeDef *spiHandle);
static void SPI2_MspDeInit(SPI_HandleTypeDef *spiHandle);
static void USART1_MspInit(UART_HandleTypeDef *huart);
static void USART1_MspDeInit(UART_HandleTypeDef *huart);
static void LPUART1_MspInit(UART_HandleTypeDef *huart);
static void LPUART1_MspDeInit(UART_HandleTypeDef *huart);

/* BUS IO driver over I2C Peripheral */
/*******************************************************************************
 BUS OPERATIONS OVER I2C
 *******************************************************************************/
/**
 * @brief Initialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hi2c1.Instance = I2C1;

  if(HAL_I2C_GetState(&hi2c1) == HAL_I2C_STATE_RESET)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    /* Init the I2C Msp */
    I2C1_MspInit(&hi2c1);
#else
      if(IsI2C1MspCbValid == 0U)
      {
        if(BSP_I2C1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
    #endif

    /* Init the I2C */
    if(MX_I2C1_Init(&hi2c1) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C1_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
  /* DeInit the I2C */
  I2C1_MspDeInit(&hi2c1);
#endif

  if(HAL_I2C_DeInit(&hi2c1) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief Return the status of the Bus
 *	@retval bool
 */
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_I2C_IsDeviceReady(&hi2c1, DevAddr, Trials, BUS_I2C1_POLL_TIMEOUT) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (8 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c1, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (8 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c1, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_8BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (16 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c1, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (16 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c1, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_16BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send an amount width data through bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C1_Send(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Transmit(&hi2c1, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Receive an amount of data through a bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C1_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Receive(&hi2c1, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send and receive an amount of data through bus (Full duplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pTxdata: Transmit data pointer
 * @param 	pRxdata: Receive data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C1_SendRecv(uint16_t DevAddr, uint8_t *pTxdata, uint8_t *pRxdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  /*
   * Send and receive an amount of data through bus (Full duplex)
   * I2C is Half-Duplex protocol
   */
  if(BSP_I2C1_Send(DevAddr, pTxdata, len) == (int32_t) len)
  {
    if(BSP_I2C1_Recv(DevAddr, pRxdata, len) == (int32_t) len)
    {
      ret = (int32_t) len;
    }
  }

  return ret;
}

/**
 * @brief  Initialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C3_Init(void)
{

  int32_t ret = BSP_ERROR_NONE;

  hi2c3.Instance = I2C3;

  if(HAL_I2C_GetState(&hi2c3) == HAL_I2C_STATE_RESET)
  {
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
    /* Init the I2C Msp */
    I2C3_MspInit(&hi2c3);
#else
      if(IsI2C3MspCbValid == 0U)
      {
        if(BSP_I2C3_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
        {
          return BSP_ERROR_MSP_FAILURE;
        }
      }
    #endif

    /* Init the I2C */
    if(MX_I2C3_Init(&hi2c3) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else if(HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
    else
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitialize a bus
 * @param None
 * @retval BSP status
 */
int32_t BSP_I2C3_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 0)
  /* DeInit the I2C */
  I2C3_MspDeInit(&hi2c3);
#endif

  if(HAL_I2C_DeInit(&hi2c3) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief Return the status of the Bus
 *	@retval bool
 */
int32_t BSP_I2C3_IsReady(uint16_t DevAddr, uint32_t Trials)
{
  int32_t ret = BSP_ERROR_NONE;

  if(HAL_I2C_IsDeviceReady(&hi2c3, DevAddr, Trials, BUS_I2C1_POLL_TIMEOUT) != HAL_OK)
  {
    ret = BSP_ERROR_BUSY;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (8 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C3_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c3, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_8BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (8 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C3_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c3, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_8BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write registers through bus (16 bits)
 * @param  Addr: Device address on Bus.
 * @param  Reg: The target register address to write
 * @param  Value: The target register value to be written
 * @retval BSP status
 */
int32_t BSP_I2C3_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Write(&hi2c3, (uint8_t) DevAddr, (uint16_t) Reg, I2C_MEMADD_SIZE_16BIT, (uint8_t*) pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Read registers through a bus (16 bits)
 * @param  DevAddr: Device address on BUS
 * @param  Reg: The target register address to read
 * @retval BSP status
 */
int32_t BSP_I2C3_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Mem_Read(&hi2c3, DevAddr, (uint16_t) Reg,
  I2C_MEMADD_SIZE_16BIT,
                      pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send an amount width data through bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C3_Send(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Transmit(&hi2c3, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Receive an amount of data through a bus (Simplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pData: Data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C3_Recv(uint16_t DevAddr, uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  if(HAL_I2C_Master_Receive(&hi2c3, DevAddr, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Send and receive an amount of data through bus (Full duplex)
 * @param  DevAddr: Device address on Bus.
 * @param  pTxdata: Transmit data pointer
 * @param 	pRxdata: Receive data pointer
 * @param  Length: Data length
 * @retval BSP status
 */
int32_t BSP_I2C3_SendRecv(uint16_t DevAddr, uint8_t *pTxdata, uint8_t *pRxdata, uint16_t len)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

  /*
   * Send and receive an amount of data through bus (Full duplex)
   * I2C is Half-Duplex protocol
   */
  if(BSP_I2C3_Send(DevAddr, pTxdata, len) == (int32_t) len)
  {
    if(BSP_I2C3_Recv(DevAddr, pRxdata, len) == (int32_t) len)
    {
      ret = BSP_ERROR_NONE;
    }
  }

  return ret;
}

/* BUS IO driver over SPI Peripheral */
/*******************************************************************************
 BUS OPERATIONS OVER SPI
 *******************************************************************************/
/**
 * @brief  Initializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI1_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbusspi1.Instance = SPI1;
  if(HAL_SPI_GetState(&hbusspi1) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI1_MspInit(&hbusspi1);
#else
    if(IsSPI1MspCbValid == 0U)
    {
      if(BSP_SPI1_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif   

    /* Init the SPI */
    if(MX_SPI1_Init(&hbusspi1) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  return ret;
}

/**
 * @brief  DeInitializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI1_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI1_MspDeInit(&hbusspi1);
#endif  

  if(HAL_SPI_DeInit(&hbusspi1) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write Data through SPI BUS.
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI1_Send(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Transmit(&hbusspi1, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Receive Data from SPI BUS
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI1_Recv(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Receive(&hbusspi1, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Send and Receive data to/from SPI BUS (Full duplex)
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbusspi1, pTxData, pRxData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Initializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI2_Init(void)
{
  int32_t ret = BSP_ERROR_NONE;

  hbusspi2.Instance = SPI2;
  if(HAL_SPI_GetState(&hbusspi2) == HAL_SPI_STATE_RESET)
  {
#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
    /* Init the SPI Msp */
    SPI2_MspInit(&hbusspi2);
#else
    if(IsSPI2MspCbValid == 0U)
    {
      if(BSP_SPI2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
      {
        return BSP_ERROR_MSP_FAILURE;
      }
    }
#endif   

    /* Init the SPI */
    if(MX_SPI2_Init(&hbusspi2) != HAL_OK)
    {
      ret = BSP_ERROR_BUS_FAILURE;
    }
  }

  __HAL_SPI_ENABLE(&hbusspi2);

  return ret;
}

/**
 * @brief  DeInitializes SPI HAL.
 * @retval None
 * @retval BSP status
 */
int32_t BSP_SPI2_DeInit(void)
{
  int32_t ret = BSP_ERROR_BUS_FAILURE;

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 0)
  SPI2_MspDeInit(&hbusspi2);
#endif  

  if(HAL_SPI_DeInit(&hbusspi2) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

/**
 * @brief  Write Data through SPI BUS.
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI2_Send(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Transmit(&hbusspi2, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Receive Data from SPI BUS
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI2_Recv(uint8_t *pData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_Receive(&hbusspi2, pData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

/**
 * @brief  Send and Receive data to/from SPI BUS (Full duplex)
 * @param  pData: Data
 * @param  len: Length of data in byte
 * @retval BSP status
 */
int32_t BSP_SPI2_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t len)
{
  int32_t ret = BSP_ERROR_UNKNOWN_FAILURE;

  if(HAL_SPI_TransmitReceive(&hbusspi2, pTxData, pRxData, len, TIMEOUT_DURATION) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }
  return ret;
}

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)  
/**
  * @brief Register Default BSP I2C1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C1_RegisterDefaultMspCallbacks (void)
{

  __HAL_I2C_RESET_HANDLE_STATE(&hi2c1);
  
  /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPINIT_CB_ID, I2C1_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPDEINIT_CB_ID, I2C1_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsI2C1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP I2C1 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C1_RegisterMspCallbacks (BSP_I2C_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_I2C_RESET_HANDLE_STATE(&hi2c1);  
 
   /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPINIT_CB_ID, Callbacks->pMspI2cInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c1, HAL_I2C_MSPDEINIT_CB_ID, Callbacks->pMspI2cDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsI2C1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief Register Default BSP I2C3 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_I2C3_RegisterDefaultMspCallbacks (void)
{

  __HAL_I2C_RESET_HANDLE_STATE(&hi2c3);
  
  /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPINIT_CB_ID, I2C3_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPDEINIT_CB_ID, I2C3_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsI2C3MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP I2C2 Bus Msp Callback registering
  * @param Callbacks     pointer to I2C2 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_I2C3_RegisterMspCallbacks (BSP_I2C_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_I2C_RESET_HANDLE_STATE(&hi2c3);  
 
   /* Register MspInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPINIT_CB_ID, Callbacks->pMspI2cInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_I2C_RegisterCallback(&hi2c3, HAL_I2C_MSPDEINIT_CB_ID, Callbacks->pMspI2cDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsI2C3MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */

#if (USE_HAL_SPI_REGISTER_CALLBACKS == 1)  
/**
  * @brief Register Default BSP SPI1 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterDefaultMspCallbacks (void)
{

  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi1);
  
  /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPINIT_CB_ID, SPI1_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPDEINIT_CB_ID, SPI1_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsSPI1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP SPI1 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI1_RegisterMspCallbacks (BSP_SPI_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi1);  
 
   /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi1, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsSPI1MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief Register Default BSP SPI2 Bus Msp Callbacks
  * @retval BSP status
  */
int32_t BSP_SPI2_RegisterDefaultMspCallbacks (void)
{

  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi2);
  
  /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPINIT_CB_ID, SPI2_MspInit)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPDEINIT_CB_ID, SPI2_MspDeInit) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  IsSPI2MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

/**
  * @brief BSP SPI1 Bus Msp Callback registering
  * @param Callbacks     pointer to SPI1 MspInit/MspDeInit callback functions
  * @retval BSP status
  */
int32_t BSP_SPI2_RegisterMspCallbacks (BSP_SPI_Cb_t *Callbacks)
{
  /* Prevent unused argument(s) compilation warning */
  __HAL_SPI_RESET_HANDLE_STATE(&hbusspi2);  
 
   /* Register MspInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPINIT_CB_ID, Callbacks->pMspSpiInitCb)  != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  /* Register MspDeInit Callback */
  if (HAL_SPI_RegisterCallback(&hbusspi2, HAL_SPI_MSPDEINIT_CB_ID, Callbacks->pMspSpiDeInitCb) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }
  
  IsSPI2MspCbValid = 1;
  
  return BSP_ERROR_NONE;  
}

#endif /* USE_HAL_SPI_REGISTER_CALLBACKS */

/**
 * @brief  Return system tick in ms
 * @retval Current HAL time base time stamp
 */
int32_t BSP_GetTick(void)
{
  return (int32_t) HAL_GetTick();
}

/* SPI1 init function */

__weak HAL_StatusTypeDef MX_SPI1_Init(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef ret = HAL_OK;

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct =
  {
      0 };

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi->Instance = SPI1;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 0x7;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi->Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi->Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if(HAL_SPI_Init(hspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if(HAL_SPIEx_SetConfigAutonomousMode(hspi, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void SPI1_MspInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable Peripheral clock */
  __HAL_RCC_SPI1_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
 
  /**SPI1 GPIO Configuration    
   PA1     ------> SPI1_SCK
   PA7     ------> SPI1_MOSI
   PA6     ------> SPI1_MISO
   */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void SPI1_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);

  /* Peripheral clock disable */
  __HAL_RCC_SPI1_CLK_DISABLE();
  __HAL_RCC_GPIOA_CLK_DISABLE();

  /**SPI1 GPIO Configuration    
   PA1     ------> SPI1_SCK
   PA7     ------> SPI1_MOSI
   PA6     ------> SPI1_MISO
   */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1 | GPIO_PIN_6 | GPIO_PIN_7);
}

/* SPI2 init function */
__weak HAL_StatusTypeDef MX_SPI2_Init(SPI_HandleTypeDef *hspi)
{
  HAL_StatusTypeDef ret = HAL_OK;

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct =
  {
      0 };

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi->Instance = SPI2;
  hspi->Init.Mode = SPI_MODE_MASTER;
  hspi->Init.Direction = SPI_DIRECTION_2LINES;
  hspi->Init.DataSize = SPI_DATASIZE_8BIT;
  hspi->Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi->Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi->Init.NSS = SPI_NSS_SOFT;
  hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi->Init.TIMode = SPI_TIMODE_DISABLE;
  hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi->Init.CRCPolynomial = 0x7;
  hspi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi->Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi->Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if(HAL_SPI_Init(hspi) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if(HAL_SPIEx_SetConfigAutonomousMode(hspi, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void SPI2_MspInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable Peripheral clock */
  __HAL_RCC_SPI2_CLK_ENABLE();

  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  /**SPI2 GPIO Configuration
   PB13     ------> SPI2_SCK
   PC2      ------> SPI2_MISO
   PC3      ------> SPI2_MOSI
   */

  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void SPI2_MspDeInit(SPI_HandleTypeDef *spiHandle)
{
  UNUSED(spiHandle);

  /* Peripheral clock disable */
  __HAL_RCC_SPI2_CLK_DISABLE();

  /**SPI2 GPIO Configuration
   PB13     ------> SPI2_SCK
   PC2      ------> SPI2_MISO
   PC3      ------> SPI2_MOSI
   */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_2 | GPIO_PIN_3);
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
}

/* I2C2 init function */

__weak HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hi2c)
{
  HAL_StatusTypeDef ret = HAL_OK;
  hi2c->Instance = I2C1;
  hi2c->Init.Timing = 0x00F07BFF;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if(HAL_I2C_Init(hi2c) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void I2C1_MspInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**I2C1 GPIO Configuration
   PB7     ------> I2C1_SCL
   PB6     ------> I2C1_SDA
   */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Peripheral clock enable */
  __HAL_RCC_I2C1_CLK_ENABLE();
}

static void I2C1_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);

  /* Peripheral clock disable */
  __HAL_RCC_I2C1_CLK_DISABLE();

  /**I2C1 GPIO Configuration
   PB7     ------> I2C1_SCL
   PB6     ------> I2C1_SDA
  */
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);
}

__weak HAL_StatusTypeDef MX_I2C3_Init(I2C_HandleTypeDef *hi2c)
{
  HAL_StatusTypeDef ret = HAL_OK;
  hi2c->Instance = I2C3;
  hi2c->Init.Timing = 0x00F07BFF;
  hi2c->Init.OwnAddress1 = 0;
  hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c->Init.OwnAddress2 = 0;
  hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if(HAL_I2C_Init(hi2c) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_DISABLE) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  if(HAL_I2CEx_ConfigDigitalFilter(hi2c, 2) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void I2C3_MspInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);
  GPIO_InitTypeDef gpio_init;

  /* Enable I2C clock */
  __HAL_RCC_I2C3_CLK_ENABLE();

  /**I2C3 GPIO Configuration
   PC0     ------> I2C3_SCL
   PB4     ------> I2C3_SDA
   */

  /* Enable GPIO clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  __HAL_RCC_PWR_CLK_ENABLE();

  /* Configure I2C SDA Line */
  gpio_init.Pin = GPIO_PIN_0;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOC, &gpio_init);

  /* Configure I2C SCL Line */
  gpio_init.Pin = GPIO_PIN_4;
  gpio_init.Mode = GPIO_MODE_AF_OD;
  gpio_init.Pull = GPIO_NOPULL;
  gpio_init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_init.Alternate = GPIO_AF4_I2C3;
  HAL_GPIO_Init(GPIOB, &gpio_init);

}

static void I2C3_MspDeInit(I2C_HandleTypeDef *i2cHandle)
{
  UNUSED(i2cHandle);

  /* Peripheral clock disable */
  __HAL_RCC_I2C3_CLK_DISABLE();

  /**I2C3 GPIO Configuration
   PC0     ------> I2C3_SCL
   PB4     ------> I2C3_SDA
   */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
}

/* BUS IO driver over USART and LPUART Peripherals */
/*******************************************************************************
 BUS OPERATIONS OVER USART and LPUART
 *******************************************************************************/

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
__weak HAL_StatusTypeDef MX_USART1_UART_Init(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef ret = HAL_OK;
  huart->Instance = USART1;
  huart->Init.BaudRate = 115200;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if(HAL_UART_Init(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if(HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if(HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if(HAL_UARTEx_DisableFifoMode(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

int32_t BSP_USART1_Init(void)
{
  huart1.Instance = USART1;

  /* Init the UART Msp */
  USART1_MspInit(&huart1);
  (void) MX_USART1_UART_Init(&huart1);

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit USART.
 * @param  none
 * @retval BSP status
 */
int32_t BSP_USART1_DeInit(void)
{
  int32_t ret = BSP_ERROR_PERIPH_FAILURE;

  huart1.Instance = USART1;

  USART1_MspDeInit(&huart1);

  if(HAL_UART_DeInit(&huart1) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}

void USART1_MspInit(UART_HandleTypeDef *huart)
{

  GPIO_InitTypeDef GPIO_InitStruct =
  {
      0 };
  if(huart->Instance == USART1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
     PA10     ------> USART1_RX
     PA9      ------> USART1_TX
     */
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void USART1_MspDeInit(UART_HandleTypeDef *huart)
{
  UNUSED(huart);

  /* Peripheral clock disable */
  __HAL_RCC_USART1_CLK_DISABLE();

  /**USART1 GPIO Configuration
    PA10     ------> USART1_RX
    PA9      ------> USART1_TX
    */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
}

__weak HAL_StatusTypeDef MX_LPUART1_UART_Init(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef ret = HAL_OK;

  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  huart->Instance = LPUART1;
  huart->Init.BaudRate = 460800;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  huart->FifoMode = UART_FIFOMODE_DISABLE;
  if (HAL_UART_Init(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if (HAL_UARTEx_SetTxFifoThreshold(huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if (HAL_UARTEx_SetRxFifoThreshold(huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    ret = HAL_ERROR;
  }
  if (HAL_UARTEx_DisableFifoMode(huart) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    ret = HAL_ERROR;
  }

  return ret;
}

static void LPUART1_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* LPUART1 clock enable */
  __HAL_RCC_LPUART1_CLK_ENABLE();

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /**LPUART1 GPIO Configuration
  PC1     ------> LPUART1_TX
  PB10     ------> LPUART1_RX
  */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

static void LPUART1_MspDeInit(UART_HandleTypeDef *huart)
{
  __HAL_RCC_LPUART1_CLK_DISABLE();

  /**LPUART1 GPIO Configuration
  PC1     ------> LPUART1_TX
  PB10     ------> LPUART1_RX
  */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);

  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);
}

int32_t BSP_LPUART1_Init(void)
{

  /* Init the UART Msp */
  LPUART1_MspInit(&hlpuart1);
  (void) MX_LPUART1_UART_Init(&hlpuart1);

  return BSP_ERROR_NONE;
}

int32_t BSP_LPUART1_DeInit(void)
{
  int32_t ret = BSP_ERROR_PERIPH_FAILURE;

  LPUART1_MspDeInit(&hlpuart1);

  if(HAL_UART_DeInit(&hlpuart1) == HAL_OK)
  {
    ret = BSP_ERROR_NONE;
  }

  return ret;
}


