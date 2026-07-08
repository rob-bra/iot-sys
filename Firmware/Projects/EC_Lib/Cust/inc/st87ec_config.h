/**
  ******************************************************************************
  * @file    st87ec_config.h
  * @author  APMS Application Team
  * @brief   Configuration header file
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
#ifndef ST87EC_CONFIG_H
#define ST87EC_CONFIG_H

#include "st87ec_config_default.h"

/* Private constants and macros-----------------------------------------------*/

/* Parameters to be configured for internal timers */
#ifndef TIMER_NB
#define TIMER_NB                      10           /*!< Number of timers                                                                           */
#endif

#ifndef TIMER_TICK_PERIOD_MS
#define TIMER_TICK_PERIOD_MS          1            /*!< Tick period of timers in ms                                                                */
#endif

/* Parameters to be configured for the socket creation */
#ifndef SOCKET_SEND_TIMEOUT
#define SOCKET_SEND_TIMEOUT           60           /*!< Maximum time needed for transmission of packet over the air and reception of acknowledgment */
#endif

#ifndef SOCKET_RECEIVE_TIMEOUT
#define SOCKET_RECEIVE_TIMEOUT        60           /*!< Maximum time needed for transmission of packet over the air                                 */
#endif

#ifndef SOCKET_FRAME_RECEIVED_URC
#define SOCKET_FRAME_RECEIVED_URC     1            /*!< Indicates if an "#IPRECV" URC shall be generated when TCP or UDP data are available and can
                                                        be read with AT#IPREAD command */
#endif

#ifndef SOCKET_SECURITY_PROFILE_ID
#define SOCKET_SECURITY_PROFILE_ID    0            /*!< If specified and if the security profile exists, a TLS link will be used for the socket.    */
#endif

/* Parameters to be configured for the UDP transfer */
#ifndef UDP_DATA_TYPE
#define UDP_DATA_TYPE                 0            /*!< Type of data to transmit:
                                                      0: ASCII String as next parameter
                                                      1: Binary data
                                                      2: Hex data in text mode: 01A34B... = 0x01 0xA3 0x4B */
#endif

/* Parameters to be configured for MQTT transfer */
#ifndef MQTTCFG_CLIENT_NAME
#define MQTTCFG_CLIENT_NAME            "ST87"       /*!< Name of the MQTT client that is provided to the broker.*/
#endif

#ifndef MQTTCFG_CONNEXION_TIMEOUT
#define MQTTCFG_CONNEXION_TIMEOUT      10           /*!< Connection to the broker timeout in seconds.            */
#endif

#ifndef MQTTCFG_PROTOCOL_TIMEOUT
#define MQTTCFG_PROTOCOL_TIMEOUT       10           /*!< Connection to the broker timeout in seconds.            */
#endif

#ifndef MQTTCFG_PUBLISH_RETRY
#define MQTTCFG_PUBLISH_RETRY          10           /*!< Number of times a publish message is sent to the broker in case of an issue.*/
#endif

#ifndef MQTTCFG_KEEP_ALIVE_PB_MSG
#define MQTTCFG_KEEP_ALIVE_PB_MSG      100          /*!< Keep alive period in sec. after which a ping req/resp is exchanged between ST87 and broker.*/
#endif

/* Parameters to be configured for the TCP transfer */
#ifndef TCP_DATA_TYPE
#define TCP_DATA_TYPE                  0            /*!< Type of data to transmit:
                                                      0: ASCII String as next parameter
                                                      1: Binary data
                                                      2: Hex data in text mode: 01A34B... = 0x01 0xA3 0x4B */
#endif

/* Parameters to be configured for GNSS config */
#ifndef GNSS_CONSTELLATION_ID
#define GNSS_CONSTELLATION_ID          0            /*!< Release assistance information.
                                                          0: GPS,
                                                          1: GALILEO,
                                                          2: GPS+GALILEO */
#endif

#ifndef GNSS_FORMAT_TYPE
#define GNSS_FORMAT_TYPE               0            /*!< Release assistance information.
                                                           0: ST_AT,
                                                           1: NMEA */
#endif

/* Parameter used if ST_AT format enabled */
#ifndef GNSS_FORMAT_ST_POSITION
#define GNSS_FORMAT_ST_POSITION        1            /*!< Position data            */
#endif

#ifndef GNSS_FORMAT_ST_ACCURACY
#define GNSS_FORMAT_ST_ACCURACY        0            /*!< Accuracy data            */
#endif

#ifndef GNSS_FORMAT_ST_SATELLITES
#define GNSS_FORMAT_ST_SATELLITES      0            /*!< Satellites info          */
#endif

#ifndef GNSS_FORMAT_ST_ORIENTATION
#define GNSS_FORMAT_ST_ORIENTATION     0            /*!< Orientation info         */
#endif

/* Parameter used if NMEA format enabled */
/* Warning: If the NMEA format is active, please change the size of GNSS_FIX_MAX_DATA_SIZE */
#ifndef GNSS_NMEA_GPGGA
#define GNSS_NMEA_GPGGA                0            /*!< $GPGGA info              */
#endif

#ifndef GNSS_NMEA_GPGSA
#define GNSS_NMEA_GPGSA                0            /*!< $GPGSA info              */
#endif

#ifndef GNSS_NMEA_GPGSV
#define GNSS_NMEA_GPGSV                0            /*!< $GPGSV info              */
#endif

#ifndef GNSS_NMEA_GPGLL
#define GNSS_NMEA_GPGLL                1            /*!< $GPGLL info              */
#endif

#ifndef GNSS_NMEA_GPRMC
#define GNSS_NMEA_GPRMC                0            /*!< $GPRMS info              */
#endif

#ifndef GNSS_NMEA_GPVTG
#define GNSS_NMEA_GPVTG                0            /*!< $GPVTG info              */
#endif

#ifndef GNSS_NB_MAX_POSITION
#define GNSS_NB_MAX_POSITION           100          /*!< Maximum number of sample positions       */
#endif

/**
 *  WMBUS USER CONFIGURATION
 *  @{
 */
#ifndef WMBUS_TX_BUFFER_SIZE
#define WMBUS_TX_BUFFER_SIZE            (290)                           /*!< TX Buffer Size (maximum payload size required by application)  */
#endif

#ifndef WMBUS_WAKEUP_BACKOFF_MS
#define WMBUS_WAKEUP_BACKOFF_MS         (100)                           /*!< Time to wakeup device before WMBUS_T_NOM, after completion of synch transfer  */
#endif

#ifndef WMBUS_PHY_MODE
#define WMBUS_PHY_MODE                  WMBUS_PHY_MODE_T1               /*!< Use T1 or C1 mode */
#endif

#ifndef WMBUS_TX_POWER
#define WMBUS_TX_POWER                  (0)                             /*!< TX power range @note must be in the range [WMBUS_TX_POWER_MIN, WMBUS_TX_POWER_MAX]  */
#endif

#ifndef WMBUS_T_NOM
#define WMBUS_T_NOM                     (20)                            /*!< Nominal synch transmission interval in [sec] @note must be in the range [WMBUS_T_NOM_MIN, WMBUS_T_NOM_MAX] */
#endif

#ifndef WMBUS_DEV_TYPE
#define WMBUS_DEV_TYPE                  WMBUS_DEV_TYPE_METER            /*!< @note user can only select meter mode */
#endif

#ifndef WMBUS_FRAME_FORMAT
#define WMBUS_FRAME_FORMAT              WMBUS_FRAME_FORMAT_A            /*!< Frame format A or B */
#endif

#ifndef WMBUS_POSTAMPLE_LENGTH
#define WMBUS_POSTAMPLE_LENGTH          WMBUS_POSTAMBLE_LENGTH_T1_MIN    /*!< only for T1 mode */
#endif

#ifndef WMBUS_HEADER_LENGTH
#define WMBUS_HEADER_LENGTH             WMBUS_DEF_HEADER_LENGTH
#endif

#ifndef WMBUS_ASYNCH_SLEEP_DURATION_MS
#define WMBUS_ASYNCH_SLEEP_DURATION_MS  WMBUS_DEF_ASYNCH_SLEEP_DURATION   /*!< must be set according average interval between asynchronous requests sending */
#endif

/* Parameter for TLS Provisioning */
#ifndef TLS_PROV_KEY_FORMAT
#define TLS_PROV_KEY_FORMAT            0            /*!< TLS Provisioning Key format.
                                                          0: DER,
                                                          1: RAW binary */
#endif

/* Parameter for LwM2M */
#ifndef LWM2M_COLD_CONFIG_ENABLE
#define LWM2M_COLD_CONFIG_ENABLE       0            /*!< Enable (or not) LwM2M cold parameter setting in NVM */
#endif


/** @} */

/* Types ---------------------------------------------------------------------*/




#endif /* ST87EC_CONFIG_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
