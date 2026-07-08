/**
 ******************************************************************************
 * @file    NBIOTV1_conf_template.h
 * @author  SRA
 * @brief   Template for NBIOTV1_conf.h (to be placed in user space)
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
#ifndef NBIOTV1_CONF_TEMPLATE_H__
#define NBIOTV1_CONF_TEMPLATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"
#include "NBIOTV1_bus.h"
#include "NBIOTV1_errno.h"

#define USE_MOTION_SENSOR_LIS2DUXS12  (1)
#define USE_ENV_SENSOR_LPS22DF        (1)
#define USE_ENV_SENSOR_SHT40          (1)

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
#define BSP_VSENSORS_PORT                   GPIOA
#define BSP_VSENSORS_PIN                    GPIO_PIN_15
#define BSP_VSENSORS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

/* M95P32 EEPROM */
#define BSP_M95P32_SPI_INIT                     BSP_SPI1_Init
#define BSP_M95P32_SPI_DEINIT                   BSP_SPI1_DeInit
#define BSP_M95P32_SPI_SEND                     BSP_SPI1_Send
#define BSP_M95P32_SPI_RECV                     BSP_SPI1_Recv
#define BSP_M95P32_CS_PORT                      GPIOB
#define BSP_M95P32_CS_PIN                       GPIO_PIN_5
#define BSP_M95P32_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()

// NOTE: rest of the definitions for the EEPROM can be found in the pgeez1_conf.h in userspace

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

#define BUTTON_USER_IT_PRIORITY         14U
#define BUTTON_PWR_IT_PRIORITY          14U

/* BSP COM Port */
#define USE_BSP_COM_FEATURE             1U
#define USE_COM_LOG                     1U
#define BSP_COM_BAUDRATE      115200

/* Define 1 to use already implemented callback; 0 to implement callback
   into an application file */
#define USE_BC_TIM_IRQ_CALLBACK         0U
#define USE_BC_GPIO_IRQ_HANDLER         1U
#define USE_BC_GPIO_IRQ_CALLBACK        1U

#ifdef __cplusplus
}
#endif

#endif /* NBIOTV1_CONF_TEMPLATE_H__*/

