/**
 ******************************************************************************
 * @file    NBIOTV1.c
 * @author  SRA
 * @brief   This file provides a set of firmware functions to manage
 *          LEDs
 *          USER push-buttons
 *          COM port
 *          Voltage regulators
 *          on NBIOTV1 board (STEVAL-NBIOTV1) from STMicroelectronics.
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
#include "NBIOTV1.h"
#include "stdio.h"

#define BSP_ST87_IO_DELAY   (100U) // ms

typedef void (*BSP_EXTI_LineCallback)(void);

#if (USE_BSP_COM_FEATURE > 0)
static void USART2_MspInit(UART_HandleTypeDef *huart);
static void USART2_MspDeInit(UART_HandleTypeDef *huart);
#endif
__weak HAL_StatusTypeDef MX_USART2_Init(UART_HandleTypeDef *huart);
static void BUTTON_USER_EXTI_Callback(void);
static void BUTTON_PWR_EXTI_Callback(void);
void BSP_PB_Callback(Button_TypeDef Button);

/* Private Variables -----------------------------------------------------------*/
static GPIO_TypeDef *BUTTON_PORT[BUTTONn] =
{
    BUTTON_USER_GPIO_PORT,
    BUTTON_PWR_GPIO_PORT };
static const uint16_t BUTTON_PIN[BUTTONn] =
{
    BUTTON_USER_PIN,
    BUTTON_PWR_PIN };
static const IRQn_Type BUTTON_IRQn[BUTTONn] =
{
    BUTTON_USER_EXTI_IRQn,
    BUTTON_PWR_EXTI_IRQn };
static const BSP_EXTI_LineCallback BUTTON_CALLBACK[BUTTONn] =
{
    BUTTON_USER_EXTI_Callback,
    BUTTON_PWR_EXTI_Callback };
static const uint32_t BUTTON_PRIO[BUTTONn] =
{
    BUTTON_USER_IT_PRIORITY,
    BUTTON_PWR_IT_PRIORITY };
static const uint32_t BUTTON_EXTI_LINE[BUTTONn] =
{
    BUTTON_USER_EXTI_LINE,
    BUTTON_PWR_EXTI_LINE };


EXTI_HandleTypeDef hpb_exti[BUTTONn] =
{
    {
        .Line = BUTTON_USER_EXTI_LINE },
    {
        .Line = BUTTON_PWR_EXTI_LINE } };

static GPIO_TypeDef *GPIO_PORT[LEDn] =
{
    LED1_GPIO_PORT,
    LED2_GPIO_PORT };

static const uint16_t GPIO_PIN[LEDn] =
{
    LED1_PIN,
    LED2_PIN };

#if (USE_BSP_COM_FEATURE > 0)
UART_HandleTypeDef hcom_uart[COMn];

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
static uint32_t IsUsart2MspCbValid = 0;
#endif
#if (USE_BSP_COM_FEATURE > 0)
COM_TypeDef COM_ActiveLogPort = COM1;
#endif
#endif /* USE_BSP_COM_FEATURE */

/**
 * @brief  Configures button GPIO and EXTI Line.
 * @param  Button: Button to be configured
 *          This parameter can be one of the following values:
 *            @arg  BUTTON_USER: User Push Button
 *            @arg  BUTTON_TAMPER: Tamper Push Button
 * @param  ButtonMode Button mode
 *          This parameter can be one of the following values:
 *            @arg  BUTTON_MODE_GPIO: Button will be used as simple IO
 *            @arg  BUTTON_MODE_EXTI: Button will be connected to EXTI line
 *                                    with interrupt generation capability
 * @retval BSP status
 */
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
  GPIO_InitTypeDef gpio_init_structure;
  GPIO_TypeDef* button_port = (GPIO_TypeDef*)BUTTON_PORT[Button];

  /* Enable the BUTTON clock*/
  if(Button == BUTTON_USER)
  {
    BUTTON_USER_GPIO_CLK_ENABLE();
  }
  else
  {
    BUTTON_PWR_GPIO_CLK_ENABLE();
  }
  gpio_init_structure.Pin = BUTTON_PIN[Button];

  //gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Pull = GPIO_PULLDOWN;

  gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

  if(ButtonMode == BUTTON_MODE_GPIO)
  {
    /* Configure Button pin as input */
    gpio_init_structure.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(button_port, &gpio_init_structure);
  }
  else /* (ButtonMode == BUTTON_MODE_EXTI) */
  {
    /* Configure Button pin as input with External interrupt */
    gpio_init_structure.Mode = GPIO_MODE_IT_RISING;

    HAL_GPIO_Init(button_port, &gpio_init_structure);

    (void) HAL_EXTI_GetHandle(&hpb_exti[Button], BUTTON_EXTI_LINE[Button]);
    (void) HAL_EXTI_RegisterCallback(&hpb_exti[Button], HAL_EXTI_COMMON_CB_ID, BUTTON_CALLBACK[Button]);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    HAL_NVIC_SetPriority((BUTTON_IRQn[Button]), BUTTON_PRIO[Button], 0x00);
    HAL_NVIC_EnableIRQ(BUTTON_IRQn[Button]);
  }
  return BSP_ERROR_NONE;
}


/**
 * @brief  DeInitialize Push Button.
 * @param  Button: Button to be configured
 *   This parameter should be: BUTTON_USER
 * @note BSP_PB_DeInit() does not disable the GPIO clock
 * @retval None
 */
int32_t BSP_PB_DeInit(Button_TypeDef Button)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_TypeDef* button_port = (GPIO_TypeDef*)BUTTON_PORT[Button];

  GPIO_InitStruct.Pin = BUTTON_PIN[Button];
  HAL_NVIC_DisableIRQ((IRQn_Type) (BUTTON_IRQn[Button]));
  
  HAL_GPIO_DeInit(button_port, GPIO_InitStruct.Pin);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Return the selected Button state.
 * @param  Button: Specifies the Button to be checked.
 *   This parameter should be: BUTTON_USER
 * @retval The Button GPIO pin value.
 */
int32_t BSP_PB_GetState(Button_TypeDef Button)
{
  GPIO_PinState ret_state = HAL_GPIO_ReadPin((GPIO_TypeDef*) BUTTON_PORT[Button], BUTTON_PIN[Button]);
  return (int32_t) (ret_state);
}

/**
 * @brief  User EXTI line detection callbacks.
 * @retval None
 */
static void BUTTON_USER_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_USER);
}

/**
 * @brief  Pwr EXTI line detection callbacks.
 * @retval None
 */
static void BUTTON_PWR_EXTI_Callback(void)
{
  BSP_PB_Callback(BUTTON_PWR);
}

/**
 * @brief  This method returns the STM32446E EVAL BSP Driver revision
 * @param  None
 * @retval version: 0xXYZR (8bits for each decimal, R for RC)
 */
int32_t BSP_GetVersion(void)
{
  return (int32_t) NBIOTV1_BSP_VERSION;
}

/**
 * @brief  Configures LEDs.
 * @param  Led: LED to be configured.
 *          This parameter can be one of the following values:
 *            @arg  LED1
 * @retval None
 */
int32_t BSP_LED_Init(Led_TypeDef Led)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable the GPIO_LED clock */
  LEDx_GPIO_CLK_ENABLE((int8_t )Led);

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = GPIO_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStruct);

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit LEDs.
 * @param  Led: LED to be configured.
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @note Led DeInit does not disable the GPIO clock nor disable the Mfx
 * @retval None
 */
int32_t BSP_LED_DeInit(Led_TypeDef Led)
{
  UNUSED(Led);
  return BSP_ERROR_NONE;
}

/**
 * @brief  Turns selected LED On.
 * @param  Led: LED to be set on
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
int32_t BSP_LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
  return BSP_ERROR_NONE;
}

/**
 * @brief  Turns selected LED Off.
 * @param  Led: LED to be set off
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
int32_t BSP_LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
  return BSP_ERROR_NONE;
}

/**
 * @brief  Toggles the selected LED.
 * @param  Led: LED to be toggled
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval None
 */
int32_t BSP_LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);

  return BSP_ERROR_NONE;
}

/**
 * @brief  Get the selected LED state.
 * @param  Led LED to be get its state
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 * @retval LED status
 */
int32_t BSP_LED_GetState(Led_TypeDef Led)
{
  if((Led != LED1) && (Led != LED2))
  {
    return BSP_ERROR_WRONG_PARAM;
  }
  else if(HAL_GPIO_ReadPin(GPIO_PORT[Led], (uint16_t) GPIO_PIN[Led]) == GPIO_PIN_RESET)
  {
    /* Led is On */
    return 1;
  }
  else
  {
    /* Led is Off */
    return 0;
  }
}

#if (USE_BSP_COM_FEATURE > 0)
/**
 * @brief  Configures COM port.
 * @param  COM: COM port to be configured.
 *              This parameter can be COM1
 * @param  UART_Init: Pointer to a UART_HandleTypeDef structure that contains the
 *                    configuration information for the specified USART peripheral.
 * @retval BSP error code
 */
int32_t BSP_COM_Init(COM_TypeDef COM)
{
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)
  /* Init the UART Msp */
  USART2_MspInit(&hcom_uart[COM]);
#else
  if(IsUsart2MspCbValid == 0U)
  {
    if(BSP_USART2_RegisterDefaultMspCallbacks() != BSP_ERROR_NONE)
    {
      return BSP_ERROR_MSP_FAILURE;
    }
  }
#endif

  (void) MX_USART2_Init(&hcom_uart[COM]);

  return BSP_ERROR_NONE;
}

/**
 * @brief  DeInit COM port.
 * @param  COM COM port to be configured.
 *             This parameter can be COM1
 * @retval BSP status
 */
int32_t BSP_COM_DeInit(COM_TypeDef COM)
{
#if (USE_HAL_UART_REGISTER_CALLBACKS == 0)  
  USART2_MspDeInit(&hcom_uart[COM]);
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 0) */

  if(HAL_UART_DeInit(&hcom_uart[COM]) != HAL_OK)
  {
    return BSP_ERROR_PERIPH_FAILURE;
  }

  return BSP_ERROR_NONE;
}

/**
 * @brief  Configures COM port.
 * @param  huart USART handle
 *               This parameter can be COM1
 * @param  COM_Init Pointer to a UART_HandleTypeDef structure that contains the
 *                  configuration information for the specified USART peripheral.
 * @retval HAL error code
 */

/* USART2 init function */

__weak HAL_StatusTypeDef MX_USART2_Init(UART_HandleTypeDef *huart)
{
  HAL_StatusTypeDef ret = HAL_OK;
  huart->Instance = USART2;
  huart->Init.BaudRate = BSP_COM_BAUDRATE;
  huart->Init.WordLength = UART_WORDLENGTH_8B;
  huart->Init.StopBits = UART_STOPBITS_1;
  huart->Init.Parity = UART_PARITY_NONE;
  huart->Init.Mode = UART_MODE_TX_RX;
  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart->Init.OverSampling = UART_OVERSAMPLING_16;
  huart->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart->Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  huart->FifoMode = UART_FIFOMODE_DISABLE;

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

/**
 * @brief  Initializes USART2 MSP.
 * @param  huart USART2 handle
 * @retval None
 */

static void USART2_MspInit(UART_HandleTypeDef *uartHandle)
{
  UNUSED(uartHandle);
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable Peripheral clock */
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /**USART2 GPIO Configuration    
   PA3     ------> USART2_RX
   PA2     ------> USART2_TX
   */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void USART2_MspDeInit(UART_HandleTypeDef *uartHandle)
{
  UNUSED(uartHandle);

  /* Peripheral clock disable */
  __HAL_RCC_USART2_CLK_DISABLE();

  /**USART2 GPIO Configuration    
   PA3     ------> USART2_RX
   PA2     ------> USART2_TX
   */
  HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);
}

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1) 
/**
 * @brief Register Default USART2 Bus Msp Callbacks
 * @retval BSP status
 */
int32_t BSP_USART2_RegisterDefaultMspCallbacks(void)
{
  int32_t ret = BSP_ERROR_NONE;
  
  __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM1]);
  
  /* Register default MspInit/MspDeInit Callback */
  if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPINIT_CB_ID, USART2_MspInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPDEINIT_CB_ID, USART2_MspDeInit) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsUsart2MspCbValid = 1U;
  }
  
  /* BSP status */  
  return ret;
}

/**
 * @brief Register USART2 Bus Msp Callback registering
 * @param Callbacks pointer to USART2 MspInit/MspDeInit callback functions
 * @retval BSP status
 */
int32_t BSP_USART2_RegisterMspCallbacks(BSP_UART_Cb_t *Callback)
{
  int32_t ret = BSP_ERROR_NONE;
  
  __HAL_UART_RESET_HANDLE_STATE(&hcom_uart[COM1]);
  
  /* Register MspInit/MspDeInit Callbacks */
  if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPINIT_CB_ID, Callback->pMspUsartInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else if(HAL_UART_RegisterCallback(&hcom_uart[COM1], HAL_UART_MSPDEINIT_CB_ID, Callback->pMspUsartDeInitCb) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }
  else
  {
    IsUsart2MspCbValid = 1U;
  }
  
  /* BSP status */  
  return ret; 
}
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

#if (USE_COM_LOG > 0)
/**
 * @brief  Select the active COM port.
 * @param  COM port to be activated.
 * @retval BSP status
 */
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM)
{
  if(COM_ActiveLogPort != COM)
  {
    COM_ActiveLogPort = COM;
  }
  return BSP_ERROR_NONE;
}

#if defined(__CC_ARM) /* For arm compiler 5 */
#if !defined(__MICROLIB) /* If not Microlib */

struct __FILE
{
  int dummyVar; //Just for the sake of redefining __FILE, we won't we using it anyways ;)
};

FILE __stdout;

#endif /* If not Microlib */
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* For arm compiler 6 */
#if !defined(__MICROLIB) /* If not Microlib */

FILE __stdout;

#endif /* If not Microlib */
#endif /* For arm compiler 5 */
#if defined(__ICCARM__) /* For IAR */
size_t __write(int Handle, const unsigned char *Buf, size_t Bufsize)
{
  int i;

  for(i=0; i<Bufsize; i++)
  {
    (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&Buf[i], 1, COM_POLL_TIMEOUT);
  }

  return Bufsize;
}
#elif defined(__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)) /* For ARM Compiler 5 and 6 */
int fputc (int ch, FILE *f)
{
  (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#else /* For GCC Toolchains */
int __io_putchar (int ch)
{
  (void)HAL_UART_Transmit(&hcom_uart[COM_ActiveLogPort], (uint8_t *)&ch, 1, COM_POLL_TIMEOUT);
  return ch;
}
#endif /* For IAR */
#endif /* USE_COM_LOG */
#endif /* USE_BSP_COM_FEATURE */

/**
 * @brief  Set all sensor Chip Select high. To be called before any SPI read/write
 * @param  None
 * @retval HAL_StatusTypeDef HAL Status
 */
uint8_t BSP_Sensor_IO_SPI_CS_Init_All(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Set all the pins before init to avoid glitch */
  BSP_LIS2DUXS12_CS_GPIO_CLK_ENABLE();
  BSP_LPS22DF_CS_GPIO_CLK_ENABLE();

  HAL_GPIO_WritePin(BSP_LIS2DUXS12_CS_PORT, BSP_LIS2DUXS12_CS_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BSP_LPS22DF_CS_PORT, BSP_LPS22DF_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_LIS2DUXS12_CS_PIN;
  HAL_GPIO_Init(BSP_LIS2DUXS12_CS_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_LPS22DF_CS_PIN;
  HAL_GPIO_Init(BSP_LPS22DF_CS_PORT, &GPIO_InitStruct);

  return 0;
}

int32_t BSP_SensorsAndEEPROM_PS_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Set all the pins before init to avoid glitch */
  BSP_VSENSORS_GPIO_CLK_ENABLE();
  
  // by default turned off
  HAL_GPIO_WritePin(BSP_VSENSORS_PORT, BSP_VSENSORS_PIN, GPIO_PIN_RESET);
 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_VSENSORS_PIN;
  HAL_GPIO_Init(BSP_VSENSORS_PORT, &GPIO_InitStruct);

  return BSP_ERROR_NONE;
}

int32_t BSP_SensorsAndEEPROM_PS_DeInit(void)
{
  HAL_GPIO_DeInit(BSP_VSENSORS_PORT, BSP_VSENSORS_PIN);

  return BSP_ERROR_NONE;
}

int32_t BSP_SensorsAndEEPROM_PS_On(void)
{
  HAL_GPIO_WritePin(BSP_VSENSORS_PORT, BSP_VSENSORS_PIN, GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

int32_t BSP_SensorsAndEEPROM_PS_Off(void)
{
  HAL_GPIO_WritePin(BSP_VSENSORS_PORT, BSP_VSENSORS_PIN, GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

int32_t BSP_ST87Module_IO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Set all the pins before init to avoid glitch */
  BSP_ST87_ANTENNA_LDO_EN_GPIO_CLK_ENABLE();
  BSP_ST87_RESET_GPIO_CLK_ENABLE();
  BSP_ST87_WAKEUP_GPIO_CLK_ENABLE();
  BSP_ST87_RING_GPIO_CLK_ENABLE();
  
  // by default set to low
  HAL_GPIO_WritePin(BSP_ST87_ANTENNA_LDO_EN_PORT, BSP_ST87_ANTENNA_LDO_EN_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BSP_ST87_RESET_PORT, BSP_ST87_RESET_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(BSP_ST87_WAKEUP_PORT, BSP_ST87_WAKEUP_PIN, GPIO_PIN_RESET);
 
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_ST87_ANTENNA_LDO_EN_PIN;
  HAL_GPIO_Init(BSP_ST87_ANTENNA_LDO_EN_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_ST87_RESET_PIN;
  HAL_GPIO_Init(BSP_ST87_RESET_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = BSP_ST87_WAKEUP_PIN;
  HAL_GPIO_Init(BSP_ST87_WAKEUP_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  HAL_GPIO_Init(BSP_ST87_RING_PORT, &GPIO_InitStruct);

  //(void) HAL_EXTI_GetHandle(hexti5, ST87_RING_INT_EXTI_LINE);
  //(void) HAL_EXTI_RegisterCallback(hexti5, HAL_EXTI_COMMON_CB_ID, RING_PIN_CALLBACK);

  /* Enable and set EXTI Interrupt */
  HAL_NVIC_SetPriority(BSP_ST87_RING_EXTI_IRQn, BSP_ST87_RING_EXTI_IRQ_PP, BSP_ST87_RING_EXTI_IRQ_SP);
  HAL_NVIC_EnableIRQ(BSP_ST87_RING_EXTI_IRQn);

  return BSP_ERROR_NONE;
}

int32_t BSP_ST87Module_IO_DeInit(void)
{
  HAL_NVIC_DisableIRQ(BSP_ST87_RING_EXTI_IRQn);

  HAL_GPIO_DeInit(BSP_ST87_ANTENNA_LDO_EN_PORT, BSP_ST87_ANTENNA_LDO_EN_PIN);
  HAL_GPIO_DeInit(BSP_ST87_RESET_PORT, BSP_ST87_RESET_PIN);
  HAL_GPIO_DeInit(BSP_ST87_WAKEUP_PORT, BSP_ST87_WAKEUP_PIN);
  HAL_GPIO_DeInit(BSP_ST87_RING_PORT, BSP_ST87_RING_PIN);

  BSP_ST87_ANTENNA_LDO_EN_GPIO_CLK_DISABLE();
  BSP_ST87_RESET_GPIO_CLK_DISABLE();
  BSP_ST87_WAKEUP_GPIO_CLK_DISABLE();
  BSP_ST87_RING_GPIO_CLK_DISABLE();

  return BSP_ERROR_NONE;
}

int32_t BSP_ST87Module_WakeUp(void)
{
  HAL_GPIO_WritePin(BSP_ST87_WAKEUP_PORT, BSP_ST87_WAKEUP_PIN, GPIO_PIN_SET);

  NBIOTV1_BSP_Delay(BSP_ST87_IO_DELAY);

  HAL_GPIO_WritePin(BSP_ST87_WAKEUP_PORT, BSP_ST87_WAKEUP_PIN, GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

int32_t BSP_ST87Module_Reset(void)
{
  HAL_GPIO_WritePin(BSP_ST87_RESET_PORT, BSP_ST87_RESET_PIN, GPIO_PIN_SET);

  NBIOTV1_BSP_Delay(BSP_ST87_IO_DELAY);

  HAL_GPIO_WritePin(BSP_ST87_RESET_PORT, BSP_ST87_RESET_PIN, GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

int32_t BSP_ST87Module_EnableAntennaLDO(void)
{
  HAL_GPIO_WritePin(BSP_ST87_ANTENNA_LDO_EN_PORT, BSP_ST87_ANTENNA_LDO_EN_PIN, GPIO_PIN_SET);

  return BSP_ERROR_NONE;
}

int32_t BSP_ST87Module_DisableAntennaLDO(void)
{
  HAL_GPIO_WritePin(BSP_ST87_ANTENNA_LDO_EN_PORT, BSP_ST87_ANTENNA_LDO_EN_PIN, GPIO_PIN_RESET);

  return BSP_ERROR_NONE;
}

int32_t BSP_SystemClock_Config(void)
{
  int32_t ret = BSP_ERROR_NONE;

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    ret = BSP_ERROR_CLOCK_FAILURE;
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                             |RCC_OSCILLATORTYPE_MSIK;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIKClockRange = RCC_MSIKRANGE_4;
  RCC_OscInitStruct.MSIKState = RCC_MSIK_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    ret = BSP_ERROR_CLOCK_FAILURE;
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                             |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                             |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    ret = BSP_ERROR_CLOCK_FAILURE;
  }

  /** Enable the force of MSIK in stop mode
  */
  __HAL_RCC_MSIKSTOP_ENABLE();

  return ret;
}

/**
* @brief BSP Push Button callback
* @param Button Specifies the pin connected EXTI line
* @retval None.
*/
__weak void BSP_PB_Callback(Button_TypeDef Button)
{
/* Prevent unused argument(s) compilation warning */
UNUSED(Button);
/* This function should be implemented by the user application.
It is called into this driver when an event on Button is triggered. */
}
