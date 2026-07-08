/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ST87_LPUART_TX_Pin GPIO_PIN_1
#define ST87_LPUART_TX_GPIO_Port GPIOC
#define LIS2DUXS12_INT1_Pin GPIO_PIN_0
#define LIS2DUXS12_INT1_GPIO_Port GPIOA
#define LIS2DUXS12_INT1_EXTI_IRQn EXTI0_IRQn
#define LIS2DUXS12_INT2_Pin GPIO_PIN_1
#define LIS2DUXS12_INT2_GPIO_Port GPIOB
#define LIS2DUXS12_INT2_EXTI_IRQn EXTI1_IRQn
#define ST87_LPUART_RX_Pin GPIO_PIN_10
#define ST87_LPUART_RX_GPIO_Port GPIOB
#define STBC02_C_EN_Pin GPIO_PIN_12
#define STBC02_C_EN_GPIO_Port GPIOB
#define ST87_RESETn_Pin GPIO_PIN_9
#define ST87_RESETn_GPIO_Port GPIOC
#define STBC02_SW_Pin GPIO_PIN_8
#define STBC02_SW_GPIO_Port GPIOA
#define STM32_VSENSOR_Pin GPIO_PIN_15
#define STM32_VSENSOR_GPIO_Port GPIOA
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOC
#define LED_ORANGE_Pin GPIO_PIN_2
#define LED_ORANGE_GPIO_Port GPIOD
#define BOOT0_USR_Pin GPIO_PIN_3
#define BOOT0_USR_GPIO_Port GPIOH
#define BOOT0_USR_EXTI_IRQn EXTI3_IRQn
#define LPS22DF_CS_Pin GPIO_PIN_8
#define LPS22DF_CS_GPIO_Port GPIOB
#define LIS2DUXS12_CS_Pin GPIO_PIN_9
#define LIS2DUXS12_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define GPO_MODEM_RESETn_Pin ST87_RESETn_Pin
#define GPO_MODEM_RESETn_GPIO_Port ST87_RESETn_GPIO_Port
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
