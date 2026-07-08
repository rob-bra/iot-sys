/**
  ******************************************************************************
  * @file    st87ec_debug.h
  * @author  APMS Application Team
  * @brief   Interface header file
  *
  @verbatim
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics International N.V.
  * All rights reserved.
  *
  ******************************************************************************
  */
#ifndef ST87EC_DEBUG_H
#define ST87EC_DEBUG_H



/* Private constants and macros-----------------------------------------------*/
#include <string.h>


/* Debug trace macro to be implemented as a "printf" function */
/* Parameter formatted as variadic : can be retrieved using __VA_ARGS__ */
#ifndef ST87EC_TRACE
#define ST87EC_TRACE(...)
#endif /*ST87EC_TRACE*/

/* Debug spy UART Tx Macro: Copy of the EC Lib to ST87 sent data */
/* PBYTEBUFFER: pointer on data payload */
/* LENGTH: data payload size in byte */
#ifndef ST87EC_FWD_ST87_TX_MSG
#define ST87EC_FWD_ST87_TX_MSG(PBYTEBUFFER, LENGTH)
#endif /*ST87EC_FWD_ST87_TX_MSG*/

/* Debug spy UART Rx Macro: Copy of the ST87 to EC Lib received data */
/* PBYTEBUFFER: pointer on data payload */
/* LENGTH: data payload size in byte */
/* EOLDETECTED: Set to one if End of line character detected */
#ifndef ST87EC_FWD_ST87_RX_MSG
#define ST87EC_FWD_ST87_RX_MSG(PBYTEBUFFER, LENGTH, EOLDETECTED)
#endif /*ST87EC_FWD_ST87_RX_MSG*/



/* Types ---------------------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/


#endif /* ST87EC_DEBUG_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
