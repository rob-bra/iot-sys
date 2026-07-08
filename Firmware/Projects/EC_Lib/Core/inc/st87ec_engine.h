/**
  ******************************************************************************
  * @file    st87ec_engine.h
  * @author  APMS Application Team
  * @brief   Engine header file
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
#ifndef ST87EC_ENGINE_H
#define ST87EC_ENGINE_H

#include <stdbool.h>
#include "st87ec_lib.h"
#include "st87ec_cold_config.h"
#include "st87ec_config.h"

/* Private constants and macros-----------------------------------------------*/

/* Description of the information for URC "#SIMST" */
#define SIMST_MAX_DATA_SIZE             (10)

/* Description of the information for Response "+CCLK" */
#define CCLK_TAG_SIZE                   (7)   /* Size of "+CCLK: " */
#define CCLK_MAX_DATA_SIZE              (30)

/* Description of the information for URC "+CSCON" */
#define CSCON_MAX_DATA_SIZE             (10)

/* Description of the information for URC "+CEREG" */
#define CEREG_MAX_DATA_SIZE             (10)

/* Description of the information for URC "#SLEEP" */
#define SLEEP_MAX_DATA_SIZE             (7)

/* Description of the information for URC "#WAKEUP" */
#define WAKEUP_MAX_DATA_SIZE             (8)

#if GNSS_FORMAT_TYPE == 1 /* NMEA format */
/* Unique  memory area for GNSS sequence */
#define GNSS_MAX_DATA_SIZE              (256)
#define GNSS_INIT_TAG_SIZE               (10)

#elif GNSS_FORMAT_TYPE == 0 /* ST format */
/* Unique  memory area for GNSS sequence */
#define GNSS_MAX_DATA_SIZE              (146)
#define GNSS_FIX_TAG_SIZE               (10)
/* Description of the information for URC "#GNSSINIT" */
#define GNSS_INIT_TAG_SIZE               (10)
#endif /* GNSS_FORMAT_TYPE */

/* Description of the information for URC "+CME" */
#define CME_ERROR_TAG_SIZE                  (11)
#define CME_ERROR_MAX_DATA_SIZE             (17)

/* Description of the information for URC "#IPCFG" */
#define IPCFG_MAX_DATA_SIZE             (14)

/* Description of the information for response "#SOCKETCREATE" */
#define SOCKETCREATE_MAX_DATA_SIZE      (19)

/* Description of the information for response "#IPRECV" */
#define IPRECV_MAX_DATA_SIZE      (13)

/* Description of the information for response "#IPREAD" */
#define IPREAD_MAX_DATA_SIZE      (17)

/* Description of the information for MQTT URC responses */
#define MQTTCONNECT_TAG_SIZE       (14)
#define MQTTCONNECT_MAX_DATA_SIZE  (16)
#define MQTTRECV_TAG_SIZE          (sizeof("#MQTTRECV: ") - 1)

/* Description of the information for response "#NVMRD" */
#define NVMREAD_MAX_DATA_SIZE      (12)

/* Description of the information for response "#TCPCONNECT" */
#define TCPCONN_MAX_DATA_SIZE      (18)

/* Description of the information for response "#TCPCONNECT" */
#define DTLSCONN_MAX_DATA_SIZE   (sizeof("#DTLSCONNECT: X,X,X") - 1)

/* Description of the information for CoAP responses */
#define COAP_MAX_DATA_SIZE         (537)  /* taking "#COAPRECV: payload" URC as longest string: 512 bytes of max payload length
                                                              + 25 bytes of max "#COAPRECV: payload" tag & echoed 16-bit Mid */
/* Description of the information for HTTP responses */
#define HTTP_TAG_SIZE              (12)
#define HTTP_MAX_DATA_SIZE         (14)

/* Description of the information for response "#HTTPRECV" */
#define HTTPRECV_TAG_SIZE           (sizeof("#HTTPRECV: ") - 1)
#define HTTPRECV_MAX_DATA_SIZE      (sizeof("#HTTPRECV: X,XXXX,XXX,XXXXXX") - 1)

/* Description of the information for DNS responses */
#define DNS_TAG_SIZE              (6)
#define DNS_MAX_DATA_SIZE         (22)

/* Description of the information for response "#HTTPREAD" */
#define HTTPREAD_TAG_SIZE            (sizeof("#HTTPREAD: ") - 1)
#define HTTPREAD_MAX_DATA_SIZE       (20)

/* Description of the information for response "#HTTPDISC" */
#define HTTPDISC_MAX_DATA_SIZE       (sizeof("#HTTPDISC: XXXXX") - 1) /* #HTTPDISC[: <tls_error>] URC with likely <tls_error> being an u16 */

#define WMBUS_INIT_MAX_DATA_SIZE     sizeof("#WMBUS_INIT:1")
#define WMBUS_SENT_MAX_DATA_SIZE     sizeof("#WMBUS_SENT")

#define WSCAN_DATA_TAG_SIZE          (sizeof("#WSCAN:") - 1)
#define WSCAN_DATA_MAX_DATA_SIZE     (50)  // TBC
#define WSCAN_RESTART_MAX_DATA_SIZE  sizeof("#WSRESTART")

#define TLSPROV_CADD_TAG_SIZE        (sizeof("#TLSCERTADD: "))
#define TLSPROV_KADD_TAG_SIZE        (sizeof("#TLSKEYADD: "))
#define TLSPROV_CLIST_TAG_SIZE       (sizeof("#TLSCERTLIST: "))
#define TLSPROV_KLIST_TAG_SIZE       (sizeof("#TLSKEYLIST: "))
#define TLSPROV_CSIGN_TAG_SIZE       (sizeof("#TLSCERTSIGN: "))

#define LWM2M_START_TAG_SIZE         (sizeof("#LWSTART: ") - 1)
#define LWM2M_START_MAX_DATA_SIZE    (sizeof("#LWSTART:  X") - 1)
#define LWM2M_REGISTER_TAG_SIZE      (sizeof("#LWREG: ") - 1)
#define LWM2M_REGISTER_MAX_DATA_SIZE (sizeof("#LWREG: X,X") - 1)
#define LWM2M_READ_TAG_SIZE          (sizeof("#LWREADRSCVAL: ") - 1)
#define LWM2M_LIST_TAG_SIZE          (sizeof("#LWLISTOBJ: ") - 1)
#define LWM2M_NTYREAD_TAG_SIZE       (sizeof("#LWNTYRDOBJ: ") - 1)
#define LWM2M_NTYWRITE_TAG_SIZE      (sizeof("#LWNTYWROBJ: ") - 1)
#define LWM2M_NTYEXE_TAG_SIZE        (sizeof("#LWNTYEXEOBJ: ") - 1)

/* Constant to be updated below if new URC/Response info is introduced */
#define TOTAL_DATA_LENGTH          COAP_MAX_DATA_SIZE   /* Considered as overall LONGEST data length dimensions TOTAL_LENGTH */

#define STR(VALUE)   # VALUE

#define ST87EC_WAKEUP_MODULE() ST87EC_Wrapper_WakeUp();

#if SLEEP_ENABLE == 1
#define ST87EC_MODULE_SLEEP() ST87EC_Wrapper_Delay(300);\
                              ST87EC_Wrapper_SendCmd("AT#SLEEPMODE");
#else
#define ST87EC_MODULE_SLEEP()
#endif  /* SLEEP_ENABLE */

#define ST87EC_RAW_BUFFER_SIZE       (2100) /* is max data length value corresponding to TLS provisioning CSR max allowable size (2*1024B + a margin) */

#define ST87EC_CME_ERR_DATA_BUF_SIZE (16)

#define CHAR_OFFSET     0x30

#define IP_ADDR_IPV4_CHAR_LEN  (16)
#define IP_ADDR_IPV6_CHAR_LEN  (40)

#define IPV4_CAPABLE_NW   (0x1)
#define IPV6_CAPABLE_NW   (0x2)


/* CME error code */
#define CME_ERR_CODE_1302  (1302) /* "PSA does not exist" (e.g., raised when TLS Provisioning element (key, cert) is not found) */

/* Types ---------------------------------------------------------------------*/

/* Definition of indexes for all the URC's information */ 
typedef enum{
  SIMST_INDEX         = 0,
  CCLK_INDEX          ,
  CSCON_INDEX         ,
  CEREG_INDEX         ,
  SLEEP_INDEX         ,
  WAKEUP_INDEX        ,
  GNSS_INIT_INDEX	  ,
#if GNSS_FORMAT_TYPE == 1 /* NMEA format */
  GNSS_GGA_INDEX      ,
  GNSS_GSV_INDEX      ,
  GNSS_GSA_INDEX      ,
  GNSS_GLL_INDEX      ,
  GNSS_RMC_INDEX      ,
  GNSS_VTG_INDEX      ,
#elif GNSS_FORMAT_TYPE == 0 /* ST format */
  GNSS_FIX_INDEX      ,
#endif /* GNSS_FORMAT_TYPE */
  CME_ERROR_INDEX     ,
  IPCFG_INDEX         ,
  SOCKETCREATE_INDEX  ,
  IPRECV_INDEX        ,
  IPREAD_INDEX        ,
  MQTTCONNECT_INDEX   ,
  MQTTRECV_INDEX      ,
  NVMREAD_INDEX       ,
  TCPCONN_INDEX       ,
  DTLSCONN_INDEX      ,
  COAPSTART_INDEX     ,
  COAPSEND_INDEX      ,
  COAPDATA_INDEX      ,
  HTTPSTART_INDEX     ,
  HTTPRECV_INDEX      ,
  DNS_INDEX           ,
  HTTPREAD_INDEX      ,
  HTTPDISC_INDEX      ,
  WMBUS_INIT_INDEX    ,
  WMBUS_SENT_INDEX    ,
  WSCAN_DATA_INDEX    ,
  WSCAN_RESTART_INDEX ,
  ATCMD_SENDING_INDEX ,
  ASYNC_URC_RCV_INDEX ,
  TLSPROV_CADD_INDEX  ,
  TLSPROV_KADD_INDEX  ,
  TLSPROV_CLIST_INDEX ,
  TLSPROV_KLIST_INDEX ,
  TLSPROV_CSIGN_INDEX ,
  LWM2M_START_INDEX   ,
  LWM2M_REGISTER_INDEX,
  LWM2M_READ_INDEX,
  LWM2M_LIST_INDEX,
  LWMNTYRDOBJ_INDEX,
  LWMNTYWROBJ_INDEX,
  LWMNTYEXEOBJ_INDEX,
  MAX_NB_OF_RSP
} ST87EC_Lib_RspIndex_t;

typedef struct
{
  const char * pTag;
  uint32_t     MaxDataSize;
}ST87EC_Lib_RspTab_t;

typedef enum{
  RSP_NONE                             = 0,                 /**< Response validity default value.                        */
  RSP_AWAITED                          = 1,                 /**< Response is awaited.                                    */
  RSP_RECEIVED                         = 2,                 /**< Response is received.                                   */
} ST87EC_Lib_RspValidity_t;

typedef enum {
  GETTIME_STATE_INIT                   = 0,                 /**< FSM of GetTime API is initialized (default value).  */
  GETTIME_STATE_WAIT_FOR_RSP           = 2,                 /**< FSM of GetTime API is in state to wait for URC/RSP. */
  GETTIME_STATE_ERROR                  = 3,                 /**< FSM of GetTime API is in error state.               */
} ST87EC_Lib_GetTime_FsmState_t;

typedef enum {
  GETFIX_STATE_INIT                      = 0,               /**< FSM of GetFix API is initialized (default value).            */
  GETFIX_STATE_INIT_WAIT_FOR_RSP         = 1,               /**< FSM of GetFix API is in state to wait for URC/RSP or timeout.*/
  GETFIX_STATE_FIX_SEND_AT_CMD           = 2,               /**< FSM of GetFix API is in state to send AT commands Fix.       */
  GETFIX_STATE_FIX_WAIT_FOR_RSP          = 3,               /**< FSM of GetFix API is in state to wait for URC/RSP or timeout.*/
  GETFIX_STATE_DEINIT_SEND_AT_CMD        = 4,               /**< FSM of GetFix API is in state to send AT commands DeInit.    */
  GETFIX_STATE_ERROR                     = 5,               /**< FSM of GetFix API is in error state.                         */
} ST87EC_Lib_GetFix_FsmState_t;

typedef enum{
  MQTT_NOT_CONNECTED                   = 0,                 /**< The ST87 is not connected to the Broker server               */
  MQTT_CONNECTED                       = 1,                 /**< The ST87 is connected to the Broker server                   */
} ST87EC_Lib_MqttStatus_t;

typedef enum{
  MQTT_PUBLISH                         = 0,                 /**< The MQTT request is Publish.            */
  MQTT_SUBSCRIBE                       = 1,                 /**< The MQTT request is Subscribe.          */
  MQTT_UNSUBSCRIBE                     = 2,                 /**< The MQTT request is Unsubscribe.        */
} ST87EC_Lib_MqttType_t;

typedef enum {
  MQTT_STATE_INIT               = 0,                 /**< FSM of MQTT session sequence is initialized (default value). */
  MQTT_STATE_WAIT_FOR_SOCKET    = 1,                 /**< FSM of TCP transfer API is waiting for socket creation       */
  MQTT_STATE_WAIT_RSP           = 2,                 /**< FSM of MQTT session sequence is in wait RSP state.           */
  MQTT_STATE_SEND_COMMANDS      = 3,                 /**< FSM of MQTT session sequence is in Send commands state.      */
  MQTT_STATE_ERROR              = 4,                 /**< FSM of MQTT session sequence is in error state.              */
  MQTT_STATE_START_DELAY        = 5,                 /**< FSM of MQTT session sequence is in Start delay state.        */
  MQTT_STATE_WAIT               = 6,                 /**< FSM of MQTT session sequence is in wait state.               */
} ST87EC_Lib_MqttSession_FsmState_t;

typedef enum {
  MQTT_SUBSC_STATE_NONE             = 0,             /**< MQTT session subscription state is no subscription on-going  */
  MQTT_SUBSC_STATE_TOPIC_SUBSCRIBED = 1,             /**< MQTT session subscription state is no subscription on-going  */
}ST87EC_Lib_MqttSession_SubscriptionState_t;

typedef enum {
  WMBUS_TRANSFER_STATE_INIT       = 0,
  WMBUS_TRANSFER_STATE_WAKEUP_INIT   ,
  WMBUS_TRANSFER_STATE_INIT_OK       ,
  WMBUS_TRANSFER_STATE_INIT_URC      ,
  WMBUS_TRANSFER_STATE_CFG           ,                      /**< Send next configuration command                                       */
  WMBUS_TRANSFER_STATE_CFG_OK        ,                      /**< Wait for configuration answer                                         */
  WMBUS_TRANSFER_STATE_READY         ,                      /**< Notify User that sequence is ready to receive send command            */
  WMBUS_TRANSFER_STATE_SEND          ,                      /**< Prepare send */
  WMBUS_TRANSFER_STATE_SEND_OK       ,                      /**< Wait for send complete                                                */
  WMBUS_TRANSFER_STATE_SEND_URC      ,
  WMBUS_TRANSFER_STATE_SLEEP         ,                      /**< Notify User that message has been sent and device will enter in sleep */
  WMBUS_TRANSFER_STATE_SLEEP_OK      ,                      /**< Wait for sleep entering                                               */
  WMBUS_TRANSFER_STATE_SLEEP_URC     ,
  WMBUS_TRANSFER_STATE_WAKEUP_URC    ,                      /**< Wait for wake-up                                                      */
  WMBUS_TRANSFER_STATE_WAKEUP_READY  ,
  WMBUS_TRANSFER_STATE_WAKEUP_DEINIT ,
  WMBUS_TRANSFER_STATE_DEINIT        ,
  WMBUS_TRANSFER_STATE_DEINIT_OK     ,
  WMBUS_TRANSFER_STATE_ERROR         ,
  WMBUS_TRANSFER_STATE_ERROR_OK      ,
  WMBUS_TRANSFER_STATE_END           ,
} ST87EC_Lib_WmbusTransfer_FsmState_t;

typedef enum {
  WSCAN_STATE_INIT                     = 0,                 /**< FSM state for WifiScan seq: initializing seq and request scan start. */
  WSCAN_STATE_WAIT_FOR_START_CNF       = 1,                 /**< FSM state for WifiScan seq: waiting for start confirmation.          */
  WSCAN_STATE_SCAN_WAIT_FOR_URCS       = 2,                 /**< FSM state for WifiScan seq: waiting for URCs.                        */
  WSCAN_STATE_SCAN_STOP                = 3,                 /**< FSM state for WifiScan seq: requesting Wifi scan stop.               */
  WSCAN_STATE_WAIT_FOR_STOP_CNF        = 4,                 /**< FSM state for WifiScan seq: waiting for stop confirmation.           */
  WSCAN_STATE_ERROR                    = 5,                 /**< FSM state for WifiScan seq: managing errors.                         */
  WSCAN_STATE_END_SEQUENCE             = 6,                 /**< FSM state for WifiScan seq: managing end of sequence                 */
} ST87EC_Lib_WifiScan_FsmState_t;

typedef enum {
  MQTT_COMMAND_WAIT_SOCKET             = 0,                 /**< Command of MQTT API to wait the 'ok' of socketcreate  */
  MQTT_COMMAND_CONFIG                  = 1,                 /**< Command of MQTT API to configure                      */
  MQTT_COMMAND_CONNECT                 = 2,                 /**< Command of MQTT API to connect                        */
  MQTT_COMMAND_CHECK_CONNECT           = 3,                 /**< Command of MQTT API to check the connection           */
  MQTT_COMMAND_OPERATE                 = 4,                 /**< Command of MQTT API to publish                        */
  MQTT_COMMAND_DISCONNECT              = 5,                 /**< Command of MQTT API to disconnect                     */
  MQTT_COMMAND_END                     = 6,                 /**< Command of MQTT API end                               */
} ST87EC_Lib_MqttSession_Command_t;

typedef enum {
  SOCKET_TCP_TYPE                       = 0,                /**< The type of socket to create is TCP                          */
  SOCKET_UDP_TYPE                       = 1,                /**< The type of socket to create is UDP                          */
  SOCKET_RAW_TYPE                       = 2,                /**< The type of socket to create is RAW                          */
} ST87EC_Lib_SocketType_t;

typedef enum {
  COLDINIT_STATE_INIT                    = 0,               /**< FSM of Cold init API is initialized (default value).  */
  COLDINIT_STATE_CHECK_VERSION           = 1,               /**< FSM of Cold init API waits for the version check.     */
  COLDINIT_STATE_ERROR                   = 2,               /**< FSM of Cold init API error.                           */
  COLDINIT_STATE_SEND_COMMANDS           = 3,               /**< FSM of Cold init API Send init commands.              */
  COLDINIT_STATE_WAIT_OK                 = 4,               /**< FSM of Cold init API Wait for OK                      */
  COLDINIT_STATE_END                     = 5,               /**< FSM of Cold init API end sequence                     */
} ST87EC_Lib_ColdInit_FsmState_t;

typedef enum {
  LOADER_STATE_INIT                     = 0,               /**< Loader FSM state for initializing Loader API              */
  LOADER_STATE_SEND_COMMANDS            = 1,               /**< Loader FSM state for command sending                      */
  LOADER_STATE_WAIT_RSP                 = 2,               /**< Loader FSM state for command response waiting and parsing */
  LOADER_STATE_ERROR                    = 3,               /**< Loader FSM state for error management                     */
  LOADER_STATE_END_SEQUENCE             = 4,               /**< Loader FSM state for ending the sequence                  */
} ST87EC_Lib_Loader_FsmState_t;

typedef enum {
  LOADER_CMD_SEND_SPECIFIC_CMDS            = 0,             /**< Loader command to send specific cmds depending on the target bin to load */
  LOADER_CMD_RESET_IN_BOOT_MODE            = 1,             /**< Loader command to reset ST87 in Boot mode                       */
  LOADER_CMD_SET_BOOT_IN_ATPARSE_STATE     = 2,             /**< Loader command to set ST87 Boot in AT Parser state              */
  LOADER_CMD_CONFIG_ST87_LOADING_BAUDRATE  = 3,             /**< Loader command to configure ST87 UART in the correct baudrate   */
  LOADER_CMD_CONFIG_HOST_LOADING_BAUDRATE  = 4,             /**< Loader command to configure ST87 UART in the correct baudrate   */
  LOADER_CMD_SET_BOOT_IN_UARTLOAD_STATE    = 5,             /**< Loader command to set ST87 Boot in UART Load state              */
  LOADER_CMD_UPLOAD_REBOOT                 = 6,             /**< Loader command to fulfilling binary upload and rebooting ST87   */
} ST87EC_Lib_Loader_Command_t;

typedef enum {
  UDPTCP_TRANSFER_STATE_INIT                         = 0,   /**< FSM of UDP/TCP transfer API is initialized (default value)    */
  UDPTCP_TRANSFER_STATE_WAIT_FOR_SOCKET              = 1,   /**< FSM of UDP/UDP transfer API is waiting for socket creation    */
  UDPTCP_TRANSFER_STATE_DTLS_CONNECT                 = 2,   /**< FSM of UDP/UDP transfer API is sending a DTLS Connect         */
  UDPTCP_TRANSFER_STATE_TCP_CONNECT                  = 3,   /**< FSM of UDP/UDP transfer API is sending a TCP Connect          */
  UDPTCP_TRANSFER_STATE_IP_SEND                      = 4,   /**< FSM of UDP/UDP transfer API is sending data via UDP or TCP    */
  UDPTCP_TRANSFER_STATE_WAIT_FOR_UDPTCP_RSP          = 5,   /**< FSM of UDP/UDP transfer is waiting for the UDP or TCP answer  */
  UDPTCP_TRANSFER_STATE_SEND_DATA_BIN                = 6,   /**< FSM of UDP/UDP transfer is sending binary data                */
  UDPTCP_TRANSFER_STATE_ERROR                        = 7,   /**< FSM of UDP/UDP transfer API is in error state.                */
  UDPTCP_TRANSFER_STATE_WAIT_FOR_IPRECV              = 8,   /**< FSM of UDP/UDP transfer API is waiting for "#IPRECV"          */
  UDPTCP_TRANSFER_STATE_WAIT_FOR_IPREAD              = 9,   /**< FSM of UDP/UDP transfer API is waiting for "#IPREAD"          */
  UDPTCP_TRANSFER_STATE_WAIT_FOR_IPREAD_RAWDATA      = 10,  /**< FSM of UDP/UDP transfer API is waiting for "#IPREAD" data     */
  UDPTCP_TRANSFER_STATE_END_PROCEDURE                = 11,  /**< FSM of UDP/UDP transfer API is in transfer end procedure      */
  UDPTCP_TRANSFER_STATE_END_SEQUENCE                 = 12,  /**< FSM of UDP/UDP transfer API is in transfer end sequence       */
} ST87EC_Lib_UdpTcpTransfer_FsmState_t;

typedef enum {
  UDPTCP_TRANSFER_TCPCONN_STATE_TEST                 = 0,   /**< Sub FSM of UDP/UDP transfer API starting TCP Connection procedure          */
  UDPTCP_TRANSFER_TCPCONN_STATE_WAIT_FOR_CHECK       = 1,   /**< Sub FSM of UDP/UDP transfer API waiting for connection check response      */
  UDPTCP_TRANSFER_TCPCONN_STATE_WAIT_FOR_TCPCONN     = 2,   /**< Sub FSM of UDP/UDP transfer API waiting for new connection ok confirmation */
} ST87EC_Lib_UdpTcpTransferTcpConn_SubFsmState_t;

typedef enum {
  UDPTCP_TRANSFER_DTLSCONN_STATE_TEST                 = 0,   /**< Sub FSM of UDP/UDP transfer API starting DTLS Connection procedure              */
  UDPTCP_TRANSFER_DTLSCONN_STATE_WAIT_FOR_CHECK       = 1,   /**< Sub FSM of UDP/UDP transfer API waiting for DTLS connection check response      */
  UDPTCP_TRANSFER_DTLSCONN_STATE_WAIT_FOR_CONN_OK     = 2,   /**< Sub FSM of UDP/UDP transfer API waiting for DTLS new connection ok confirmation */
} ST87EC_Lib_UdpTcpTransferDtlsConn_SubFsmState_t;

typedef enum {
  UDPTCP_TRANSFER_ENDPROC_STATE_START                = 0,   /**< Sub FSM of UDP/UDP transfer API starting UDP/TCP transfer end procedure   */
  UDPTCP_TRANSFER_ENDPROC_STATE_WAIT_FOR_SOCKETCLOSE = 1,   /**< Sub FSM of UDP/UDP transfer API waiting for Socket close prior to ending  */
} ST87EC_Lib_UdpTcpTransferEnd_SubFsmState_t;

/**< Enum for specific responses tags  */
typedef enum {
  SPECIFIC_RSP_NONE                      = 0,               /**< No specific response message indication from ST87           */
  SPECIFIC_RSP_OK                        = 1,               /**< 'OK' specific response message indication from ST87         */
  SPECIFIC_RSP_HANDSHAKE_OK              = 2,               /**< 'OK' specific response for ST87 Boot handshake              */
  SPECIFIC_RSP_CME                       = 3,               /**< 'CME ERROR' specific response message indication from ST87  */
  SPECIFIC_RSP_NBSENT                    = 4,               /**< 'NB_SENT' specific response message indication from ST87    */
  SPECIFIC_RSP_WATCHDOG                  = 5,               /**< "#REBOOT_WD" specific response message indication from ST87 */
} ST87EC_Lib_SpecificResponse_t;

typedef enum {
  SOCKET_CREATION_TEST                     = 0,             /**< FSM of socket creation checks if socket needs to be created    */
  SOCKET_CREATION_WAIT_FOR_SOCKET_CHECK    = 1,             /**< FSM of socket creation waits for socket creation check         */
  SOCKET_CREATION_WAIT_FOR_DNS_RSP         = 2,             /**< FSM of socket creation waits for DNS resolution (HTTP only)    */
  SOCKET_CREATION_SOCKETCREATE             = 3,             /**< FSM of socket creation proceed with socket creation at ST87    */
  SOCKET_CREATION_WAIT_FOR_SOCKET_CREATION = 4,             /**< FSM of socket creation waits for socket creation rps from ST87 */
} ST87EC_Lib_SocketCreation_FsmState_t;

typedef enum {
  SOCKET_CLOSE_TEST                       = 0,              /**< FSM of socket close checks if socket exists             */
  SOCKET_CLOSE_WAIT_FOR_SOCKET_CHECK      = 1,              /**< FSM of socket close waits for socket existence check    */
  SOCKET_CLOSE_WAIT_FOR_SOCKET_CLOSE      = 2,              /**< FSM of socket close waits for socket close fulfillment  */
} ST87EC_Lib_SocketClose_FsmState_t;

typedef enum {
  COAP_TRANSFER_NONE                     = 0,               /**< CoAP FSM state no CoAP session on-going                 */
  COAP_TRANSFER_OPENED                   = 1,               /**< CoAP FSM state opening CoAP session on-going            */
  COAP_TRANSFER_RXTX                     = 2,               /**< CoAP FSM state Rx and/or Tx CoAP session on-going       */
} ST87EC_Lib_CoapTransfer_FsmState_t;

typedef enum {
  COAP_OPEN_STATE_INIT                   = 0,               /**< CoAP Open FSM state for initialization                  */
  COAP_OPEN_STATE_WAIT_FOR_COAP_INIT_LVL = 1,               /**< CoAP Open FSM state waiting for COAPSTART cmd response  */
  COAP_OPEN_STATE_SEND_CMDS              = 2,               /**< CoAP Open FSM state for sending commands                */
  COAP_OPEN_STATE_WAIT_RSP               = 3,               /**< CoAP Open FSM state for waiting for OK/CME responses    */
  COAP_OPEN_STATE_ERROR                  = 4,               /**< CoAP Open FSM state for error management                */
  COAP_OPEN_STATE_END_SEQUENCE           = 5,               /**< CoAP Open FSM state for ending the sequence             */
} ST87EC_Lib_CoapOpen_FsmState_t;

typedef enum {
  COAP_OPEN_CMD_START                    = 0,               /**< CoAP Open command to start CoAP session                 */
  COAP_OPEN_CMD_CONNECT                  = 1,               /**< CoAP Open command to connect CoAP session               */
  COAP_OPEN_CMD_END                      = 2,               /**< State ending CoAP Open command procedure                */
} ST87EC_Lib_CoapOpen_Command_t;

typedef enum {
  COAP_TX_STATE_INIT                     = 0,               /**< CoAP Transmit FSM state for initialization                */
  COAP_TX_STATE_WAIT_OPT_RSP             = 1,               /**< CoAP Transmit FSM state waiting for COAPOPT cmd response  */
  COAP_TX_STATE_WAIT_SENDDATA_RSP        = 2,               /**< CoAP Transmit FSM state waiting for COAPSEND cmd response */
  COAP_TX_STATE_ERROR                    = 3,               /**< CoAP Transmit FSM state for error management              */
  COAP_TX_STATE_END_SEQUENCE             = 4,               /**< CoAP Transmit FSM state for ending the sequence           */
} ST87EC_Lib_CoapTx_FsmState_t;

typedef enum {
  COAP_RX_STATE_NONE                     = 0,               /**< No CoAP URC reception                                   */
  COAP_RX_STATE_URC_RECV                 = 1,               /**< CoAP URC reception                                      */
  COAP_RX_STATE_PAYLOAD_DATA_RECV        = 2,               /**< CoAP payload data reception                             */
} ST87EC_Lib_CoapRx_State_t;

typedef enum {
  COAP_CLOSE_STATE_INIT                  = 0,               /**< CoAP Close FSM state for initialization                 */
  COAP_CLOSE_STATE_CLOSE_ONGOING         = 1,               /**< CoAP Close FSM state for closing procedure              */
  COAP_CLOSE_STATE_WAIT_RSP              = 2,               /**< CoAP Close FSM state for waiting for OK/CME responses   */
  COAP_CLOSE_STATE_ERROR                 = 3,               /**< CoAP Close FSM state for error management               */
  COAP_CLOSE_STATE_END_SEQUENCE          = 4,               /**< CoAP Close FSM state for ending the sequence            */
} ST87EC_Lib_CoapClose_FsmState_t;

typedef enum {
  COAP_CLOSE_CMD_DISC                    = 0,               /**< CoAP Close command to disconnect CoAP session           */
  COAP_CLOSE_CMD_STOP                    = 1,               /**< CoAP Close command to stop CoAP session                 */
} ST87EC_Lib_CoapClose_Command_t;


typedef enum {
  HTTP_TRANSFER_NONE                     = 0,               /**< HTTP FSM state no HTTP session on-going                 */
  HTTP_TRANSFER_OPENED                   = 1,               /**< HTTP FSM state opening HTTP session on-going            */
  HTTP_TRANSFER_RXTX                     = 2,               /**< HTTP FSM state Rx and/or Tx HTTP session on-going       */
} ST87EC_Lib_HttpTransfer_FsmState_t;

typedef enum {
  HTTP_OPEN_STATE_INIT                   = 0,               /**< HTTP Open FSM state for initialization                  */
  HTTP_OPEN_STATE_WAIT_FOR_HTTP_INIT_LVL = 1,               /**< HTTP Open FSM state waiting for HTTP cmd response  */
  HTTP_OPEN_STATE_SEND_CMDS              = 2,               /**< HTTP Open FSM state for sending commands                */
  HTTP_OPEN_STATE_WAIT_RSP               = 3,               /**< HTTP Open FSM state for waiting for OK/CME responses    */
  HTTP_OPEN_STATE_ERROR                  = 4,               /**< HTTP Open FSM state for error management                */
  HTTP_OPEN_STATE_END_SEQUENCE           = 5,               /**< HTTP Open FSM state for ending the sequence             */
} ST87EC_Lib_HttpOpen_FsmState_t;

typedef enum {
  HTTP_OPEN_CMD_SOCKET_CREATE            = 0,               /**< HTTP Open command to create TCP Socket                  */
  HTTP_OPEN_CMD_SOCKET_CONNECT           = 1,               /**< HTTP Open command to connect to a TCP Socket            */
  HTTP_OPEN_CMD_START                    = 2,               /**< HTTP Open command to start HTTP session                 */
  HTTP_OPEN_CMD_END                      = 3,               /**< State ending HTTP Open command procedure                */
} ST87EC_Lib_HttpOpen_Command_t;

typedef enum {
  HTTP_CLOSE_STATE_INIT                  = 0,               /**< HTTP Close FSM state for initialization                 */
  HTTP_CLOSE_STATE_CLOSE_ONGOING         = 1,               /**< HTTP Close FSM state for closing procedure              */
  HTTP_CLOSE_STATE_WAIT_RSP              = 2,               /**< HTTP Close FSM state for waiting for OK/CME responses   */
  HTTP_CLOSE_STATE_ERROR                 = 3,               /**< HTTP Close FSM state for error management               */
  HTTP_CLOSE_STATE_END_SEQUENCE          = 4,               /**< HTTP Close FSM state for ending the sequence            */
} ST87EC_Lib_HttpClose_FsmState_t;

typedef enum {
  HTTP_CLOSE_CMD_STOP                    = 0,               /**< HTTP Stop command to stop Http stack                    */
  HTTP_CLOSE_CMD_CLOSE                   = 1,               /**< HTTP Close command to close TCP socket                  */
} ST87EC_Lib_HttpClose_Command_t;

typedef enum {
  HTTP_TRANSFER_STATE_INIT                      = 0,        /**< HTTP Transfer FSM state for initialization                          */
  HTTP_TRANSFER_STATE_SEND_CMDS                 = 1,        /**< HTTP Transfer FSM state for sending commands                        */
  HTTP_TRANSFER_STATE_WAIT_RSP                  = 2,        /**< HTTP Transfer FSM state for waiting for OK/CME responses            */
  HTTP_TRANSFER_STATE_WAIT_FOR_HTTPRECV         = 3,        /**< HTTP Transfer FSM state for waiting for HTTPRECV responses          */
  HTTP_TRANSFER_STATE_SEND_HTTPREAD             = 4,        /**< HTTP Transfer FSM state for sending HTTPREAD command                */
  HTTP_TRANSFER_STATE_WAIT_FOR_HTTPREAD         = 5,        /**< HTTP Transfer FSM state for waiting for HTTPREAD responses          */
  HTTP_TRANSFER_STATE_WAIT_FOR_HTTPREAD_RAWDATA = 6,        /**< HTTP Transfer FSM state for waiting for raw data HTTPREAD responses */
  HTTP_TRANSFER_STATE_PRINT_DATA                = 7,        /**< HTTP Transfer FSM state outputting data to EC Lib API               */
  HTTP_TRANSFER_STATE_ERROR                     = 8,        /**< HTTP Transfer FSM state for error management                        */
  HTTP_TRANSFER_STATE_END_SEQUENCE              = 9,        /**< HTTP Transfer FSM state for ending the sequence                     */
} ST87EC_Lib_HttpRxTx_FsmState_t;

typedef enum {
  HTTP_TRANSFER_CMD_METHOD               = 0,               /**< HTTP command to set the method of the next HTTP transaction.*/
  HTTP_TRANSFER_CMD_HEADER               = 1,               /**< HTTP command to set header(s) of the next HTTP transaction. */
  HTTP_TRANSFER_CMD_SEND                 = 2,               /**< HTTP Transfer command to send the HTTP transaction          */
  HTTP_TRANSFER_CMD_END                  = 3,               /**< State ending HTTP Transfer command procedure                */
} ST87EC_Lib_HttpRxTx_Command_t;

typedef enum {
  NORMAL_MODE                            = 0,               /**< ST87 Boot mode in normal                     */
  BOOT_AT_MODE                           = 1,               /**< ST87 Boot mode in boot                       */
  BOOT_UPLOAD_MODE                       = 2,               /**< ST87 Boot mode in boot in upload             */
} ST87EC_Lib_BootMode_t;

typedef enum {
  LOADER_AT_STEP_NONE                   = 0,                /**< Loader sub FSM state where no waiting for Boot AT response is on-going             */
  LOADER_AT_WAIT_FOR_AT_RSP             = 1,                /**< Loader sub FSM state waiting for Boot AT response                                  */
  LOADER_AT_START_BOOT_UART_HANDSHAKE   = 2,                /**< Loader sub FSM state executing Boot UART handshake (when no rsp from Boot is seen) */
  LOADER_AT_WAIT_FOR_HANDSHAKE_RSP      = 3                 /**< Loader sub FSM state waiting for Boot UART handshake response                      */
} ST87EC_Lib_SentAtToBootSteps_t;

typedef enum {
  ATCMD_SEND_STATE_INIT                 = 0,                /**< Unitary AT command sending FSM state for initialization            */
  ATCMD_SEND_STATE_WAIT_FOR_RSP         = 1,                /**< Unitary AT command sending FSM state waiting for OK/CME responses  */
  ATCMD_SEND_STATE_ERROR                = 2,                /**< Unitary AT command sending FSM state for error management          */
  ATCMD_SEND_STATE_END                  = 3,                /**< Unitary AT command sending FSM state for ending the sequence       */
} ST87EC_Lib_AtCmdSend_FsmState_t;

typedef enum {
  ASYNC_URC_RCV_STATE_INIT              = 0,                /**< Asynchronous URC receive FSM state for initialization                     */
  ASYNC_URC_RCV_STATE_WAIT_FOR_CNF      = 1,                /**< Asynchronous URC receive FSM state waiting for OK/CME responses           */
  ASYNC_URC_RCV_STATE_ERROR             = 2,                /**< Asynchronous URC receive FSM state for error management                   */
  ASYNC_URC_RCV_STATE_END               = 3,                /**< Asynchronous URC receive FSM state for ending the sequence                */
  ASYNC_URC_RCV_STATE_WAIT_FOR_URC      = 4,                /**< Asynchronous URC receive FSM state waiting for asynchronous URC reception */
} ST87EC_Lib_AsyncUrcRcv_FsmState_t;

typedef enum {
  TLS_PROV_STATE_INIT                   = 0,                /**< TLS Provisioning FSM state for initialization                     */
  TLS_PROV_STATE_WAIT_FOR_RSP           = 1,                /**< TLS Provisioning FSM state waiting for OK/CME responses           */
  TLS_PROV_STATE_WAIT_FOR_DATA_RSP      = 2,                /**< TLS Provisioning FSM state for TLS provisioning data reception    */
  TLS_PROV_STATE_WAIT_FOR_NVM_WRITE     = 3,                /**< TLS Provisioning FSM state for element storing/deletion from NVM  */
  TLS_PROV_STATE_ERROR                  = 4,                /**< TLS Provisioning FSM state for error management                   */
  TLS_PROV_STATE_END                    = 5,                /**< TLS Provisioning FSM state for ending the sequence                */
} ST87EC_Lib_TlsProv_FsmState_t;

typedef enum {
  LWM2M_SESSION_NONE                     = 0,               /**< LwM2M FSM state for no session on-going                      */
  LWM2M_SESSION_OPENED                   = 1,               /**< LwM2M FSM state for opened LwM2M session                     */
} ST87EC_Lib_Lwm2mSession_FsmState_t;

typedef enum {
  LWM2M_OPEN_STATE_INIT                   = 0,              /**< LwM2M Open FSM state for initialization                      */
  LWM2M_OPEN_STATE_WAIT_FOR_GET_INIT_LVL  = 1,              /**< LwM2M Open FSM state waiting for LWSTART cmd response        */
  LWM2M_OPEN_STATE_SEND_CMDS              = 2,              /**< LwM2M Open FSM state for sending commands                    */
  LWM2M_OPEN_STATE_WAIT_RSP               = 3,              /**< LwM2M Open FSM state for waiting for OK/CME responses        */
  LWM2M_OPEN_STATE_ERROR                  = 4,              /**< LwM2M Open FSM state for error management                    */
  LWM2M_OPEN_STATE_END_SEQUENCE           = 5,              /**< LwM2M Open FSM state for ending the sequence                 */
} ST87EC_Lib_Lwm2mOpen_FsmState_t;

typedef enum {
  LWM2M_OPEN_STATE_NONE                   = 0,              /**< LwM2M stack state "not started"                              */
  LWM2M_OPEN_STATE_STARTED                = 1,              /**< LwM2M stack state "started"                                  */
  LWM2M_OPEN_STATE_CONFIGURED             = 2,              /**< LwM2M stack state "configured"                               */
  LWM2M_OPEN_STATE_ACTIVATED              = 3,              /**< LwM2M stack state "activated"                                */
} ST87EC_Lib_Lwm2mOpen_Command_t;

typedef enum {
  LWM2M_OP_STATE_INIT                   = 0,                /**< LwM2M Operate FSM state for initialization                   */
  LWM2M_OP_STATE_WAIT_RSP               = 2,                /**< LwM2M Operate FSM state for waiting for ST87 responses       */
  LWM2M_OP_STATE_ERROR                  = 3,                /**< LwM2M Operate FSM state for error management                 */
  LWM2M_OP_STATE_END_SEQUENCE           = 4,                /**< LwM2M Operate FSM state for ending the sequence              */
} ST87EC_Lib_Lwm2mOperate_FsmState_t;

typedef enum {
  LWM2M_OP_TYPE_REGISTRATION           = 0,                 /**< LwM2M operation type of registration request                 */
  LWM2M_OP_TYPE_READ                   = 1,                 /**< LwM2M operation type of built-object read                    */
  LWM2M_OP_TYPE_WRITE                  = 2,                 /**< LwM2M operation type of built-object write                   */
  LWM2M_OP_TYPE_LIST                   = 3,                 /**< LwM2M operation type of built-object listing                 */
  LWM2M_OP_TYPE_ADDOBJ                 = 4,                 /**< LwM2M operation type of custom object create                 */
  LWM2M_OP_TYPE_RMVOBJ                 = 5,                 /**< LwM2M operation type of custom object remove                 */
  LWM2M_OP_TYPE_NTYREAD                = 6,                 /**< LwM2M operation type of custom object read notify            */
  LWM2M_OP_TYPE_NTYWRITE               = 7,                 /**< LwM2M operation type of custom object write notify
                                                                 (to be kept just after LWM2M_OP_TYPE_NTYREAD!                */
  LWM2M_OP_TYPE_NTYEXE                 = 8,                 /**< LwM2M operation type of custom object execute notify
                                                                 (to be kept just after LWM2M_OP_TYPE_NTYWRITE!               */
} ST87EC_Lib_Lwm2mOperate_OpType_t;

typedef enum {
  LWM2M_CLOSE_STATE_INIT                  = 0,              /**< LwM2M Close FSM state for initialization                     */
  LWM2M_CLOSE_STATE_WAIT_RSP              = 2,              /**< LwM2M Close FSM state for waiting for ST87 responses         */
  LWM2M_CLOSE_STATE_ERROR                 = 3,              /**< LwM2M Close FSM state for error management                   */
  LWM2M_CLOSE_STATE_END_SEQUENCE          = 4,              /**< LwM2M Close FSM state for ending the sequence                */
} ST87EC_Lib_Lwm2mClose_FsmState_t;

typedef struct{
  uint32_t Hash;                                            /**< Hash value of the URC/RSP tag.                          */
  ST87EC_Lib_RspValidity_t Validity;                        /**< Validity status of the URC/RSP tag.                     */
} ST87EC_Lib_RspInfo_t;

typedef struct{                                             
  ST87EC_Lib_Status_t * pState;                             /**< Parameter of the GetState API, giving a ptr to
                                                                the current status.                                      */
} ST87EC_Lib_GetStateParams_t;                              

typedef struct{                                             
  ST87EC_Lib_GetTimeCallback_t * pGetTimeCallbackFunc;      /**< Parameter of the GetTime API,
                                                                giving a ptr to the return callback.                     */
  uint32_t TimeoutMs;                                       /**< Parameter of the GetTime API, giving the timeout
                                                                in ms after which the sequence is aborted if no
                                                                is available.                                            */
} ST87EC_Lib_GetTime_Params_t;                              


typedef struct{                                             
  ST87EC_Lib_GetTime_FsmState_t FsmState;                   /**< Current status of the GetTime FSM.                      */
  ST87EC_Lib_GetTime_Params_t Params;                       /**< Structure containing the parameters of the GetTime API. */
  uint32_t TimerId;                                         /**< Timer Id for the running timeout */
} ST87EC_Lib_GetTime_t;                                     

typedef struct{
  ST87EC_Lib_RingPinStatus_t Status;                        /**< Current status of the RingPin.                          */
} ST87EC_Lib_RingPin_t;

typedef struct{
  ST87EC_Lib_ColdInit_FsmState_t FsmState;                   /**< Current status of the GetTime FSM.*/
  uint32_t TimerId;                                          /**< Timer Id for the running timeout  */
  uint32_t SequenceIndex;                                    /**< Index of the cold init sequence  */
} ST87EC_LibColdInit_t;

typedef struct{
  ST87EC_Lib_BinToLoad_t BinaryId;                          /**< Parameter of the Loader API giving the name of the binary to load (targeted core..) */
  uint8_t * pBinaryStartAddr;                               /**< Parameter of the Loader API giving a pointer to the binary to load start address    */
  uint32_t BinaryLength;                                    /**< Parameter of the Loader API giving the length of the binary to load                 */
  uint32_t Timeout;                                         /**< Parameter of the Loader API, giving the limit time for the binary upload in ms
                                                                    (after which the sequence is aborted ).                                          */
} ST87EC_Lib_Loader_Params_t;

typedef struct{
  ST87EC_Lib_Loader_FsmState_t FsmState;                    /**< Current state of the Loader FSM */
  ST87EC_Lib_Loader_Params_t Params;                        /**< Structure containing the parameters of the Loader API */
  uint32_t TimerId;                                         /**< Timer Id for the sequence running timeout  */
  ST87EC_Lib_Loader_Command_t Command;                      /**< Index of current Loader command  */
  uint8_t NbCmdSent;                                        /**< Number of command already sent  */
  uint32_t OriBaudrate;                                     /**< Storage of ST87 UART original baudrate value  */
} ST87EC_Lib_Loader_t;

typedef struct                                              
{                                                           
  ST87EC_Lib_GetTime_t GetTime;                             /**< Structure containing the information of the GetTime API. */
  ST87EC_Lib_RingPin_t RingPin;                             /**< Structure containing the information of the RingPin API. */
  ST87EC_LibColdInit_t ColdInit;                            /**< Structure containing the information of the ColdInit API.*/
  ST87EC_Lib_Loader_t Loader;                               /**< Structure containing the information of the Loader API.  */
} ST87EC_Lib_SequenceSysStatus_t;                           

typedef struct{
  uint8_t InitDone;                                         /**< Parameter of the GetFix API, check if init is done      */
  uint32_t CountPos;                                        /**< Parameter of the GetFix API, counting the Pos sampled   */
  ST87EC_Lib_GNSS_GetPosCallback_t * pGetFixCallbackFunc;   /**< Parameter of the GetTime API,
                                                                giving a ptr to the return callback.                     */
  uint32_t TimeoutMs;                                       /**< Parameter of the GetFix API, giving the timeout
                                                                in ms after which the sequence is aborted if no
                                                                is available.                                            */
  uint32_t NbPosition;                                      /**< Parameter of the GetFix API, giving the NbPosition
                                                                 to sample                                               */
} ST87EC_Lib_GetFix_Params_t;

typedef struct
{
  ST87EC_Lib_GetFix_FsmState_t FsmState;                   /**< Current status of the GetFix FSM.                      */
  ST87EC_Lib_GetFix_Params_t Params;                       /**< Structure containing the parameters of the GetFix API. */
  uint32_t TimerId;                                        /**< Timer Id for the running timeout                       */
} ST87EC_Lib_GetFix_t;

typedef struct
{
  ST87EC_Lib_GetFix_t GetFix;                              /**< Structure containing the information of the GetFix API. */
} ST87EC_Lib_SequenceGnssStatus_t;


typedef struct
{
  uint8_t contextId;                                       /**< Context ID of the data transfer.                              */
  uint8_t ipModeCaps;                                      /**< IP mode capabilities (IPv4 and/or IPv6) of the network.       */
  uint8_t socketId;                                        /**< Socket ID of the data transfer.                               */
  int8_t SecProfileId;                                     /**< Security profile Id (TLS connection use, e.g. HTTPS,
                                                                 left set to -1 otherwise).                                   */
  char pIpAddress[IP_ADDR_IPV6_CHAR_LEN];                  /**< IP address of the remote target. Can be IPV4 or IPV6 address. */
} ST87EC_Lib_TransferCfg_t;


typedef struct
{
  char * pHost;                                             /**< Address of the remote target (can be of IPV4, IPV6 or URL type)    */
  ST87EC_Lib_AddressType_t AddressType;                     /**< Type of address (IPV4, IPV6 or URL)                                */
  uint32_t PortNb;                                          /**< UDP or TCP port number                                             */
  char * pDataTx;                                           /**< Pointer to the data to transfer (maximum is 512 bytes)             */
  uint16_t DataTxLength;                                    /**< Length of the data to transfer (can takes value from 1 to 512)     */
  ST87EC_Lib_LastPacket_t LastPacket;                       /**< Indicate if it is the last data chunk to transfer (1) or not (0)   */
  ST87EC_Lib_UdpTcpTransferReadCallback_t * p_TransferCallbackFunc;  /**< Parameter of the UDP transfer API,
                                                                giving a ptr to the return callback containing the data read.       */
  uint32_t TimeoutMs;                                       /**< Parameter of the GetTime API, giving the timeout
                                                                in ms after which the sequence is aborted if no
                                                                is available.                                                       */
} ST87EC_Lib_UdpTcpTransfer_Params_t;


typedef struct
{
  ST87EC_Lib_UdpTcpTransfer_FsmState_t FsmState;                       /**< Current status of the UDP or TCP transfer FSM.              */
  ST87EC_Lib_UdpTcpTransfer_Params_t Params;                           /**< Structure containing the parameters of the UdpTransfer API. */
  uint32_t TimerId;                                                    /**< Timer Id for the running timeout                            */
  ST87EC_Lib_UdpTcpTransferTcpConn_SubFsmState_t TcpConnSubFsmState;   /**< Current status of the TCP connect proc sub FSM.             */
  ST87EC_Lib_UdpTcpTransferDtlsConn_SubFsmState_t DtlsConnSubFsmState; /**< Current status of the DTLS connect proc sub FSM.            */
  ST87EC_Lib_UdpTcpTransferEnd_SubFsmState_t EndSubFsmState;           /**< Current status of the UDP/TCP transfer ending proc sub FSM. */
} ST87EC_Lib_UdpTcpTransfer_t;

typedef struct{
  char * BrokerAddress;                                     /**< IPv4 or IPv6 address of the remote Broker.                                   */
  ST87EC_Lib_AddressType_t AddressType;                     /**< Type of address (IPV4, IPV6 or URL)                                          */
  uint32_t BrokerPort;                                      /**< Port number of the remote Broker.                                            */
  char * Topic;                                             /**< Is the topic name. It shall not include any wildcard characters.
                                                                  Maximum length is 50.                                                       */
  char * Message;                                           /**< Is the payload containing the application message that is being
                                                                  published. Maximum length is 50.                                            */
  char * pTopicsQosList;                                    /**< The list of topics and associated QoS to subscribe
                                                                 in the topic1,qos1,topic2,qos2,... format.                                   */
  char * pTopicsList;                                       /**< The list of topics to unsubscribe in the topic1,topic2,... format.           */
  ST87EC_Lib_MqttReceiveCallback_t * pMqttRecvCallbackFunc; /**< Pointer to callback function containing the subscribed topic published data. */
  ST87EC_Lib_MqttDisconnect_t Disconnect;                   /**< Indicates if a MQTT session disconnect is required.                          */
  uint32_t TimeoutMs;                                       /**< Parameter giving the timeout in ms after which the sequence is aborted.      */
} ST87EC_Lib_MqttSession_Params_t;

typedef struct
{
  ST87EC_Lib_MqttSession_FsmState_t FsmState;               /**< Current status of the MqttPublish FSM.                      */
  ST87EC_Lib_MqttSession_Params_t Params;                   /**< Structure containing the parameters of the MqttPublish API. */
  ST87EC_Lib_MqttStatus_t Status;                           /**< Connection status with the MQTT Broker server.              */
  ST87EC_Lib_MqttType_t Type;
  uint32_t TimerId;                                         /**< Timer Id for the running timeout.                           */
  ST87EC_Lib_MqttSession_Command_t Command;                 /**< Manage the command to be sent.                              */
  ST87EC_Lib_MqttSession_SubscriptionState_t SubscriptionState;
  uint32_t DelayTimerId;                                    /**< Timer Id for the delay.                                     */
  uint32_t DelayTimerValueMs;                               /**< Timer value for the delay.                                  */
} ST87EC_Lib_MqttSession_t;

typedef struct
{
  ST87EC_Lib_SocketCreation_FsmState_t FsmState;               /**< Current status of the Socket Creation FSM.                */

} ST87EC_Lib_SocketCreation_t;

typedef struct
{
  char * pIpAddress;                                            /**< IP address of the remote target. Can be IPV4 or IPV6 address.           */
  ST87EC_Lib_AddressType_t AddressType;                         /**< Type of address (IPV4, IPV6 or URL).                                    */
  uint32_t PortNb;                                              /**< CoAP port number.                                                       */
  ST87EC_Lib_CoapTxObject_t CoapTxParams;                       /**< Parameters for CoAP Transmit.                                           */
  ST87EC_Lib_CoapReceiveCallback_t * pCoapReceiveCallbackFunc;  /**< Pointer on the callback function called upon data reception.            */
  ST87EC_Lib_CoapRxData_t * pCoapRxData;                        /**< Pointer on the received data structure.                                 */
  uint32_t Timeout;                                             /**< Parameter giving the timeout in ms after which the sequence is aborted. */
} ST87EC_Lib_CoapTransfer_Params_t;

typedef struct
{
  ST87EC_Lib_CoapOpen_FsmState_t FsmState;                      /**< Current status of the CoAP open FSM.                           */
  ST87EC_Lib_CoapOpen_Command_t Command;                        /**< CoAP Open on-going command to ST87.                            */
} ST87EC_Lib_CoapOpen_t;

typedef struct
{
  ST87EC_Lib_CoapTx_FsmState_t FsmState;                        /**< Current status of the CoAP Transmit FSM.                       */
} ST87EC_Lib_CoapTx_t;

typedef struct
{
  ST87EC_Lib_CoapClose_FsmState_t FsmState;                     /**< Current status of the CoAP close FSM.                          */
  ST87EC_Lib_CoapClose_Command_t Command;                       /**< CoAP Close on-going command to ST87.                           */
} ST87EC_Lib_CoapClose_t;

typedef struct
{
  ST87EC_Lib_CoapTransfer_FsmState_t FsmState;                  /**< Current status of the CoAP transfer FSM.                       */
  ST87EC_Lib_CoapTransfer_Params_t Params;                      /**< Structure containing the parameters of the CoAP transfer API.  */
  uint32_t TimerId;                                             /**< Timer Id for the running timeout                               */
  ST87EC_Lib_CoapOpen_t CoapOpen;                               /**< Structure containing CoAP Open elements.                       */
  ST87EC_Lib_CoapTx_t CoapTx;                                   /**< Structure containing CoAP Transmit elements.                   */
  ST87EC_Lib_CoapRx_State_t CoapRxState;                        /**< Current state of the CoAP URC data reception.                  */
  ST87EC_Lib_CoapClose_t CoapClose;                             /**< Structure containing CoAP Close elements.                      */
} ST87EC_Lib_CoapTransfer_t;

typedef struct {
    char * pField;                                              /**< HTTP field selected for header                                 */
    char * pValue;                                              /**< HTTP value to field selected for header                        */
} ST87EC_Lib_HttpTransfer_Header_t;

typedef struct
{
  char * pHttpRawInStrCopy;                                             /**< Pointer to internal copy of HTTP input raw data string.                    */
  char * pUrl;                                                          /**< URL address of the remote target. (ex: st.com)                             */

  char * pMethod;                                                       /**< Pointer to the string containing the HTTP Method                           */
  char * pPath;                                                         /**< Pointer to the string containing the folder of HTTP server                 */
  char * pBody;                                                         /**< Pointer to the string containing the data                                  */
  char * pPayload;                                                      /**< Pointer to the received data string.                                       */
  uint8_t KeepAlive;                                                    /**< 0 : Connection can be released when data is received by the server
                                                                             1 : Say to http server to keep alive the connection                        */
  ST87EC_Lib_HttpTransfer_Header_t Header;                              /**< Structure of the header part                                               */
  ST87EC_Lib_HttpTransferReadCallback_t * HttpTransferReadCallbackFunc; /**< Callback function that contains the data read.                             */
  uint32_t Timeout;                                                     /**< Parameter giving the timeout in ms after which the sequence is aborted     */
} ST87EC_Lib_HttpTransfer_Params_t;

typedef struct
{
  ST87EC_Lib_HttpOpen_FsmState_t FsmState;                   /**< Current status of the HTTP open FSM.                           */
  ST87EC_Lib_HttpOpen_Command_t Command;                     /**< CoAP Open on-going command to ST87.                            */
} ST87EC_Lib_HttpOpen_t;

typedef struct
{
  ST87EC_Lib_HttpClose_FsmState_t FsmState;                  /**< Current status of the HTTP close FSM.                          */
  ST87EC_Lib_HttpClose_Command_t Command;                    /**< HTTP Close on-going command to ST87.                           */
} ST87EC_Lib_HttpClose_t;

typedef struct
{
  ST87EC_Lib_HttpRxTx_FsmState_t FsmState;                   /**< Current status of the HTTP method FSM.                       */
  ST87EC_Lib_HttpRxTx_Command_t Command;                     /**< HTTP Method on-going command to ST87.                        */
  union                                                      /**< Struct and vars managing the HTTP header and body data chunks received from ST87. */
  {
    struct
    {
      uint8_t HeaderFound:1;
      uint8_t ContentLengthFound:1;
      uint8_t unused:6;
    } Bit;
    uint8_t Value;
  } Flag;
  uint16_t HeaderLen;
  uint16_t BodyLen;
  uint16_t ContentLen;
  uint16_t ReadIdx;
  uint16_t TotReadLen;
} ST87EC_Lib_HttpRxTx_t;

typedef struct
{
  ST87EC_Lib_HttpTransfer_FsmState_t FsmState;               /**< Current status of the HTTP transfer FSM.                           */
  ST87EC_Lib_HttpTransfer_Params_t Params;                   /**< Structure containing the parameters of the HTTP transfer API.      */
  uint32_t TimerId;                                          /**< Timer Id for the running timeout                                   */
  char * pHttpInData;                                        /**< Parsing pointer to current data element within HTTP raw input data */
  ST87EC_Lib_HttpOpen_t HttpOpen;                            /**< Structure containing HTTP Open elements.                           */
  ST87EC_Lib_HttpClose_t HttpClose;                          /**< Structure containing HTTP Close elements.                          */
  ST87EC_Lib_HttpRxTx_t HttpRxTx;                            /**< Structure containing HTTP Rx and Tx elements.                      */
} ST87EC_Lib_HttpTransfer_t;

typedef struct
{
  ST87EC_Lib_AddressType_t IpVersion;                                              /**< IP version for the LwM2M session                                                                      */
  ST87EC_Lib_RegistrationReq_t RegisterOption;                                     /**< Registration option requested (for ST87EC_Lib_NBIOT_Lwm2mRegister API)                                */
  char * pUri;                                                                     /**< Pointer on the URI of the LwM2M built-in object to read or write                                      */
  char * pData;                                                                    /**< Pointer on the LwM2M built-in object data to write                                                    */
  ST87EC_Lib_Lwm2mObjDataRecvCallback_t * pLwm2mObjDataRecvCallbackFunc;           /**< Pointer on the callback called upon LwM2M object data (read or list) reception                        */
  char * pObjectList;                                                              /**< Pointer on the string list of LwM2M custom object information (for custom object creation)            */
  uint32_t ObjectId;                                                               /**< Custom object Id (for custom object removal)                                                          */
  char * pNotificationData;                                                        /**< Custom object info data to be used for notification of R/W/X action on custom object                  */
  ST87EC_Lib_Lwm2mCustObjectNotifyCallback_t * pLwm2mCustObjectNotifyCallbackFunc; /**< Pointer on the callback called upon notification of a R/W or X action on a LwM2M custom object
                                                                                          (to be used in case of use of custom object, set to 0 otherwise)                                    */
  uint32_t ServerNotifyTimeout;                                                    /**< Timeout in ms between user notification of a custom object R/W/X action from server and
                                                                                          user response to this notification (to be used in case of use of custom object, set to 0 otherwise) */
  uint32_t Timeout;                                                                /**< Parameter giving the timeout in ms after which the sequence is aborted                                */
} ST87EC_Lib_Lwm2mSession_Params_t;

typedef struct
{
  ST87EC_Lib_Lwm2mOpen_FsmState_t FsmState;                   /**< Current status of the LwM2M open FSM.                           */
  uint8_t StackState;                                         /**< LwM2M stack state.                                              */
} ST87EC_Lib_Lwm2mOpen_t;

typedef struct
{
  ST87EC_Lib_Lwm2mOperate_FsmState_t FsmState;                /**< Current status of the LwM2M operate FSM.                        */
  ST87EC_Lib_Lwm2mOperate_OpType_t OpType;                    /**< Operation type for the LwM2M operate FSM.                       */
} ST87EC_Lib_Lwm2mOperate_t;

typedef struct
{
  ST87EC_Lib_Lwm2mClose_FsmState_t FsmState;                  /**< Current status of the LwM2M close FSM.                          */
} ST87EC_Lib_Lwm2mClose_t;

typedef struct
{
  ST87EC_Lib_Lwm2mSession_FsmState_t FsmState;                /**< Current status of the LwM2M session FSM.                        */
  ST87EC_Lib_Lwm2mSession_Params_t Params;                    /**< Structure containing the parameters of the LwM2M session API.   */
  uint32_t TimerId;                                           /**< Timer Id for the running timeout.                               */
  ST87EC_Lib_Lwm2mOpen_t Lwm2mOpen;                           /**< Structure containing LwM2M open elements.                       */
  ST87EC_Lib_Lwm2mOperate_t Lwm2mOperate;                     /**< Structure containing LwM2M operate elements.                    */
  ST87EC_Lib_Lwm2mClose_t Lwm2mClose;                         /**< Structure containing LwM2M close elements.                      */
} ST87EC_Lib_Lwm2mSession_t;

typedef struct
{
  ST87EC_Lib_TransferCfg_t TransferCfg;                       /**< Structure containing the information of the transfer config.    */
  ST87EC_Lib_UdpTcpTransfer_t UdpTcpTransfer;                 /**< Structure containing the information of the UDP or TCP API.     */
  ST87EC_Lib_SocketCreation_t SocketCreation;                 /**< Structure containing the information of the socket creation.    */
  ST87EC_Lib_SocketClose_FsmState_t SocketCloseFsmState;      /**< Socket closing sub-FSM state                                    */
  ST87EC_Lib_MqttSession_t MqttSession;                       /**< Structure containing the information of the MqttPublish API.    */
  ST87EC_Lib_CoapTransfer_t CoapTransfer;                     /**< Structure containing the information of the CoAP API.           */
  ST87EC_Lib_HttpTransfer_t HttpTransfer;                     /**< Structure containing the information of the HTTP API.           */
  ST87EC_Lib_Lwm2mSession_t Lwm2mSession;                     /**< Structure containing the information of the LwM2M API.          */
} ST87EC_Lib_SequenceNbiotStatus_t;

typedef struct
{
  char Data[ST87EC_RAW_BUFFER_SIZE + 1];                     /**< ST87 incoming message raw data buffer.                       */
  uint16_t ExpectedLength;                                   /**< Raw data expected length of ST87 incoming message.           */
  uint16_t ReceivedLength;                                   /**< Raw data received length of ST87 incoming message.           */
} ST87EC_Lib_ExpectedRawData_t;

typedef struct
{
  char Data[ST87EC_CME_ERR_DATA_BUF_SIZE + 1];               /**< ST87 incoming CME error data buffer.                       */
  uint16_t CmeDataLength;                                    /**< Length of ST87 incoming CME error data received.           */
} ST87EC_Lib_CmeErrData_t;

typedef struct{
  ST87EC_Lib_WmbusTransfer_FsmState_t FsmState;
  uint8_t CmdIdx;
  uint32_t TimerId;
  void * UsrPtr;
  ST87EC_Lib_WmbusCallback_t Callback;
  uint8_t NAcc;
  uint8_t DataBuffer[WMBUS_TX_BUFFER_SIZE];
  uint16_t DataLen;
  ST87EC_Lib_LastPacket_t LastPacket;
  uint8_t Asynch;
  struct
  {
    uint8_t id;
    int32_t value;
  }LastError;
} ST87EC_Lib_WmbusTransfer_t;

typedef struct{
  ST87EC_Lib_WmbusTransfer_t Transfer;
} ST87EC_Lib_SequenceWmbusStatus_t;


typedef struct
{
  ST87EC_Lib_WifiScan_FsmState_t FsmState;                   /**< Current status of the Wifi scanning FSM.                         */
  ST87EC_Lib_WifiScanObject_t Params;                        /**< Structure containing the parameters of the Wifi scan API.        */
  uint32_t TimerId;                                          /**< Timer Id for the running timeout.                                */
  uint32_t IterCnt;                                          /**< Counter of the number of scan round iterations already performed.*/
  uint8_t Started;                                           /**< Indication that a Wifi scanning is on-going.                     */
} ST87EC_Lib_WifiScan_t;

typedef struct
{
  ST87EC_Lib_WifiScan_t Scan;                                /**< Structure containing the information of the Wifi scan sequence.  */
} ST87EC_Lib_SequenceWifiStatus_t;


typedef struct
{
  ST87EC_Lib_SentAtToBootSteps_t SendAtToBootStep;           /**< Sub FSM states managing the Boot AT command response protocol               */
  uint32_t AtRspWaitingTimerId;                              /**< Id of the timer waiting for Boot response to AT cmd                         */
  uint32_t HandshakeRspWaitingTimerId;                       /**< Id of the timer waiting for Boot handshake response to                      */
  uint32_t HandshakeOkTrialsCnt;                             /**< Counter for handshake OK trial (for which no AT rsp is received from Boot)  */
  uint32_t HandshakeKoTrialsCnt;                             /**< Counter for handshake KO trial (for which no Ack rsp is received from Boot) */
  bool LastMsgBeforeBootUpload;                              /**< Flag indicating imminent switch of Boot into Upload mode                    */
  uint32_t NbBytesUploaded;                                  /**< Counter of the number of bytes sent to ST87 Boot during Upload              */
} ST87EC_Lib_BootInterface_t;

typedef struct {
  ST87EC_Lib_AtCmdSend_FsmState_t FsmState;                  /**< Current status of the single AT command sending FSM.                      */
  ST87EC_Lib_AtCmdObject_t Params;                           /**< Structure containing the parameters of the single AT command sending API. */
  uint32_t TimerId;                                          /**< Timer Id for the running timeout.                                         */
} ST87EC_Lib_SeqAtCmdSendStatus_t;

typedef struct {
  ST87EC_Lib_AsyncUrcRcv_FsmState_t FsmState;                /**< Current status of the asynchronous URC reception FSM.                      */
  ST87EC_Lib_AsyncUrcObject_t Params;                        /**< Structure containing the parameters of the asynchronous URC reception API. */
  uint32_t TimerId;                                          /**< Timer Id for the running timeout.                                          */
} ST87EC_Lib_SeqAsyncUrcRcvStatus_t;

typedef struct {
  int8_t SecureId;                                           /**< TLS Provisioning secure profile ID parameter.                              */
  union                                                      /**< TLS Provisioning element type parameter treated by the sequence.           */
  {
    ST87EC_Lib_TlsImportElemTypes_t ImportElementType;
    ST87EC_Lib_TlsGenerateElemTypes_t GenerateElementType;
    ST87EC_Lib_TlsListElemTypes_t ListElementType;
    ST87EC_Lib_TlsDelElemTypes_t DelElementType;
    ST87EC_Lib_TlsDumpElemTypes_t DumpElementType;
  };
  ST87EC_Lib_TlsProvEccTypes_t EccType;                      /**< TLS Provisioning Elliptic Curve Crypto parameter.                                */
  uint16_t InputLength;                                      /**< TLS Provisioning length of input data parameter.                                 */
  char * pInputData;                                         /**< TLS Provisioning input data string parameter.                                    */
  ST87EC_Lib_TlsProvCallback_t * pTlsProvCallback;           /**< TLS Provisioning callback parameter giving back the collected data from ST87Mxx. */
  uint32_t Timeout;                                          /**< Timer Id for the running timeout.                                                */
} ST87EC_Lib_TlsProv_Params_t;

typedef struct {
  ST87EC_Lib_TlsProv_FsmState_t FsmState;                    /**< Current status of the TLS Provisioning sequence FSM.                       */
  ST87EC_Lib_TlsProv_Params_t Params;                        /**< Structure containing the parameters for the TLS Provisioning sequence   . */
  uint32_t TimerId;                                          /**< Timer Id for the running timeout.                                         */
} ST87EC_Lib_SeqTlsProvisioningStatus_t;

typedef struct {
  ST87EC_Lib_SequenceSysStatus_t SequenceSys;                /**< Structure containing the information of the System sequences.           */
  ST87EC_Lib_SequenceGnssStatus_t SequenceGnss;              /**< Structure containing the information of the GNSS sequences.             */
  ST87EC_Lib_SequenceNbiotStatus_t SequenceNbiot;            /**< Structure containing the information of the NBIOT sequences.            */
  ST87EC_Lib_SequenceWmbusStatus_t SequenceWmbus;            /**< Structure containing the information of the WMBUS sequences.            */
  ST87EC_Lib_SequenceWifiStatus_t SequenceWifi;              /**< Structure containing the information of the Wifi sequences.             */
  ST87EC_Lib_Status_t ModuleStatus;                          /**< Current status of the ST87M01 module.                                   */
  ST87EC_Lib_SequenceValue_t OnGoingSequence;                /**< Status of the current sequence.                                         */
  ST87EC_Lib_SequenceValue_t PreviousSequence;               /**< Status of the previous sequence.                                        */
  ST87EC_Lib_GenericErrorCallback_t * pGenericErrorCallback; /**< Pointer to EC Lib generic error callback.                               */
  ST87EC_Lib_RspInfo_t RspInfo[MAX_NB_OF_RSP];               /**< Information on the received Response.                                   */
  char RspData[TOTAL_DATA_LENGTH + 1];                       /**< Received Response data.                                                 */
  uint8_t ErrorOccurredInSeq;                                /**< Set to 1 if an error occurred in the current sequence                   */
  ST87EC_Lib_SpecificResponse_t RspReceived;                 /**< Last received specific response.                                        */
  ST87EC_Lib_SpecificResponse_t NbSentReceived;              /**< Specific NB_SENT response received.                                     */
  ST87EC_Lib_ExpectedRawData_t RawDataRsp;                   /**< ST87 incoming message raw data information.                             */
  ST87EC_Lib_CmeErrData_t CmeDataRsp;
  uint8_t ColdInitVersion;                                   /**< Cold init parameters version.                                           */
  ST87EC_Lib_BootMode_t BootMode;                            /**< Mode in which ST87 system is launched                                   */
  ST87EC_Lib_BootInterface_t BootIf;                         /**< Structure of elements managing the interface with ST87 Boot             */
  ST87EC_Lib_SeqAtCmdSendStatus_t SeqAtCmdSend;              /**< Structure containing the information of the single AT command sending.  */
  ST87EC_Lib_SeqAsyncUrcRcvStatus_t SeqAsyncUrcRcv;          /**< Structure containing the information of the async URC reception.        */
  ST87EC_Lib_SeqTlsProvisioningStatus_t SeqTlsProvisioning;  /**< Structure containing the information of the TLS Provisioning sequences  */
  uint8_t WakeUpTriggered;                                   /**< Set to 1 if a wakeup of ST87 is needed (allow one-shot wakeup req.)     */
} ST87EC_Lib_LocalStatus_t;

/* Global variables ---------------------------------------------------------------------*/
extern ST87EC_Lib_LocalStatus_t EcLibVars;
extern const ST87EC_Lib_RspTab_t EcLibRspTab[];

/* Exported functions -------------------------------------------------------------------*/
ST87EC_Lib_Result_t ST87EC_Lib_SequenceMainInit(void);
ST87EC_Lib_Result_t ST87EC_Lib_SequenceMain(void);
uint32_t ST87EC_Lib_ComputeHash(const char * pString, uint16_t Len);
ST87EC_Lib_Result_t ST87EC_Lib_HandleGenericRsp(char * pTagData, uint16_t RspTagLen);
ST87EC_Lib_Result_t ST87EC_Lib_HandleSpecificRsp(ST87EC_Lib_SpecificResponse_t Response);
ST87EC_Lib_Result_t ST87EC_Lib_HandleRawRsp(void);
ST87EC_Lib_Result_t ST87EC_Lib_TrigColdParamInit(void);
ST87EC_Lib_Result_t ST87EC_Lib_StoreCmeErrData(uint16_t RawRspLen);
uint16_t ST87EC_Lib_GetCmeErrorCode(void);


#endif /* ST87EC_ENGINE_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
