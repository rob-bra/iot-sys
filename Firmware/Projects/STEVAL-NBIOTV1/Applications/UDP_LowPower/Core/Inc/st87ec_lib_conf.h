/**
  ******************************************************************************
  * @file    st87ec_lib_conf.h
  * @brief   Easy Connect library configuration file (User space)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __ST87EC_LIB_CONF_H__
#define __ST87EC_LIB_CONF_H__

#include "usart.h"
#include "stm32u5xx_ll_usart.h"

/* UART handle definition for ST87EC_Lib */
#define ST87EC_LIB_UART_HANDLE                (&hlpuart1)

/* Reset GPIO definition for ST87EC_Lib */
#define ST87EC_LIB_RESETn_GPIO_Port           ST87_RESETn_GPIO_Port
#define ST87EC_LIB_RESETn_Pin                 ST87_RESETn_Pin

#define DEVICE_LOG_PREFIX                     "--> "
#define DEVICE_LOG_PREFIX_LENGTH              (4U)
#define ST87EC_FWD_ST87_TX_MSG(PBYTEBUFFER, LENGTH)  HAL_UART_Transmit(&huart2, (uint8_t *)DEVICE_LOG_PREFIX, DEVICE_LOG_PREFIX_LENGTH, 100); \
                                                     HAL_UART_Transmit(&huart2, PBYTEBUFFER, LENGTH, 100);

/* Debug spy UART Rx Macro: Copy of the ST87 to EC Lib received data */
#define HOST_LOG_PREFIX                       "<-- "
#define HOST_LOG_PREFIX_LENGTH                (4U)
#define ST87EC_FWD_ST87_RX_MSG(PBYTEBUFFER, LENGTH, EOLDETECTED)  HAL_UART_Transmit(&huart2, (uint8_t *)HOST_LOG_PREFIX, HOST_LOG_PREFIX_LENGTH, 100); \
                                                                  HAL_UART_Transmit(&huart2, PBYTEBUFFER, LENGTH, 100);

/****************************** ST87M01 HW configuration ****************************/
/*
 * Below you can redefine the hardware configuration parameters for the ST87M01 device.
 * The default configuration values are defined in the file "st87ec_cold_config.h",
 * but you have the option to override them here.
 * To avoid writing to the ST87M01’s non-volatile memory (NVM) on every boot, the parameters
 * are only updated if the "ST87EC_COLD_CONFIG_VERSION" is different from the version
 * currently stored in the device’s NVM.
 *
 * So, if you need to modify any of the cold configuration parameters, you must also update
 * the "ST87EC_COLD_CONFIG_VERSION" value to ensure the new settings are saved correctly.
 */
//#define ST87EC_COLD_CONFIG_VERSION            6

//#define SLEEP_ENABLE                          1

#define UDP_DATA_TYPE                           1

#endif /* __ST87EC_LIB_CONF_H__ */
