/**
 ******************************************************************************
 * @file    NBIOTV1.h
 * @author  SRA
 * @brief   This file contains definitions for NBIOTV1.c
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
#ifndef NBIOTV1_H
#define NBIOTV1_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "NBIOTV1_conf.h"
#include "NBIOTV1_errno.h"

#ifndef NBIOTV1_BSP_Delay
#define NBIOTV1_BSP_Delay(delay)   HAL_Delay(delay)
#endif

#define NBIOTV1_BSP_VERSION_MAIN   (0x01U) /*!< [31:24] main version */
#define NBIOTV1_BSP_VERSION_SUB1   (0x00U) /*!< [23:16] sub1 version */
#define NBIOTV1_BSP_VERSION_SUB2   (0x00U) /*!< [15:8]  sub2 version */
#define NBIOTV1_BSP_VERSION_RC     (0x00U) /*!< [7:0]  release candidate */
#define NBIOTV1_BSP_VERSION        ((NBIOTV1_BSP_VERSION_MAIN << 24)\
                                   |(NBIOTV1_BSP_VERSION_SUB1 << 16)\
                                   |(NBIOTV1_BSP_VERSION_SUB2 << 8 )\
                                   |(NBIOTV1_BSP_VERSION_RC))

typedef enum
{
  LED1 = 0,
  LED_GREEN = LED1,
  LED2 = 1,
  LED_ORANGE = LED2
} Led_TypeDef;

typedef enum
{
  SPI_DEV_LPS22DF,
  SPI_DEV_LIS2DUXS12
} SPI_Device_t;

typedef enum
{
  BUTTON_USER = 0U,
  /* Alias */
  BUTTON_KEY = BUTTON_USER,
  BUTTON_PWR = 1U
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0U,
  BUTTON_MODE_EXTI = 1U
} ButtonMode_TypeDef;

typedef enum
{
  COM1 = 0U,
} COM_TypeDef;
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  pUART_CallbackTypeDef  pMspUsartInitCb;
  pUART_CallbackTypeDef  pMspUsartDeInitCb;
} BSP_UART_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */

typedef enum
{
  ADC1_NOT_USED = 0,
  ADC1_FOR_BC = 1,
  ADC1_FORCE_RESTART = 2
} ADC_InitUsedDef;

#define LEDn                                  2

#define LED1_PIN                              GPIO_PIN_12
#define LED1_GPIO_PORT                        GPIOC
#define LED1_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOC_CLK_DISABLE()

#define LED2_PIN                              GPIO_PIN_2
#define LED2_GPIO_PORT                        GPIOD
#define LED2_GPIO_CLK_ENABLE()                __HAL_RCC_GPIOD_CLK_ENABLE()
#define LED2_GPIO_CLK_DISABLE()               __HAL_RCC_GPIOD_CLK_DISABLE()

#define LEDx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {LED1_GPIO_CLK_ENABLE();} else\
                                                                    {LED2_GPIO_CLK_ENABLE();   }} while(0)
#define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do { if((__INDEX__) == 0) {LED1_GPIO_CLK_DISABLE();} else\
                                                                    {LED2_GPIO_CLK_DISABLE();   }} while(0)

/* LPS22DF */
#define BSP_LPS22DF_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LPS22DF_INT_EXTI_IRQHandler       EXTI11_IRQHandler

/* LIS2DUXS12 */  
#define BSP_LIS2DUXS12_INT_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_LIS2DUXS12_INT_PORT               GPIOA
#define BSP_LIS2DUXS12_INT_PIN                GPIO_PIN_0
#define BSP_LIS2DUXS12_INT_EXTI_IRQn          EXTI0_IRQn
#ifndef BSP_LIS2DUXS12_INT_EXTI_IRQ_PP
#define BSP_LIS2DUXS12_INT_EXTI_IRQ_PP        7
#endif
#ifndef BSP_LIS2DUXS12_INT_EXTI_IRQ_SP
#define BSP_LIS2DUXS12_INT_EXTI_IRQ_SP        0
#endif
#define BSP_LIS2DUXS12_INT_EXTI_IRQHandler     EXTI0_IRQHandler

/* SHT40 */  
// no CS nor EXTI configuration
                                                                    
#define BUTTONn                               2

/**
 * @brief Key push-button
 */
#define BUTTON_USER_PIN                       GPIO_PIN_3
#define BUTTON_USER_GPIO_PORT                 GPIOH
#define BUTTON_USER_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOH_CLK_ENABLE()
#define BUTTON_USER_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOH_CLK_DISABLE()
#define BUTTON_USER_EXTI_LINE                 EXTI_LINE_3
#define BUTTON_USER_EXTI_IRQn                 EXTI3_IRQn
#define H_EXTI_3			                        hpb_exti[BUTTON_USER]

#define BUTTON_PWR_PIN                        GPIO_PIN_15
#define BUTTON_PWR_GPIO_PORT                  GPIOB
#define BUTTON_PWR_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUTTON_PWR_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOB_CLK_DISABLE()
#define BUTTON_PWR_EXTI_LINE                  EXTI_LINE_15
#define BUTTON_PWR_EXTI_IRQn                  EXTI15_IRQn
#define H_EXTI_15			                        hpb_exti[BUTTON_PWR]

#define BUTTONx_GPIO_CLK_ENABLE(__INDEX__)   do { if((__INDEX__) == 0) {USER_BUTTON_GPIO_CLK_ENABLE();} else\
                                                                    {POWER_BUTTON_GPIO_CLK_ENABLE();   }} while(0)
#define BUTTONx_GPIO_CLK_DISABLE(__INDEX__)   do { if((__INDEX__) == 0) {USER_BUTTON_GPIO_CLK_DISABLE();} else\
                                                                    {POWER_BUTTON_GPIO_CLK_DISABLE();   }} while(0)

#ifndef BUTTON_USER_IT_PRIORITY
#define BUTTON_USER_IT_PRIORITY     14U
#endif

#ifndef BUTTON_PWR_IT_PRIORITY
#define BUTTON_PWR_IT_PRIORITY      14U
#endif

#define COMn                             1U 
#define COM1_UART                        USART2

#define COM_POLL_TIMEOUT                 1000

#define UartHandle huart2

#ifndef BSP_COM_BAUDRATE
 #define BSP_COM_BAUDRATE                921600
#endif


/* On-board ST87 module */
#define BSP_ST87_ANTENNA_LDO_EN_PORT                  GPIOC
#define BSP_ST87_ANTENNA_LDO_EN_PIN                   GPIO_PIN_10
#define BSP_ST87_ANTENNA_LDO_EN_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_ST87_ANTENNA_LDO_EN_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()

#define BSP_ST87_RESET_PORT                           GPIOC
#define BSP_ST87_RESET_PIN                            GPIO_PIN_9
#define BSP_ST87_RESET_GPIO_CLK_ENABLE()              __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_ST87_RESET_GPIO_CLK_DISABLE()             __HAL_RCC_GPIOC_CLK_DISABLE()

#define BSP_ST87_WAKEUP_PORT                          GPIOC
#define BSP_ST87_WAKEUP_PIN                           GPIO_PIN_8
#define BSP_ST87_WAKEUP_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_ST87_WAKEUP_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOC_CLK_DISABLE()

#define BSP_ST87_RING_PORT                            GPIOC
#define BSP_ST87_RING_PIN                             GPIO_PIN_5
#define BSP_ST87_RING_GPIO_CLK_ENABLE()               __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_ST87_RING_GPIO_CLK_DISABLE()              __HAL_RCC_GPIOC_CLK_DISABLE()
  extern EXTI_HandleTypeDef hexti5;
#define H_EXTI_5          hexti5
#define H_EXTI_ST87_RING                              hexti5
#define ST87_RING_INT_EXTI_LINE                       EXTI_LINE_5
#define BSP_ST87_RING_EXTI_IRQn                       EXTI5_IRQn
#ifndef BSP_ST87_RING_EXTI_IRQ_PP
#define BSP_ST87_RING_EXTI_IRQ_PP                     7
#endif
#ifndef BSP_ST87_RING_EXTI_IRQ_SP
#define BSP_ST87_RING_EXTI_IRQ_SP                     0
#endif

/* Power supply for the sensors */
#define BSP_VSENSORS_PORT                             GPIOA
#define BSP_VSENSORS_PIN                    GPIO_PIN_15
#define BSP_VSENSORS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()


/* M95P32 EEPROM */
#define BSP_M95P32_SPI_INIT                     BSP_SPI1_Init
#define BSP_M95P32_SPI_DEINIT                   BSP_SPI1_DeInit
#define BSP_M95P32_SPI_SEND                     BSP_SPI1_Send
#define BSP_M95P32_SPI_RECV                     BSP_SPI1_Recv
#define BSP_M95P32_SPI_SENDRECV                 BSP_SPI1_SendRecv
#define BSP_M95P32_CS_PORT                      GPIOB
#define BSP_M95P32_CS_PIN                       GPIO_PIN_5
#define BSP_M95P32_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

 //NOTE: rest of the definitions for the EEPROM can be found in the pgeez1_conf.h in userspace

/* SHT40 temp Sensor */
#define BSP_SHT40_I2C_DEV_ADDR                  (0x89U)
#define BSP_SHT40_I2C_INIT                  BSP_I2C1_Init
#define BSP_SHT40_I2C_DEINIT                BSP_I2C1_DeInit
#define BSP_SHT40_I2C_READ                  BSP_I2C1_Recv
#define BSP_SHT40_I2C_WRITE                 BSP_I2C1_Send

/* LPS22DF press Sensor */
extern EXTI_HandleTypeDef hexti11;
#define H_EXTI_11         hexti11
#define H_EXTI_INT_LPS22DF                        hexti11
#define LPS22DF_INT_EXTI_LINE                    EXTI_LINE_11
#define BSP_LPS22DF_INT_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOC_CLK_ENABLE()
#define BSP_LPS22DF_INT_PORT                     GPIOC
#define BSP_LPS22DF_INT_PIN                      GPIO_PIN_11
#define BSP_LPS22DF_INT_EXTI_IRQn                EXTI11_IRQn
#ifndef BSP_LPS22DF_INT_EXTI_IRQ_PP
#define BSP_LPS22DF_INT_EXTI_IRQ_PP              7
#endif
#ifndef BSP_LPS22DF_INT_EXTI_IRQ_SP
#define BSP_LPS22DF_INT_EXTI_IRQ_SP              0
#endif
#define BSP_LPS22DF_SPI_INIT                     BSP_SPI2_Init
#define BSP_LPS22DF_SPI_DEINIT                   BSP_SPI2_DeInit
#define BSP_LPS22DF_SPI_SEND                     BSP_SPI2_Send
#define BSP_LPS22DF_SPI_RECV                     BSP_SPI2_Recv
#define BSP_LPS22DF_CS_PORT                      GPIOB
#define BSP_LPS22DF_CS_PIN                       GPIO_PIN_8
#define BSP_LPS22DF_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

/* LIS2DUXS12 acc Sensor */
extern EXTI_HandleTypeDef hexti0;
#define H_EXTI_0          hexti0
#define H_EXTI_INT1_LIS2DUXS12                        hexti0
#define LIS2DUXS12_INT1_EXTI_LINE                     EXTI_LINE_0
#define BSP_LIS2DUXS12_INT1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_LIS2DUXS12_INT1_PORT                      GPIOA
#define BSP_LIS2DUXS12_INT1_PIN                       GPIO_PIN_0
#define BSP_LIS2DUXS12_INT1_EXTI_IRQn                 EXTI0_IRQn

#ifndef BSP_LIS2DUXS12_INT1_EXTI_IRQ_PP
#define BSP_LIS2DUXS12_INT1_EXTI_IRQ_PP               7
#endif

#ifndef BSP_LIS2DUXS12_INT1_EXTI_IRQ_SP
#define BSP_LIS2DUXS12_INT1_EXTI_IRQ_SP               0
#endif

extern EXTI_HandleTypeDef hexti1;
#define H_EXTI_1          hexti1
#define H_EXTI_INT2_LIS2DUXS12                        hexti1
#define LIS2DUXS12_INT2_EXTI_LINE                     EXTI_LINE_1
#define BSP_LIS2DUXS12_INT2_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define BSP_LIS2DUXS12_INT2_PORT                      GPIOB
#define BSP_LIS2DUXS12_INT2_PIN                       GPIO_PIN_1
#define BSP_LIS2DUXS12_INT2_EXTI_IRQn                 EXTI1_IRQn

#ifndef BSP_LIS2DUXS12_INT2_EXTI_IRQ_PP
#define BSP_LIS2DUXS12_INT2_EXTI_IRQ_PP               7
#endif

#ifndef BSP_LIS2DUXS12_INT2_EXTI_IRQ_SP
#define BSP_LIS2DUXS12_INT2_EXTI_IRQ_SP               0
#endif

#define BSP_LIS2DUXS12_SPI_INIT                       BSP_SPI2_Init
#define BSP_LIS2DUXS12_SPI_DEINIT                     BSP_SPI2_DeInit
#define BSP_LIS2DUXS12_SPI_SEND                       BSP_SPI2_Send
#define BSP_LIS2DUXS12_SPI_RECV                       BSP_SPI2_Recv
#define BSP_LIS2DUXS12_CS_PORT                        GPIOB
#define BSP_LIS2DUXS12_CS_PIN                         GPIO_PIN_9
#define BSP_LIS2DUXS12_CS_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()




typedef enum
{
  COM_STOPBITS_1 = UART_STOPBITS_1,
  COM_STOPBITS_2 = UART_STOPBITS_2,
} COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE = UART_PARITY_NONE,
  COM_PARITY_EVEN = UART_PARITY_EVEN,
  COM_PARITY_ODD = UART_PARITY_ODD,
} COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE = UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS = UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS = UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS = UART_HWCONTROL_RTS_CTS,
} COM_HwFlowCtlTypeDef;

typedef struct
{
  uint32_t BaudRate;
  uint32_t WordLength;
  COM_StopBitsTypeDef StopBits;
  COM_ParityTypeDef Parity;
  COM_HwFlowCtlTypeDef HwFlowCtl;
} COM_InitTypeDef;

extern EXTI_HandleTypeDef hpb_exti[BUTTONn];

/* Exported Functions --------------------------------------------------------*/
int32_t BSP_GetVersion(void);
int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);
int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t BSP_PB_DeInit(Button_TypeDef Button);
int32_t BSP_PB_GetState(Button_TypeDef Button);
int32_t BSP_COM_Init(COM_TypeDef COM);
int32_t BSP_COM_DeInit(COM_TypeDef COM);
int32_t BSP_COM_SelectLogPort(COM_TypeDef COM);

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1) 
int32_t BSP_USART2_RegisterDefaultMspCallbacks(void);
int32_t BSP_USART2_RegisterMspCallbacks(BSP_UART_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

void BSP_PB_PWR_Init(void);

int32_t BSP_SensorsAndEEPROM_PS_Init(void);
int32_t BSP_SensorsAndEEPROM_PS_DeInit(void);
int32_t BSP_SensorsAndEEPROM_PS_On(void);
int32_t BSP_SensorsAndEEPROM_PS_Off(void);
uint8_t BSP_Sensor_IO_SPI_CS_Init_All(void);

int32_t BSP_ST87Module_IO_Init(void);
int32_t BSP_ST87Module_IO_DeInit(void);
int32_t BSP_ST87Module_WakeUp(void);
int32_t BSP_ST87Module_Reset(void);
int32_t BSP_ST87Module_EnableAntennaLDO(void);
int32_t BSP_ST87Module_DisableAntennaLDO(void);

int32_t BSP_SystemClock_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* NBIOTV1_H */
