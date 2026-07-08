/**
******************************************************************************
* @file    NBIOTV1_EEPROM.h
* @author  SRA (based on work from SRA-SAIL)
* @brief   header of NBIOTV1 board based on PGEEZ1 expansion board with M95P32 series EEPROM header
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
#ifndef NBIOTV1_EEPROM_H
#define NBIOTV1_EEPROM_H

#ifdef __cplusplus
 extern "C" {
#endif

   
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "m95p32.h"
#include "NBIOTV1.h"
   
/* Exported macro ------------------------------------------------------------*/
#define BSP_M95P32_INSTANCES_NBR     1U
#define BSP_M95P32_0                 0U
   
#define EEPROMEX_SLAVE_CS_PIN          GPIO_PIN_5
#define EEPROMEX_SLAVE_CS_PIN_PORT     GPIOB

#define EEPROMEX_SPI_SLAVE_FOUR_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()

#ifdef  USE_SPI
   void EEPROMEX_CTRL_HIGH(void);
   void EEPROMEX_CTRL_LOW(void);
#endif

int32_t M95P32_RegisterBusIO(M95_Object_t *pObj, M95_IO_t *pIO);

int32_t BSP_EEPROM_Init(uint32_t Instance);
int32_t BSP_EEPROM_WriteEnable(uint32_t Instance);
int32_t BSP_EEPROM_WriteDisable(uint32_t Instance);
int32_t BSP_EEPROM_StatusRegRead(uint32_t Instance, uint8_t * pData);
int32_t BSP_EEPROM_StatusConfigRegWrite(uint32_t Instance, uint8_t * pData, uint32_t Size);
int32_t BSP_EEPROM_WritePage(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,  uint32_t Size);
int32_t BSP_EEPROM_ProgramPage(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_ReadPage(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,  uint32_t Size);
int32_t BSP_EEPROM_FastRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_FastDRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_FastQRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_ErasePage(uint32_t Instance, uint32_t TarAddr);
int32_t BSP_EEPROM_EraseSector(uint32_t Instance, uint32_t TarAddr);
int32_t BSP_EEPROM_EraseBlock(uint32_t Instance, uint32_t TarAddr);
int32_t BSP_EEPROM_EraseChip(uint32_t Instance);
int32_t BSP_EEPROM_ReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_FastReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_WriteID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_DeepPowerDown(uint32_t Instance);
int32_t BSP_EEPROM_DeepPowerDownRel(uint32_t Instance);
int32_t BSP_EEPROM_VolRegRead(uint32_t Instance, uint8_t *pData);
int32_t BSP_EEPROM_VolRegWrite(uint32_t Instance, uint8_t regVal);
int32_t BSP_EEPROM_ConfSafetyRegRead(uint32_t Instance, uint8_t *pData, uint8_t Size);
int32_t BSP_EEPROM_ClearSafetyFlag(uint32_t Instance);
int32_t BSP_EEPROM_SFDPRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t BSP_EEPROM_JEDECRead(uint32_t Instance, uint8_t *pData, uint32_t Size);
int32_t BSP_EEPROM_EnableReset(uint32_t Instance);
int32_t BSP_EEPROM_SoftReset(uint32_t Instance);

#ifdef __cplusplus
  }
#endif
#endif /* NBIOTV1_EEPROM_H */

