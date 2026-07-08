/**
  ******************************************************************************
  * @file    st87ec_cold_config.h
  * @author  APMS Application Team
  * @brief   Cold configuration header file
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
#ifndef ST87EC_COLD_CONFIG_H
#define ST87EC_COLD_CONFIG_H

/* Private constants and macros-----------------------------------------------*/
#ifndef ST87EC_COLD_CONFIG_VERSION
#define ST87EC_COLD_CONFIG_VERSION     6
#endif

/****************************** HW configuration ****************************/
/* Temperature limit */
#ifndef TEMP_LOW_SHUTDOWN
#define TEMP_LOW_SHUTDOWN  -45   /*!< Signed integer type Temperature low threshold in Celsius degree for shutdown display and shutdown */
#endif

#ifndef TEMP_HIGH_SHUTDONW
#define TEMP_HIGH_SHUTDONW 110   /*!< Signed integer type Temperature high threshold in Celsius degree for shutdown display and shutdown */
#endif

#ifndef TEMP_SHUTDOWN
#define TEMP_SHUTDOWN      1     /*!< Integer type 0: Disable shutdown if the shutdown threshold is reached
                                                 1: Enable shutdown if the shutdown threshold is reached */
#endif

/* Battery level settings */
#ifndef VBAT_LOW_SHUTDOWN
#define VBAT_LOW_SHUTDOWN  2000 /*!< Integer type Low battery threshold in mV for shutdown display and shutdown */
#endif

#ifndef VBAT_HIGH_SHUTDOWN
#define VBAT_HIGH_SHUTDOWN 3200 /*!< Integer type High battery threshold in mV for shutdown display and shutdown */
#endif

#ifndef VBAT_SHUTDOWN
#define VBAT_SHUTDOWN      1    /*!< Integer type 0: Disable shutdown if the shutdown threshold is reached
                                                1: Enable shutdown if the shutdown threshold is reached */
#endif

/****************************** NBIOT configuration ****************************/
/* Band selection : configure the band usage and split between various NMO */
#ifndef BANDLIST
#define BANDLIST      "20,8"              /*!< Band selected comma separated  */
#endif

#ifndef BANDCFG
#define BANDCFG       "0,0,20,01,7910"    /*!< Band=20, Option=01, StartFreq=7910 */
#endif

#ifndef BANDCFG_NMO1
#define BANDCFG_NMO1  "0,1,0,2,1,100,0"   /*!< Pref=0, Guard=2, In=1, BW=100, OffsetFreq=0 */
#endif

#ifndef BANDCFG_NMO2
#define BANDCFG_NMO2  "0,2,0,2,1,100,100" /*!< Pref=0, Guard=2, In=1, BW=100, OffsetFreq=100 */
#endif

#ifndef BANDCFG_NMO3
#define BANDCFG_NMO3  "0,3,0,2,1,100,200" /*!< Pref=0, Guard=2, In=1, BW=100, OffsetFreq=200 */
#endif

/* EDRX Setting */
#ifndef EDRX_VALUE
#define EDRX_VALUE    1011       /*!< Requested EDRX value "1011" -> 655.36s See 3GPP 24.008 Table 10.5.5.32 */
#endif

/* Paging Time Window */
#ifndef PTW_VALUE
#define PTW_VALUE     0011       /*!< PTW value "0011" -> 10.24s See 3GPP TS 24.008 Table 10.5.5.32 */
#endif

/* Power saving mode setting */
#ifndef PSM_ENABLE
#define PSM_ENABLE    1
#endif

#ifndef PERIODIC_TAU
#define PERIODIC_TAU  00100001  /*!< TAU value (T3412) "00100001" -> 1H See 3GPP TS 24.008 Table 10.5.5.32 */
                                /*!< TAU value (T3412) "00111000" -> 24H See 3GPP TS 24.008 Table 10.5.5.32 */
#endif

#ifndef ACTIVE_TIME
#define ACTIVE_TIME   00000101  /*!< Active time (T3324) "00000101" */
#endif

/* IP configuration */
#ifndef NB_PACKET_SENT_ENABLE
#define NB_PACKET_SENT_ENABLE  1  /*!< Activation of the counting of the number of UDP packets actually received and acknowledged by the eNodeB.
                                       However, it does not guarantee that the packet has been received by the remote server.
                                       If NB_PACKET_SENT is 1, counting is active.
                                          NbUdpPacketsSent counter var in EC Lib State structure (ST87EC_Lib_Status_t) will reflect the counting.
                                       If NB_PACKET_SENT is 0, counting is inactive.
                                          NbUdpPacketsSent counter var in EC Lib State structure (ST87EC_Lib_Status_t) keeps 0 value.           */
#endif

#ifndef DOMAIN_NAME
#define DOMAIN_NAME "8.8.8.8"    /*!< IP address for DNS resolution    */
#endif

/********************************* LWM2M parameters *****************************/
#ifndef LWM2M_CLIENT_NAME
#define LWM2M_CLIENT_NAME  ""      /*!< LwM2M client name (String type) */
#endif

#ifndef LWM2M_SERVER_ID
#define LWM2M_SERVER_ID    1       /*!< LwM2M server Id (Integer type)  */
#endif

#ifndef LWM2M_SERVER_URL
#define LWM2M_SERVER_URL  ""       /*!< Server URL (String type)
                                      scheme ":" ["//" authority] path ["?" query] ["#" fragment]
                                      authority = [userinfo "@"] host [":" port]    */
#endif
#ifndef LWM2M_LIFETIME
#define LWM2M_LIFETIME    120      /*!< Lifetime in seconds (Integer type) */
#endif

/*************************** WIFI scanning configuration ************************/
#ifndef URC_MODE
#define URC_MODE        1  /*!< URC mode To avoid too much data coming again and again on UART, the Host has the possibility to set a filter mode
                                 to display only one time the BSSID found during the full time of Wi-Fi scanning.
                                 Possible values: 0: Only the new SSID after one loop (Default mode) 1: In infinite loop URC */
#endif

#ifndef HOPPING_TIME
#define HOPPING_TIME 1024  /*!< Hopping time is the scan duration in ms before swapping to next channel  */
#endif

#ifndef ANT_SEL
#define ANT_SEL         0  /*!< Selects the suitable ST87 input for antenna (GNSS or NB-IOT).
                                 Possible values: 0: GNSS ant. input (Default mode) 1: NB-IOT ant. input */
#endif

/****************************** ST87 configuration ****************************/
/* Sleep mode configuration */
#ifndef SLEEP_ENABLE
#define SLEEP_ENABLE   1   /*!< Allow ST87 to sleep (especially between 2 EC Lib sequences)  */
#endif

#ifndef HOLD_TIME
#define HOLD_TIME     10   /*!< Integer type Time in seconds between the last AT command and the sleep mode entry */
#endif

#ifndef AWAKE_TIME
#define AWAKE_TIME     0   /*!< Integer type Define the timeout in seconds that the module is awake at each wake up (telecom activity, AT command activity..) */
#endif

/* Parameters to be configured for Ring pin setup */
#ifndef RING_PIN_ENABLE
#define RING_PIN_ENABLE      0           /*!< Ring pin enable */
#endif

#ifndef RING_PIN_GPIO
#define RING_PIN_GPIO        10          /*!< Ring pin number to be set for ST87M01. The GPIO number between 8 and 31                    */
#endif

#ifndef RING_PIN_POLARITY
#define RING_PIN_POLARITY    1           /*!< Ring pin voltage polarity (0: active low and 1: active high).                              */
#endif

#ifndef RING_PIN_DELAY
#define RING_PIN_DELAY       200         /*!< The time in ms when ring pin is active (min value: 10ms and max value:300ms by 10ms steps) */
#endif

#endif /* ST87EC_COLD_CONFIG_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
