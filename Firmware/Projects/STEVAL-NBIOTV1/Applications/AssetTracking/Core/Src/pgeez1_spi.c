#include "pgeez1_spi.h"

int32_t PGEEZ1_SPI1_Init(void)
{
  int32_t ret = PGEEZ1_ERROR_NONE;

  // nothing to do here
  // initialization done in main.c

  return ret;
}

int32_t PGEEZ1_SPI1_DeInit(void)
{
  int32_t ret = PGEEZ1_ERROR_NONE;

  // nothing to do here

  return ret;
}

int32_t PGEEZ1_SPI1_Send(uint8_t *pData, uint16_t Length)
{
  int32_t ret = PGEEZ1_ERROR_NONE;

  if(HAL_SPI_Transmit(&hspi1, pData, Length, PGEEZ1_SPI1_POLL_TIMEOUT) != HAL_OK)
  {
      ret = PGEEZ1_ERROR_UNKNOWN_FAILURE;
  }

  return ret;
}

int32_t PGEEZ1_SPI1_Recv(uint8_t *pData, uint16_t Length)
{
  int32_t ret = PGEEZ1_ERROR_NONE;

  if(HAL_SPI_Receive(&hspi1, pData, Length, PGEEZ1_SPI1_POLL_TIMEOUT) != HAL_OK)
  {
      ret = PGEEZ1_ERROR_UNKNOWN_FAILURE;
  }

  return ret;
}

int32_t PGEEZ1_SPI1_SendRecv(uint8_t *pTxData, uint8_t *pRxData, uint16_t Length)
{
  int32_t ret = PGEEZ1_ERROR_NONE;

  if(HAL_SPI_TransmitReceive(&hspi1, pTxData, pRxData, Length, PGEEZ1_SPI1_POLL_TIMEOUT) != HAL_OK)
  {
      ret = PGEEZ1_ERROR_UNKNOWN_FAILURE;
  }

  return ret;
}
