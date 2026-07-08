#ifndef __PGEEZ1_SPI_H__
#define __PGEEZ1_SPI_H__

#include "spi.h"

#define PGEEZ1_ERROR_NONE                    0
#define PGEEZ1_ERROR_NO_INIT                -1
#define PGEEZ1_ERROR_WRONG_PARAM            -2
#define PGEEZ1_ERROR_BUSY                   -3
#define PGEEZ1_ERROR_PERIPH_FAILURE         -4
#define PGEEZ1_ERROR_COMPONENT_FAILURE      -5
#define PGEEZ1_ERROR_UNKNOWN_FAILURE        -6
#define PGEEZ1_ERROR_UNKNOWN_COMPONENT      -7
#define PGEEZ1_ERROR_BUS_FAILURE            -8
#define PGEEZ1_ERROR_CLOCK_FAILURE          -9
#define PGEEZ1_ERROR_MSP_FAILURE            -10
#define PGEEZ1_ERROR_FEATURE_NOT_SUPPORTED  -11

#define PGEEZ1_SPI1_POLL_TIMEOUT  (1000U)

int32_t PGEEZ1_SPI1_Init(void);
int32_t PGEEZ1_SPI1_DeInit(void);
int32_t PGEEZ1_SPI1_Send(uint8_t *pData, uint16_t Length);
int32_t PGEEZ1_SPI1_Recv(uint8_t *pData, uint16_t Length);
int32_t PGEEZ1_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length);

#endif /* __PGEEZ1_SPI_H__*/
