 /**
  ******************************************************************************
  * @file    st87ec_lib_hal.h
  * @author  APMS Application Team
  * @brief   Easy Connect lib HAL functions for STM32
  *
  * @verbatim
  * @endverbatim
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics International N.V.
  * All rights reserved.
  *
  ******************************************************************************
  */
#include <string.h>
#include "st87ec_lib.h"
#include "st87ec_wrapper.h"
#include "st87ec_lib_hal.h"
#include "st87ec_debug.h"

/* Constants -----------------------------------------------------------------*/

/* Check if the user has defined the required macros in st87ec_lib_conf.h */

#ifndef ST87EC_LIB_RESETn_GPIO_Port
#error "ST87EC_LIB_RESETn_GPIO_Port not defined. Please define it in st87ec_lib_conf.h (user space)"
#endif

#ifndef ST87EC_LIB_RESETn_Pin
#error "ST87EC_LIB_RESETn_Pin not defined. Please define it in st87ec_lib_conf.h (user space)"
#endif

#ifndef ST87EC_LIB_UART_HANDLE
#error "ST87EC_LIB_UART_HANDLE not defined. Please define it in st87ec_lib_conf.h (user space)"
#endif


/* Global variables ----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/**
* @brief ST87 Set the GPIO for the ST87 Reset pin
*
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_GpioInit(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* ST87EC USER CODE BEGIN  */
  /* Configure a GPO for ST87 Reset GPIO */
  /* Configure a GPI for ST87 ring pin, when ring pin rises call ST87EC_Lib_Hal_RingPinIsr */
  /* ST87EC USER CODE END  */

  return(result);
}

/**
* @brief ST87 Set the GPIO for the ST87 Reset pin
*
* @param State : State to set on the Reset pin
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_DriveResetGpio(uint32_t State)
{
  ST87EC_Lib_Result_t result = RESULT_OK;
  /* ST87EC USER CODE BEGIN  */
  /* Configure a GPO to Reset the ST87 */
  HAL_GPIO_WritePin(ST87EC_LIB_RESETn_GPIO_Port, ST87EC_LIB_RESETn_Pin, (GPIO_PinState) State);
  /* ST87EC USER CODE END */

  return(result);
}

/**
* @brief ST87 Ring Pin Isr function
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_RingPinIsr(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  ST87EC_Wrapper_RingPinIsr();

  return(result);
}


/**
* @brief ST87 Rx/Tx UART interrupts enabling function.
*
* @param Flags : bitmap of interrupts to enable
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_EnableUartInterrupts(uint8_t Flags)
{
  if ((Flags & ST87EC_UART_FLAG_RX_NOTEMPTY) == ST87EC_UART_FLAG_RX_NOTEMPTY)
  {
    /* ST87EC USER CODE BEGIN  */
    /* Enables interrupts for the UART RX communicating with the ST87 */
    __HAL_UART_ENABLE_IT(ST87EC_LIB_UART_HANDLE, UART_IT_RXNE);
    /* ST87EC USER CODE END  */
  }
  if ((Flags & ST87EC_UART_FLAG_TX_COMPLETE) == ST87EC_UART_FLAG_TX_COMPLETE)
  {
    /* ST87EC USER CODE BEGIN  */
    /* Enables interrupts for the UART TX communicating with the ST87 */
    __HAL_UART_ENABLE_IT(ST87EC_LIB_UART_HANDLE, UART_IT_TC);
    /* ST87EC USER CODE END  */
  }

  return RESULT_OK;
}


/**
* @brief ST87 UART ISR handler wrapper function
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_UartIsr(void)
{
  return ST87EC_Wrapper_UartIsr();
}


/**
* @brief  Function wrapping the driver function writing a byte on ST87 UART Tx
*
* @param Byte : Char to send over UART
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_UartTxByte(uint8_t Byte)
{
  /* ST87EC USER CODE BEGIN  */
  /* Sends byte to UART TX to ST87 */
  LL_USART_TransmitData8(ST87EC_LIB_UART_HANDLE->Instance, Byte);
  /* ST87EC USER CODE END  */

  return RESULT_OK;
}


/**
* @brief  Function wrapping the driver function reading a byte on ST87 UART Rx
*
* @param : None
* @retval Byte read from ST87 UART
*/
uint8_t ST87EC_Lib_Hal_UartRxByte(void)
{
  uint8_t RxByte = 0U;

  /* ST87EC USER CODE BEGIN  */
  /* Receive byte from UART RX from ST87 */
  RxByte = LL_USART_ReceiveData8(ST87EC_LIB_UART_HANDLE->Instance);
  /* ST87EC USER CODE END  */

  return RxByte;
}


/**
* @brief ST87 Rx/Tx UART driver status reading function.
*
* @param UartChan : UART channel selection (Rx or Tx)
* @param StsFlagToGet : status flag to read (only one flag indication expected, not a bitmap)
* @retval flag value read
*/
bool ST87EC_Lib_Hal_UartGetStatusFlag(ST87EC_Lib_UartChannel_t UartChan, uint8_t StsFlagToGet)
{
  bool status = false;

  if (UartChan == UART_CHANNEL_RX)
  {
    if (StsFlagToGet == ST87EC_UART_FLAG_RX_NOTEMPTY)
    {
      /* ST87EC USER CODE BEGIN  */
      /* Get host UART RX flag */
      status = __HAL_UART_GET_FLAG(ST87EC_LIB_UART_HANDLE, UART_FLAG_RXNE);
      /* ST87EC USER CODE END  */

    }
    else {}
  }
  else if (UartChan == UART_CHANNEL_TX)
  {
    if (StsFlagToGet == ST87EC_UART_FLAG_TX_COMPLETE)
    {
      /* ST87EC USER CODE BEGIN  */
      /* Get host UART TX flag */
      status = __HAL_UART_GET_FLAG(ST87EC_LIB_UART_HANDLE, UART_FLAG_TC);
      /* ST87EC USER CODE END  */
    }
    else {}
  }
  else {}

  return status;
}


/**
* @brief ST87 Rx/Tx UART driver status flag clearing function.
*
* @param UartChan : UART channel selection (Rx or Tx)
* @param FlagsToClear : bitmap of status flag to clear
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_UartClearFlags(ST87EC_Lib_UartChannel_t UartChan, uint8_t FlagsToClear)
{

  if (UartChan == UART_CHANNEL_RX)
  {
    if ((FlagsToClear & ST87EC_UART_CLEAR_FLAG_RXOVERRUN) == ST87EC_UART_CLEAR_FLAG_RXOVERRUN)
    {
      /* ST87EC USER CODE BEGIN  */
      /* Clear host UART RX flag */
      __HAL_UART_CLEAR_OREFLAG(ST87EC_LIB_UART_HANDLE);
      /* ST87EC USER CODE END  */
    }
  }
  else if (UartChan == UART_CHANNEL_TX)
  {
    if ((FlagsToClear & ST87EC_UART_CLEAR_FLAG_TXCOMPLETE) == ST87EC_UART_CLEAR_FLAG_TXCOMPLETE)
    {
      /* ST87EC USER CODE BEGIN  */
      /* Clear host UART TX flag */
      __HAL_UART_CLEAR_FLAG(ST87EC_LIB_UART_HANDLE, UART_CLEAR_TCF);
      /* ST87EC USER CODE END  */
    }
  }
  else {}

  return RESULT_OK;
}


/**
* @brief ST87 Tick Isr function
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t  ST87EC_Lib_Hal_TickIsr(void)
{
  return ST87EC_Wrapper_TickIsr();
}


/**
* @brief Host baudrate setting for ST87 UART
*
* @param BaudRate : baudrate value to be set
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_Lib_Hal_SetHostUartBaudrate(uint32_t BaudRate)
{
  /* ST87EC USER CODE BEGIN  */
  /* Change Host UART baudrate by stopping/restarting UART drivers */

  ST87EC_TRACE("SetHostUartBaudrate to: %d, mode is %d", BaudRate, EcLibVars.BootMode);
  HAL_UART_Abort_IT(ST87EC_LIB_UART_HANDLE);
  HAL_UART_DeInit(ST87EC_LIB_UART_HANDLE);
  ST87EC_LIB_UART_HANDLE->Init.BaudRate = BaudRate;
  if (HAL_UART_Init(ST87EC_LIB_UART_HANDLE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable back UART interrupts */
  ST87EC_Lib_Hal_EnableUartInterrupts(ST87EC_UART_FLAG_RX_NOTEMPTY | ST87EC_UART_FLAG_TX_COMPLETE);
  /* ST87EC USER CODE END  */

  return RESULT_OK;
}


/**
* @brief Host baudrate setting for ST87 UART
*
* @param: None
* @retval Returned baudrate
*/
uint32_t ST87EC_Lib_Hal_GetHostUartBaudrate(void)
{
  uint32_t baudrate = 0;
  /* ST87EC USER CODE BEGIN  */
  /* Change Host UART baudrate */
  baudrate = ST87EC_LIB_UART_HANDLE->Init.BaudRate;
  /* ST87EC USER CODE END  */
  return baudrate;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
