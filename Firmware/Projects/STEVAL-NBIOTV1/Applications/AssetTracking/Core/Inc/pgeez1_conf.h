/**
  ******************************************************************************
  * @file           : pgeez1_conf.h
  * @author     : SRA Application Team
  * @brief      : This file contains definitions for the PGEEZ1 components bus interfaces
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
#ifndef __PGEEZ1_CONF_H__
#define __PGEEZ1_CONF_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "pgeez1_spi.h"

/* Global configuration ---------------------------------------------------*/

/* Defines ---------------------------------------------------------------*/
#define SERIAL_COM_INIT           PGEEZ1_SPI1_Init
#define SERIAL_COM_DEINIT         PGEEZ1_SPI1_DeInit
#define SERIAL_COM_RECV           PGEEZ1_SPI1_Recv
#define SERIAL_COM_SEND           PGEEZ1_SPI1_Send
#define SERIAL_COM_SENDRECV       PGEEZ1_SPI1_SendRecv
#define PGEEZ1_M95_DELAY          HAL_Delay

#define USE_SPI
#define SPI_INSTANCE              hspi1

#define M95P32_EEPROM_SPI_CS_PORT M95_CS_GPIO_Port
#define M95P32_EEPROM_SPI_CS_PIN  M95_CS_Pin


#ifdef __cplusplus
}
#endif

#endif /* __PGEEZ1_CONF_H__*/

