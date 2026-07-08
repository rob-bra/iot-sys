/**
 ******************************************************************************
 * @file    NBIOTV1_bc.h
 * @author  SRA
 * @brief   This file contains definitions for NBIOTV1_bc.c
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
#ifndef NBIOTV1_BC_H
#define NBIOTV1_BC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "NBIOTV1.h"

#define BC_BATTERY_MAX_VOLTAGE 4225
#define BC_BATTERY_MIN_VOLTAGE 3250
#define WINDOW_VOLTAGE_DIM 16

#define NBIOTV1_BC_ADC_VOLTAGE_DIVIDER_R1 (27.4f) // kOhms
#define NBIOTV1_BC_ADC_VOLTAGE_DIVIDER_R2 (100.0f) // kOhms

#define STBC02_GetTick()                    HAL_GetTick()  //!< Get SysTick macro
#define STBC02_Delay(delay)                 HAL_Delay(delay)

#define STBC02_SW_50US                      (float_t)50e-6 // s
#define STBC02_SW_TIM                       TIM16
#define STBC02_SW_TIM_CLK_ENABLE()          __HAL_RCC_TIM16_CLK_ENABLE()
#define STBC02_SW_TIM_CLK_DISABLE()         __HAL_RCC_TIM16_CLK_DISABLE()

#define BC_SW_TIM_IRQn                      TIM16_IRQn
#define BC_Sw_TIM_IRQHandler                TIM16_IRQHandler

#ifndef BSP_BC_SW_TIM_IT_PRIORITY
#define BSP_BC_SW_TIM_IT_PRIORITY           3U
#endif

#define STBC02_SW_PIN                       GPIO_PIN_8
#define STBC02_SW_GPIO_PORT                 GPIOA
#define STBC02_SW_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()
#define STBC02_SW_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOA_CLK_DISABLE()

#define STBC02_CHG_PIN                      GPIO_PIN_14
#define STBC02_CHG_GPIO_PORT                GPIOB
#define STBC02_CHG_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define STBC02_CHG_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()

#define STBC02_CE_PIN                      GPIO_PIN_12
#define STBC02_CE_GPIO_PORT                GPIOB
#define STBC02_CE_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define STBC02_CE_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOB_CLK_DISABLE()
#define STBC02_CE_REENABLE_DURATION        (100U) // ms


typedef enum
{
  NotValidInput = 0,
  ValidInput,
  VbatLow,
  EndOfCharge,
  ChargingPhase,
  OverchargeFault,
  ChargingTimeout,
  BatteryVoltageBelowVpre,
  ChargingThermalLimitation,
  BatteryTemperatureFault
} stbc02_ChgState_TypeDef;

typedef enum
{
  SW1_OA_OFF = 1,
  SW1_OA_ON = 2,
  SW1_OB_OFF = 3,
  SW1_OB_ON = 4,
  SW2_OA_OFF = 5,
  SW2_OA_ON = 6,
  SW2_OB_OFF = 7,
  SW2_OB_ON = 8,
  BATMS_OFF = 9,
  BATMS_ON = 10,
  IEND_OFF = 11,
  IEND_5_PC_IFAST = 12,
  IEND_2_5_PC_IFAST = 13,
  IBAT_OCP_900_mA = 14,
  IBAT_OCP_450_mA = 15,
  IBAT_OCP_250_mA = 16,
  IBAT_OCP_100_mA = 17,
  VFLOAT_ADJ_OFF = 18,
  VFLOAT_ADJ_PLUS_50_mV = 19,
  VFLOAT_ADJ_PLUS_100_mV = 20,
  VFLOAT_ADJ_PLUS_150_mV = 21,
  VFLOAT_ADJ_PLUS_200_mV = 22,
  SHIPPING_MODE_ON = 23,
  AUTORECH_OFF = 24,
  AUTORECH_ON = 25,
  WATCHDOG_OFF = 26,
  WATCHDOG_ON = 27,
  IFAST_IPRE_50_PC_OFF = 28,
  IFAST_IPRE_50_PC_ON = 29
} stbc02_SwCmd_TypeDef;

typedef enum
{
  idle,
  start,
  pulse_l,
  pulse_h,
  stop_l,
  stop_h,
  wait
} stbc02_SwState_TypeDef;

typedef struct
{
  char *name;
  float_t freq;
} stbc02_ChgStateNameAndFreq_TypeDef;

typedef struct
{
  stbc02_ChgState_TypeDef Id;
  uint8_t Name[32];
} stbc02_State_TypeDef;

extern TIM_HandleTypeDef hstbc02_SWTim;

/* Public Function_Prototypes -----------------------------------------------*/

/* API for Single Wire protocol interface (STBC02 control interface) */
void    BSP_BC_Sw_Init(void);
int32_t BSP_BC_Sw_CmdSend(stbc02_SwCmd_TypeDef stbc02_SwCmd);

/* API for CHG pin frequency detection (STBC02 Status) */
int32_t BSP_BC_Chg_Init(void);
int32_t BSP_BC_Chg_DeInit(void);
void    BSP_BC_ChgPinHasToggled(TIM_HandleTypeDef *htim);
void    BSP_BC_GetState(stbc02_State_TypeDef *BC_State);
int32_t BSP_BC_IsChgPinToggling(void);

/* API for Charger Enable pin */
int32_t BSP_BC_ChargerEnableInit(void);
int32_t BSP_BC_ChargerEnableDeInit(void);
int32_t BSP_BC_ChargerReEnable(void);

/* API for Battery Voltage monitoring */
int32_t BSP_BC_BatMs_Init(void);
int32_t BSP_BC_BatMs_DeInit(void);
int32_t BSP_BC_GetVoltage(uint32_t *BatteryVoltage);
int32_t BSP_BC_GetVoltageAndLevel(uint32_t *BatteryVoltage, uint32_t *BatteryLevel);

void STBC02_CHG_PIN_Callback(void);
void STBC02_CHG_EXTI_IRQHandler(void);
void TIM15_IRQHandler(void);

#ifdef __cplusplus
}
#endif
#endif /* NBIOTV1_BC_H */
