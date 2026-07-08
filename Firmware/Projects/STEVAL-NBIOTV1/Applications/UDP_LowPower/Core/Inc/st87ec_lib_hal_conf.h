#ifndef ST87EC_LIB_HAL_CONF_H
#define ST87EC_LIB_HAL_CONF_H

#include <stdint.h>
#include <stdbool.h>

#include "usart.h"

#include "stm32u5xx_ll_usart.h"

#include "st87ec_lib.h"

// #define ST87_UART_HANDLE (&huart2) // on L4
#define ST87_UART_HANDLE (&hlpuart1)

#endif // ST87EC_LIB_HAL_CONF_H
