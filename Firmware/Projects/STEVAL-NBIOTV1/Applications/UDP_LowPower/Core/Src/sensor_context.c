/**
  ******************************************************************************
  * @file           : sensor_context.c
  * @brief          : Sensor context initialization. It depends on MCU and board
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
#include "sensor_context.h"

/* Private includes ----------------------------------------------------------*/
#include "main.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  Bus_Sensor *sensor = (Bus_Sensor*) handle;

  if(sensor->AddressI2C == 0)
  {
    HAL_GPIO_WritePin(sensor->GPIOx, sensor->GPIO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(sensor->handle, &reg, 1, 1000);
    HAL_SPI_Transmit(sensor->handle, (uint8_t*) bufp, len, 1000);
    HAL_GPIO_WritePin(sensor->GPIOx, sensor->GPIO_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_I2C_Master_Transmit(sensor->handle, (uint16_t)sensor->AddressI2C, bufp, len, 1000);
  }

  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  Bus_Sensor *sensor = (Bus_Sensor*) handle;

  if(sensor->AddressI2C == 0)
  {
    reg |= 0x80;
    HAL_GPIO_WritePin(sensor->GPIOx, sensor->GPIO_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(sensor->handle, &reg, 1, 1000);
    HAL_SPI_Receive(sensor->handle, bufp, len, 1000);
    HAL_GPIO_WritePin(sensor->GPIOx, sensor->GPIO_Pin, GPIO_PIN_SET);
  }
  else
  {
    HAL_I2C_Master_Receive(sensor->handle, (uint16_t)sensor->AddressI2C, bufp, len, 1000);
  }

  return 0;
}

/**
 * @brief  Initialize the context for the sensor driver
 * @param  dev_ctx: Pointer to the device context structure to be initialized.
 * @param  handle: Pointer to a device-specific handle that will be used by the context.
 * @note   This function is responsible for initializing the sensor context by associating
 *         the device-specific handle with the sensor's device context. This setup is
 *         necessary for the sensor operations that rely on the context to communicate
 *         with the sensor hardware.
 */
void sensor_context_init(stmdev_ctx_t *dev_ctx, void *handle)
{
  /* Sensor driver context initialization: read, write and interface */
  dev_ctx->read_reg = platform_read;
  dev_ctx->write_reg = platform_write;
  dev_ctx->handle = handle;
  dev_ctx->mdelay = HAL_Delay;
}
