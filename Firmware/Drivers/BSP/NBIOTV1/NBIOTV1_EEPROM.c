/**
******************************************************************************
* @file    NBIOTV1_EEPROM.c
* @author  SRA (based on work from SRA-SAIL)
* @brief   This file provides X-NUCLEO-BSP Board
*          specific functions refactored for the NBIOTV1 BSP
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
#include "NBIOTV1_EEPROM.h"

/** @addtogroup BSP
* @{
*/

/* -------- Static functions prototypes --------------------------------------------- */
static M95P32_Drv_t *M95P32Drv[BSP_M95P32_INSTANCES_NBR];
void *M95P32CompObj[BSP_M95P32_INSTANCES_NBR];
static int32_t M95P32_Probe(void);

#ifdef USE_SPI
/**
  * @brief  This function gives high on selected control pin
  * @param  None
  * @retval None
  */  
void EEPROMEX_CTRL_HIGH(void)
{
  HAL_GPIO_WritePin(BSP_M95P32_CS_PORT, BSP_M95P32_CS_PIN, GPIO_PIN_SET );
}

/**
  * @brief  This function gives low on selected control pin
  * @param  None
  * @retval None
  */  
void EEPROMEX_CTRL_LOW(void)
{
  HAL_GPIO_WritePin(BSP_M95P32_CS_PORT, BSP_M95P32_CS_PIN, GPIO_PIN_RESET );
}

#endif /* USE_SPI */

/**
  * @brief  Initializes the SPI EEPROMs
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_Init(uint32_t Instance)
{
  int32_t ret = M95_OK;
  
  switch (Instance)
  {
  case BSP_M95P32_0:
    if (M95P32_Probe() != M95_OK)
    {
      return M95_ERROR;
    }
    break;  
  default:
    ret = M95_ERROR;
    break;
  }
  
  return ret;  
}

/**
 * @brief  Function pointer to WRITE_ENABLE function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_WriteEnable(uint32_t Instance)
{
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif 
  
  ret = (M95P32Drv[Instance]->WriteEnable(M95P32CompObj[Instance]));

#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret; 
}

/**
 * @brief  Function pointer to WRITE_DISABLE function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_WriteDisable(uint32_t Instance)
{
  int32_t ret = M95_OK;

#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->WriteDisable(M95P32CompObj[Instance]));

#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret; 
}

/**
  * @brief  Function pointer to Read_StatusReg function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read          
  * @retval BSP status
  */
int32_t BSP_EEPROM_StatusRegRead(uint32_t Instance, uint8_t * pData)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], pData));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret; 
  
}

/**
  * @brief  Function pointer to Write_StatusConfigReg function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  Size : number of bytes to write           
  * @retval BSP status
  */
int32_t BSP_EEPROM_StatusConfigRegWrite(uint32_t Instance, uint8_t * pData, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->StatusConfigRegWrite(M95P32CompObj[Instance], pData, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 
  
  return ret; 
}

/**
  * @brief  Function pointer to Single_Read function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read    
  * @retval BSP status
  */
int32_t BSP_EEPROM_ReadPage(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->ReadPage(M95P32CompObj[Instance], pData, TarAddr, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif  
  
  return ret;
}

/**
  * @brief  Function pointer to FAST_Read function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read    
  * @retval BSP status
  */
int32_t BSP_EEPROM_FastRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->FastRead(M95P32CompObj[Instance], pData, TarAddr, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
}

/**
  * @brief  Function pointer to FAST_DRead function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read    
  * @retval BSP status
  */
int32_t BSP_EEPROM_FastDRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
  ret = (M95P32Drv[Instance]->FastDRead(M95P32CompObj[Instance], pData, TarAddr, Size));
  
  return ret;
}

/**
  * @brief  Function pointer to FAST_QRead function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read    
  * @retval BSP status
  */
int32_t BSP_EEPROM_FastQRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
  ret = (M95P32Drv[Instance]->FastQRead(M95P32CompObj[Instance], pData, TarAddr, Size));
  
  return ret;
}

/**
  * @brief  Function pointer to Page_Write function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  TarAddr : starting address to write
  * @param  Size : number of bytes to write           
  * @retval BSP status
  */
int32_t BSP_EEPROM_WritePage(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint32_t Size)
{
  int32_t ret = M95_OK;
  uint32_t remainingSize = Size;
  uint32_t targetAddress = TarAddr;
  uint8_t statusReg = 0;
  uint32_t bytesToWrite;
      
  /* Calculate the starting page and offset */
  uint32_t startOffset = TarAddr % M95P32_PAGESIZE;
  uint32_t offset = startOffset;
      
      
  /* Check for invalid inputs */
  if((M95P32CompObj[Instance] == NULL) || (pData == NULL) || (M95P32_PAGESIZE == 0U) || (remainingSize == 0U))
  {
    return M95_ERROR;
  }
      
  /* Check WIP status bit*/
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
      
  do
  {
    ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
  }while((statusReg & 0x01U) != 0U);
      
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
#endif  
      
  /* Iterate over the pages and write the data */
  while(remainingSize > 0U) 
  {
    bytesToWrite = (remainingSize < (M95P32_PAGESIZE - offset)) ? remainingSize : (M95P32_PAGESIZE - offset);
        
#ifdef USE_SPI
    EEPROMEX_CTRL_LOW();
#endif
	
    ret = (M95P32Drv[Instance]->WriteEnable(M95P32CompObj[Instance]));
    
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
#endif    

#ifdef USE_SPI
    EEPROMEX_CTRL_LOW();
#endif
    
    ret = (M95P32Drv[Instance]->WritePage(M95P32CompObj[Instance], pData, targetAddress, bytesToWrite));

#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 

    if(ret == M95_OK)
    {
      /* Update the pointers and sizes for the next page */
      pData += bytesToWrite;
      remainingSize -= bytesToWrite;
      targetAddress += bytesToWrite;
      offset = targetAddress % M95P32_PAGESIZE;
          
      /* Check WIP status bit*/
#ifdef USE_SPI
      EEPROMEX_CTRL_LOW();
#endif
          
      do
      {
        ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
      }while((statusReg & 0x01U) != 0U);
          
#ifdef USE_SPI
      EEPROMEX_CTRL_HIGH();
#endif
    }
    else
    {
      ret = M95_ERROR;
      break;
    }	
        
  }
      
  return ret; 
}
    
/**
  * @brief  Function pointer to Page_Prog function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  TarAddr : starting address to write
  * @param  Size : number of bytes to write     
  * @retval BSP status
  */
int32_t BSP_EEPROM_ProgramPage(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  uint32_t remainingSize = Size;
  uint32_t targetAddress = TarAddr;
  uint8_t statusReg = 0;
  uint32_t bytesToWrite;
      
  /* Calculate the starting page and offset */
  uint32_t startOffset = TarAddr % M95P32_PAGESIZE;
  uint32_t offset = startOffset;
      
      
  /* Check for invalid inputs */
  if((M95P32CompObj[Instance] == NULL) || (pData == NULL) || (M95P32_PAGESIZE == 0U) || (remainingSize == 0U))
  {
    return M95_ERROR;
  }
      
  /* Check WIP status bit*/
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
      
  do
  {
    ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
  }while((statusReg & 0x01U) != 0U);
      
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
#endif  
      
  /* Iterate over the pages and write the data */
  while(remainingSize > 0U) 
  {
    bytesToWrite = (remainingSize < (M95P32_PAGESIZE - offset)) ? remainingSize : (M95P32_PAGESIZE - offset);
        
#ifdef USE_SPI
    EEPROMEX_CTRL_LOW();
#endif
	
    ret = (M95P32Drv[Instance]->WriteEnable(M95P32CompObj[Instance]));
    
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
#endif    

#ifdef USE_SPI
    EEPROMEX_CTRL_LOW();
#endif
    
    ret = (M95P32Drv[Instance]->ProgramPage(M95P32CompObj[Instance], pData, targetAddress, bytesToWrite));

#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 

    if(ret == M95_OK)
    {
      /* Update the pointers and sizes for the next page */
      pData += bytesToWrite;
      remainingSize -= bytesToWrite;
      targetAddress += bytesToWrite;
      offset = targetAddress % M95P32_PAGESIZE;
          
      /* Check WIP status bit*/
#ifdef USE_SPI
      EEPROMEX_CTRL_LOW();
#endif
          
      do
      {
        ret = (M95P32Drv[Instance]->StatusRegRead(M95P32CompObj[Instance], &statusReg));
      }while((statusReg & 0x01U) != 0U);
          
#ifdef USE_SPI
      EEPROMEX_CTRL_HIGH();
#endif
    }
    else
    {
      ret = M95_ERROR;
      break;
    }	
        
  }
      
  return ret; 
}

/**
  * @brief  Function pointer to Page_Erase function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  TarAddr : starting address of page           
  * @retval BSP status
  */
int32_t BSP_EEPROM_ErasePage(uint32_t Instance, uint32_t TarAddr)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->ErasePage(M95P32CompObj[Instance], TarAddr));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif   
  
  return ret;
}

/**
  * @brief  Function pointer to Sector_Erase function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  TarAddr : starting address of sector
  * @retval BSP status
  */
int32_t BSP_EEPROM_EraseSector(uint32_t Instance, uint32_t TarAddr)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->EraseSector(M95P32CompObj[Instance], TarAddr));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 
  
  return ret;
}

/**
  * @brief  Function pointer to Block_Erase function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  TarAddr : starting address of block
  * @retval BSP status
  */
int32_t BSP_EEPROM_EraseBlock(uint32_t Instance, uint32_t TarAddr)
{  
  int32_t ret = M95_OK;
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif  
  ret = (M95P32Drv[Instance]->EraseBlock(M95P32CompObj[Instance], TarAddr));

#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 
  
  return ret;
}

/**
  * @brief  Function pointer to Chip_Erase function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_EraseChip(uint32_t Instance)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->EraseChip(M95P32CompObj[Instance]));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 
  
  return ret;
}

/**
  * @brief  Function pointer to Read_ID function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read    
  * @retval BSP status
  */
int32_t BSP_EEPROM_ReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->ReadID(M95P32CompObj[Instance], pData, TarAddr, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Function pointer to FAST_Read_ID function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : Number of bytes to read             
  * @retval BSP status
  */
int32_t BSP_EEPROM_FastReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->FastReadID(M95P32CompObj[Instance], pData, TarAddr, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Function pointer to Write_ID function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to the data to write
  * @param  TarAddr : starting address to write
  * @param  Size : number of bytes to write         
  * @retval BSP status
  */
int32_t BSP_EEPROM_WriteID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->WriteID(M95P32CompObj[Instance], pData, TarAddr, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 
  
  return ret;
  
}

/**
 * @brief  Function pointer to Deep_Power_Down function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_DeepPowerDown(uint32_t Instance)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->DeepPowerDown(M95P32CompObj[Instance]));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
 * @brief  Function pointer to Deep_Power_Down_Release function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_DeepPowerDownRel(uint32_t Instance)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->DeepPowerDownRel(M95P32CompObj[Instance]));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
 * @brief  Function pointer to Read_JEDEC function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @param  pData : pointer to store data read
 * @param  Size : number of bytes to read    
 * @retval BSP : status
 */
int32_t BSP_EEPROM_JEDECRead(uint32_t Instance, uint8_t *pData, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->JEDECRead(M95P32CompObj[Instance], pData, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Function pointer to ReadConfigReg function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  Size : number of bytes to read            
  * @retval BSP status
  */
int32_t BSP_EEPROM_ConfSafetyRegRead(uint32_t Instance, uint8_t *pData, uint8_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->ConfSafetyRegRead(M95P32CompObj[Instance], pData, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
 * @brief  Function pointer to ReadVolatileReg function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @param  pData : pointer to store data read
 * @retval BSP status
 */
int32_t BSP_EEPROM_VolRegRead(uint32_t Instance, uint8_t *pData)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->VolRegRead(M95P32CompObj[Instance], pData));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
}

/**
 * @brief  Function pointer to WriteVolatileRegister function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @param  regVal : register value to write            
 * @retval BSP status
 */
int32_t BSP_EEPROM_VolRegWrite(uint32_t Instance, uint8_t regVal)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->VolRegWrite(M95P32CompObj[Instance], regVal));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();
  BSP_M95P32_DELAY(5);
  
  EEPROMEX_CTRL_LOW();
  ret = Transmit_Data_polling(M95P32CompObj[Instance]);
  EEPROMEX_CTRL_HIGH();
#endif 
  
  return ret;
}

/**
 * @brief  Function pointer to ClearSafetyFlag function in BSP driver structure
 * @param  Instance : SPI EEPROMs instance to be used
 * @retval BSP status
 */
int32_t BSP_EEPROM_ClearSafetyFlag(uint32_t Instance)
{
  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->ClearSafetyFlag(M95P32CompObj[Instance]));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Function pointer to Read_StatusReg function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @param  pData : pointer to store data read
  * @param  TarAddr : starting address to read
  * @param  Size : number of bytes to read
  * @retval BSP status
  */
int32_t BSP_EEPROM_SFDPRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint32_t Size)
{ 
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->SFDPRegRead(M95P32CompObj[Instance], pData, TarAddr, Size));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Function pointer to Reset_Enable function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_EnableReset(uint32_t Instance)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->EnableReset(M95P32CompObj[Instance]));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Function pointer to Soft_Reset function in BSP driver structure
  * @param  Instance : SPI EEPROMs instance to be used
  * @retval BSP status
  */
int32_t BSP_EEPROM_SoftReset(uint32_t Instance)
{  
  int32_t ret = M95_OK;
  
#ifdef USE_SPI
  EEPROMEX_CTRL_LOW();
#endif
  
  ret = (M95P32Drv[Instance]->SoftReset(M95P32CompObj[Instance]));
  
#ifdef USE_SPI
  EEPROMEX_CTRL_HIGH();  
#endif
  
  return ret;
  
}

/**
  * @brief  Register Bus IOs for instance M95P32 if component ID is OK
  * @param  None
  * @retval BSP status
  */
static int32_t M95P32_Probe(void)
{
  M95_IO_t io_ctxm95p32;
  int32_t ret = M95_OK;
  static M95_Object_t M95P32_obj_0;
  
  GPIO_InitTypeDef GPIO_InitStruct;

  BSP_M95P32_CS_GPIO_CLK_ENABLE();

  /* Configure CS pin */
  HAL_GPIO_WritePin(BSP_M95P32_CS_PORT, BSP_M95P32_CS_PIN, GPIO_PIN_SET);

  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;

  GPIO_InitStruct.Pin = BSP_M95P32_CS_PIN;
  HAL_GPIO_Init(BSP_M95P32_CS_PORT, &GPIO_InitStruct);

  io_ctxm95p32.Init                = BSP_M95P32_SPI_INIT;
  io_ctxm95p32.DeInit              = BSP_M95P32_SPI_DEINIT;
  io_ctxm95p32.Delay               = BSP_M95P32_DELAY;
  
#ifdef USE_SPI
  io_ctxm95p32.Read                = BSP_M95P32_SPI_RECV;
  io_ctxm95p32.Write               = BSP_M95P32_SPI_SEND;
  io_ctxm95p32.SendRecv            = BSP_M95P32_SPI_SENDRECV;
#endif
  
  if (M95P32_RegisterBusIO(&M95P32_obj_0,&io_ctxm95p32) != M95_OK)
  {
    ret = M95_ERROR;
  }
  
  M95P32CompObj[BSP_M95P32_0] = &M95P32_obj_0;
  M95P32Drv[BSP_M95P32_0] = (M95P32_Drv_t *)(void *)&M95P32_spi_Drv;
  
  if (M95P32Drv[BSP_M95P32_0]->Init(M95P32CompObj[BSP_M95P32_0]) != M95_OK)
  {
    ret = M95_ERROR;
  }
  else
  {
    ret = M95_OK;
  }  
  return ret;
}

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

