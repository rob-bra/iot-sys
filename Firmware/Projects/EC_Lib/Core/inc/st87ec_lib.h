/**
  ******************************************************************************
  * @file    st87ec_lib.h
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
#ifndef ST87EC_LIB_H
#define ST87EC_LIB_H
#include <stdint.h>
#include "st87ec_lib_conf.h"

/* Exported constants and macros-----------------------------------------------*/
/**
 * ST87 EC library version
 */
#define ST87EC_LIB_MAJOR_VERSION                                           ("1")
#define ST87EC_LIB_MINOR_VERSION                                           ("8")
#define ST87EC_LIB_INTERNAL_VERSION                                        ("0")


#define ST87EC_LIB_TCPIP_IPV6_MAX_TEXTUAL_ADDRESS                    (8 * 4 + 7)
#define ST87EC_LIB_COAP_TOKEN_STR_MAX_LEN                                (8 + 1)
#define ST87EC_LIB_COAP_OPTION_STR_MAX_LEN                             (512 + 1)
#define ST87EC_LIB_COAP_PAYLOAD_STR_MAX_LEN                            (512 + 1)


#define ST87EC_LIB_HTTP_HEADERS_NB_MAX                                     (20)

/* Types ---------------------------------------------------------------------*/

/** 
 * Output value of the API functions.
 */
typedef enum{
  RESULT_OK = 0,                                  /**< Function execution status is OK.       */
  RESULT_KO = 1,                                  /**< Function execution status is KO.       */
  RESULT_BUSY = 2,                                /**< Function execution status is busy      */
  RESULT_BAD_PARAM = 3,                           /**< Function gets bad parameter            */
  RESULT_BAD_SEQUENCING = 4                       /**< Function gets bad FSM state sequencing */
} ST87EC_Lib_Result_t;

/** 
 * Output value of the SIM status.
 */
typedef enum{
  SIM_STATUS_SIM_INVALID = 0,                     /**< SIM is invalid.            */
  SIM_STATUS_SIM_VALID   = 1,                     /**< SIM is valid.              */
  SIM_STATUS_UNKNOWN     = 0xFF,                  /**< SIM status is unknown.     */
} ST87EC_Lib_SimStatus_t; 

/**
 * Output value of the module initialization.
 */
typedef enum{
  INIT_NOT_COMPLETE = 0,                          /**< Module initialization is not complete. */
  INIT_COMPLETE = 1,                              /**< Module initialization is complete.     */
} ST87Ec_Lib_InitComplete_t;

/** 
 * Output value of the connection status.
 */
typedef enum{
  CONN_STATUS_IDLE         = 0,                   /**< The stack is in IDLE state.          */
  CONN_STATUS_CONNECTED    = 1,                   /**< The stack is in CONNECTED state.     */
  CONN_STATUS_UNKNOWN      = 0xFF,                /**< The stack is in unknown state.       */
} ST87EC_Lib_ConnectionStatus_t; 

/**
 * Output value of the registration status.
 */
typedef enum{
  NOT_REGISTERED           = 0,                   /**< The module is not registered to the network.*/
  REGISTERED               = 1,                   /**< The module is registered to the network.    */
} ST87EC_Lib_RegistrationStatus_t;

/**
 * Output value of the HTTP connection status.
 */
typedef enum{
  HTTP_NOT_CONNECTED           = 0,               /**< No active HTTP connection.                  */
  HTTP_CONNECTED               = 1,               /**< An active HTTP connection exists.          */
} ST87EC_Lib_HttpConnectionStatus_t;

/**
 * Output value of the LwM2M registration status.
 */
typedef enum{
  LWM2M_REGISTER_NONE          = 0,               /**< LwM2M not registered or bootstrap not started              */
  LWM2M_REGISTER_INIT          = 1,               /**< LwM2M initial registration delay or delay between retries  */
  LWM2M_REGISTER_PENDING       = 2,               /**< LwM2M registration pending                                 */
  LWM2M_REGISTER_OK            = 3,               /**< LwM2M successfully registered                              */
  LWM2M_REGISTER_FAILED        = 4,               /**< LwM2M last registration failed                             */
  LWM2M_REGISTER_UPD_PENDING   = 5,               /**< LwM2M registration update pending                          */
  LWM2M_REGISTER_UPD_REQ       = 6,               /**< LwM2M registration update required                         */
  LWM2M_REGISTER_UPD_OBJ_REQ   = 7,               /**< LwM2M registration update with objects required            */
  LWM2M_REGISTER_DEREG_PENDING = 8,               /**< LwM2M deregistration pending                               */
} ST87EC_Lib_Lwm2mRegistrationStatus_t;

/**
 * Status of the Ring pin
 */
typedef enum {
  RINGPIN_DISABLE           = 0,                  /**< Disable the Ring pin                 */
  RINGPIN_ENABLE            = 1,                  /**< Enable the Ring pin                  */
} ST87EC_Lib_RingPinStatus_t;

/**
 * Indication if last data chunk to transfer
 */
typedef enum {
  LAST_PKT_FALSE                    = 0,          /**< It is not the last packet to transfer, do not close the link  */
  LAST_PKT_TRUE                     = 1,          /**< It is the last packet to transfer, close the link                 */
} ST87EC_Lib_LastPacket_t;

/** 
 * Sleep/wakeup status of the module.
 */
typedef enum{
  STATUS_SLEEP               = 0,                 /**< The Module is sleeping.              */
  STATUS_WAKEUP              = 1,                 /**< The module is woken up.              */
} ST87EC_Lib_SleepWakeupstatus_t;

/**
 * IP version indication.
 */
typedef enum{
  IPV4               = 0,                        /**< IP address version 4 is used.              */
  IPV6               = 1,                        /**< IP address version 6 is used.              */
  URL                = 2,                        /**< URL is used.                               */
} ST87EC_Lib_AddressType_t;

/**
 * List of possible sequences.
 */
typedef enum{
  SEQUENCE_NONE                  = 0,              /**< No sequence running.                                                 */
  SEQUENCE_COLD_INIT             = 1,              /**< Registration of the EC Cold parameter init request.                  */
  SEQUENCE_GET_TIME              = 2,              /**< Registration of the EC Get Time request.                             */
  SEQUENCE_GET_STATE             = 3,              /**< Registration of the EC Get State request.                            */
  SEQUENCE_GNSS_GET_FIX          = 4,              /**< Registration of the EC GNSS Get Fix request.                         */
  SEQUENCE_NBIOT_MQTT_SESSION    = 5,              /**< Registration of the EC NBIOT MQTT Publish request.                   */
  SEQUENCE_UDP_TRANSFER          = 6,              /**< Registration of the UDP transfer request.                            */
  SEQUENCE_TCP_TRANSFER          = 7,              /**< Registration of the TCP transfer request.                            */
  SEQUENCE_COAP_OPEN             = 8,              /**< Registration of the CoAP open request.                               */
  SEQUENCE_COAP_RXTX             = 9,              /**< Registration of the CoAP transmit or receive request.                */
  SEQUENCE_COAP_CLOSE            = 10,             /**< Registration of the CoAP close request.                              */
  SEQUENCE_HTTP_OPEN             = 11,             /**< Registration of the HTTP open request.                               */
  SEQUENCE_HTTP_TRANSFER         = 12,             /**< Registration of the HTTP transfer request.                           */
  SEQUENCE_HTTP_CLOSE            = 13,             /**< Registration of the HTTP close request.                              */
  SEQUENCE_WMBUS_TRANSFER        = 14,             /**< Registration of the EC WMBUS transfer request.                       */
  SEQUENCE_WIFI_SCANNING         = 15,             /**< Registration of the EC Wi-fi scanning request.                       */
  SEQUENCE_LOADER                = 16,             /**< Registration of the Loader request.                                  */
  SEQUENCE_ATCMD_SEND            = 17,             /**< Registration of the unitary AT command send request.                 */
  SEQUENCE_ASYNC_URC_RCV         = 18,             /**< Registration of the async URC reception en/disabling request.        */
  SEQUENCE_TLS_PROV_IMPORT_ELEM  = 19,             /**< Registration of the TLS Provisioning request for importing element.  */
  SEQUENCE_TLS_PROV_GEN_ELEM     = 20,             /**< Registration of the TLS Provisioning request for generating element .*/
  SEQUENCE_TLS_PROV_DEL_ELEM     = 21,             /**< Registration of the TLS Provisioning request for deleting element.   */
  SEQUENCE_TLS_PROV_LIST_ELEM    = 22,             /**< Registration of the TLS Provisioning request for listing element.    */
  SEQUENCE_TLS_PROV_DUMP_ELEM    = 23,             /**< Registration of the TLS Provisioning request for dumping element.    */
  SEQUENCE_LWM2M_OPEN            = 24,             /**< Registration of the LwM2M session open request.                      */
  SEQUENCE_LWM2M_OPERATION       = 25,             /**< Registration of the sequence treating a LwM2M operation on an object.*/
  SEQUENCE_LWM2M_CLOSE           = 26,             /**< Registration of the LwM2M session close request.                     */
} ST87EC_Lib_SequenceValue_t;

/**
 * EC Lib internal error codes.
 */
typedef enum{
  ERROR_NO_CODE                  = 0,              /**< No error code specified                                 */
  ERROR_ST87_WATCHDOG            = (-1),           /**< Error code indicating ST87 raised a watchdog            */
} ST87EC_Lib_ErrorCode_t;

typedef enum {
  MQTT_DONT_DISCONNECT           = 0,              /**< Do not require a MQTT disconnect                        */
  MQTT_DISCONNECT                = 1,              /**< Require a MQTT disconnect                               */
} ST87EC_Lib_MqttDisconnect_t;

typedef enum{
  ST87_SEC_BINARY                = 0,              /**< Binary to load to ST87 Boot is: SEC                     */
  ST87_ALP_BINARY                = 1,              /**< Binary to load to ST87 Boot is: ALP                     */
  ST87_MODEM_BINARY              = 2,              /**< Binary to load to ST87 Boot is: MODEM                   */
  ST87_GNSS_BINARY               = 3,              /**< Binary to load to ST87 Boot is: GNSS                    */
  ST87_CAP_BINARY                = 4,              /**< Binary to load to ST87 Boot is: CAP                     */
} ST87EC_Lib_BinToLoad_t;

typedef enum{
  ST87_BIN_DATA_TRANSFER_OK      = 0,              /**< Binary transfer status OK user side for EC Lib ST87 Loader   */
  ST87_BIN_DATA_TRANSFER_KO      = 1,              /**< Binary transfer status KO user side for EC Lib ST87 Loader   */
  ST87_BIN_DATA_TRANSFER_BUSY    = 2,              /**< Binary transfer status BUSY user side for EC Lib ST87 Loader */
} ST87EC_Lib_BinTransferStatus_t;

typedef enum{
  ASYNC_URC_RCV_ENABLE           = 0,              /**< Enable asynchronous URC reception                       */
  ASYNC_URC_RCV_DISABLE          = 1,              /**< Disable asynchronous URC reception                      */
} ST87EC_Lib_AsyncUrcRcvEnabling_t;

typedef enum{
  TLS_PROV_ELEM_DEVICE_CERT      = 0,              /**< Device certificate TLS Provisioning element             */
  TLS_PROV_ELEM_ROOT_CERT        = 1,              /**< CA root certificate TLS Provisioning element            */
  TLS_PROV_ELEM_PSK_ID           = 2,              /**< Pre-Shared Key identity TLS Provisioning element        */
  TLS_PROV_ELEM_PRIV_KEY         = 3,              /**< Private key TLS Provisioning element                    */
  TLS_PROV_ELEM_PSK              = 4,              /**< Pre-Shared Key TLS Provisioning element                 */
} ST87EC_Lib_TlsImportElemTypes_t;

typedef enum{
  TLS_PROV_ECC_SECP_R1           = 0,              /**< Elliptic-curve cryptography curse type SECP R1          */
  TLS_PROV_ECC_BRAINPOOLP_R1     = 1,              /**< Elliptic-curve cryptography curse type BrainpoolP R1    */
} ST87EC_Lib_TlsProvEccTypes_t;

typedef enum{
  TLS_PROV_ELEM_DEV_PRIV_KEY     = 0,              /**< Device private key TLS Provisioning element             */
  TLS_PROV_ELEM_CSR              = 1,              /**< Certificate Signing Request TLS Provisioning element    */
} ST87EC_Lib_TlsGenerateElemTypes_t;

typedef enum{
  TLS_PROV_DEL_ELEM_DEVICE_CERT = 0,               /**< Device certificate TLS Provisioning element             */
  TLS_PROV_DEL_ELEM_ROOT_CERT   = 1,               /**< CA root certificate TLS Provisioning element            */
  TLS_PROV_DEL_ELEM_PSK_ID      = 2,               /**< Pre-Shared Key identity TLS Provisioning element        */
  TLS_PROV_DEL_ELEM_ALL_CERTS   = 3,               /**< TLS Provisioning all certificates (for a SecId)         */
  TLS_PROV_DEL_ELEM_KEY         = 4,               /**< TLS Provisioning key (for a SecId)                      */
} ST87EC_Lib_TlsDelElemTypes_t;

typedef enum{
  TLS_PROV_ELEM_KEYS            = 0,               /**< Key type TLS Provisioning element                       */
  TLS_PROV_ELEM_CERTS           = 1,               /**< Certificate type TLS Provisioning element               */
} ST87EC_Lib_TlsListElemTypes_t;

typedef enum{
  TLS_PROV_DUMP_ELEM_DEVICE_CERT = 0,              /**< Device certificate TLS Provisioning element             */
  TLS_PROV_DUMP_ELEM_ROOT_CERT   = 1,              /**< CA root certificate TLS Provisioning element            */
  TLS_PROV_DUMP_ELEM_PSK_ID      = 2,              /**< Pre-Shared Key identity TLS Provisioning element        */
  TLS_PROV_DUMP_ELEM_PUB_KEY     = 3,              /**< Public key TLS Provisioning element                     */
} ST87EC_Lib_TlsDumpElemTypes_t;

typedef enum{
  LWM2M_DEREGISTER          = 0,                   /**< LwM2M registration option Deregister                    */
  LWM2M_REGISTER            = 1,                   /**<  LwM2M registration option Register                     */
} ST87EC_Lib_RegistrationReq_t;

typedef enum{
  LWM2M_NOTIFY_TYPE_READ    = 0,                   /**< LwM2M type of action to notify for a read of custom object(s)     */
  LWM2M_NOTIFY_TYPE_WRITE   = 1,                   /**< LwM2M type of action to notify for a write of custom object(s)    */
  LWM2M_NOTIFY_TYPE_EXECUTE = 2,                   /**< LwM2M type of action to notify for an execute of custom object(s) */
} ST87EC_Lib_Lwm2mNotifyType_t;

/** 
 * Structure for the status request.
 */
typedef struct{
  ST87Ec_Lib_InitComplete_t InitComplete;                       /**< Module initialization is complete                    */
  ST87EC_Lib_SimStatus_t SimStatus;                             /**< SIM status request value.                            */
  ST87EC_Lib_ConnectionStatus_t ConnectionStatus;               /**< NB-IOT connection status request value.              */
  ST87EC_Lib_RegistrationStatus_t RegistrationStatus;           /**< EPS network registration status value.               */
  ST87EC_Lib_SleepWakeupstatus_t SleepWakeupstatus;             /**< Sleep or Wakeup status value.                        */
  ST87EC_Lib_SequenceValue_t  OnGoingSequence;                  /**< Name of the current sequence                         */
  uint32_t NbUdpPacketsSent;                                    /**< Number of UDP packets sent counter
                                                                       (keeps 0 if NB_PACKET_SENT cold param value is 0)  */
  uint8_t TransferOnGoing;                                      /**< Indication that a data transfer (TCP, UDP,...)
                                                                                 is on-going (0: no / >0: yes) */
  ST87EC_Lib_HttpConnectionStatus_t HttpConnectionStatus;       /**< HTTP connection status feedback.                     */
  ST87EC_Lib_Lwm2mRegistrationStatus_t Lwm2mRegistrationStatus; /**< LwM2M client's registration status.                  */
} ST87EC_Lib_Status_t;

/**
* @brief Callback called to read the data sent by UDP transfer
*
* @param pString: pointer to the returned string (read only pointer).
*/
typedef void (ST87EC_Lib_UdpTcpTransferReadCallback_t) (char const * const pString);

/**
 * Structure for the interface for UDP transfer.
 */
typedef struct{
  char * pHost;                                                        /**< IP address or URL of the remote entity                */
  ST87EC_Lib_AddressType_t AddressType;                                /**< Type of address (IPV4, IPV6 or URL)                   */
  uint32_t PortNb;                                                     /**< Port number of the remote entity                      */
  char * pDataTx;                                                      /**< Pointer on the data to transmit                       */
  uint16_t DataTxLength;                                               /**< Length of the data to transmit, meaningful
                                                                              only in case of binary transfer.                    */
  int32_t SecureId;                                                    /**< TLS security profile Id (to indicate if a secure TCP
                                                                              or UDP connection is used, set to (-1) otherwise.   */
  ST87EC_Lib_UdpTcpTransferReadCallback_t * pTransferReadCallbackFunc; /**< Callback function that contains
                                                                              the data read.                                      */
  ST87EC_Lib_LastPacket_t LastPacket;                                  /**< Bit to indicate the link has to be closed.            */
  uint32_t TimeoutMs;                                                  /**< Timeout in ms after which the request is cancelled.   */
} ST87EC_Lib_UdpTcpObject_t;


/**
 * Interface structure for MQTT Publish.
 */
typedef char string_t[];

typedef struct{
  char * pIpAddress;                             /**< IPv4 or IPv6 address of the remote Broker.
                                                      (if a MQTT session is already on-going this param is ignored).   */
  ST87EC_Lib_AddressType_t AddressType;          /**< Type of address (IPV4, IPV6 or URL)
                                                      (if a MQTT session is already on-going this param is ignored).   */
  uint32_t PortNumber;                           /**< Port number of the remote Broker.
                                                      (if a MQTT session is already on-going this param is ignored).   */
  char * pTopic;                                 /**< Name of the topic to which the message will be sent.             */
  char * pMessage;                               /**< The content of the message to be sent.                           */
  ST87EC_Lib_MqttDisconnect_t Disconnect;        /**< Indicates if an MQTT disconnect is required (i.e. last publish). */
  uint32_t TimeoutMs;                            /**< Timeout in ms after which the request is cancelled.              */
} ST87EC_Lib_MqttPubObject_t;

/**
 * Interface structure for MQTT Subscribe.
 */
typedef void (ST87EC_Lib_MqttReceiveCallback_t) (char const * const pString);

typedef struct{
  char * pIpAddress;                                        /**< IPv4 or IPv6 address of the remote Broker.
                                                                 (if a MQTT session is already on-going this param is ignored).               */
  ST87EC_Lib_AddressType_t AddressType;                     /**< Type of address (IPV4, IPV6 or URL)
                                                                 (if a MQTT session is already on-going this param is ignored).               */
  uint32_t PortNumber;                                      /**< Port number of the remote Broker.
                                                                 (if a MQTT session is already on-going this param is ignored).               */
  char * pTopicsQosList;                                    /**< The list of topics and associated QoS to subscribe
                                                                 in the topic1,qos1,topic2,qos2,... format.                                   */
  ST87EC_Lib_MqttReceiveCallback_t * pMqttRecvCallbackFunc; /**< Pointer to callback function containing the subscribed topic published data. */
  uint32_t TimeoutMs;                                       /**< Timeout in ms after which the request is cancelled.                          */
} ST87EC_Lib_MqttSubscObject_t;


/**
 * Interface structure for MQTT Unsubscribe.
 */
typedef struct{
  char * pIpAddress;                             /**< IPv4 or IPv6 address of the remote Broker
                                                      (if a MQTT session is already on-going this param is ignored).        */
  ST87EC_Lib_AddressType_t AddressType;          /**< Type of address (IPV4, IPV6 or URL)
                                                      (if a MQTT session is already on-going this param is ignored).        */
  uint32_t PortNumber;                           /**< Port number of the remote Broker
                                                      (if a MQTT session is already on-going this param is ignored).        */
  char * pTopicsList;                            /**< The list of topics to unsubscribe in the topic1,topic2,... format.    */
  ST87EC_Lib_MqttDisconnect_t Disconnect;        /**< Indicates if an MQTT disconnect is required (i.e. last unsubscribe).  */
  uint32_t TimeoutMs;                            /**< Timeout in ms after which the request is cancelled.                   */
} ST87EC_Lib_MqttUnsubscObject_t;


/**
 * Interface structure for CoAP Tx transfer.
 */
typedef struct{
  char * pOption;                                /**< String containing the likely CoAP options to be appended.                            */
  uint8_t MessageType;                           /**< CoAP message Type (0: Confirmable, 1: Non-confirmable, 2: Acknowledgment, 3: Reset). */
  uint8_t Method;                                /**< CoAP method Type (1: GET, 2: POST, 3: PUT, 4: DELETE).                               */
  uint16_t Mid;                                  /**< CoAP message Id (integer value sequentially assigned).                               */
  uint8_t TxDataFormat;                          /**< Format type of the data to transmit (0: ASCII string, 2: Hex data in text mode).     */
  char * pToken;                                 /**< Pointer to the token Id (string of a hexadecimal random value).                      */
  char * pPath;                                  /**< Pointer to the string containing the resource URI path (max. length is 50 bytes).    */
  char * pPayload;                               /**< Pointer to the string data to transmit (max. length is 128 bytes).                   */
} ST87EC_Lib_CoapTxObject_t;


/**
 * Interface structure for CoAP Rx transfer.
 */
typedef struct{
  uint8_t Mid;                                   /**< Echoed CoAP message Id.                                                              */
  uint8_t MessageType;                           /**< CoAP message Type (0: Confirmable, 1: Non-confirmable, 2: Acknowledgment, 3: Reset). */
  uint16_t ResponseCode;                         /**< CoAP Response code.                                                                  */
  char * pToken;                                 /**< Echoed pointer to the token Id string (if available).                                */
  uint16_t OptionNb;                             /**< Number of defined options.                                                           */
  uint16_t PayloadLength;                        /**< Length of the payload in bytes.                                                      */
  char * pCurOptions;                            /**< String containing the current active options.                                        */
  char * pPayload;                               /**< Pointer to the received data string.                                                 */
} ST87EC_Lib_CoapRxData_t;


/**
 * Interface structure for HTTP transfer.
 */
typedef void (ST87EC_Lib_HttpTransferReadCallback_t) (char const * const pString);

typedef struct{
  char * pHttpRawInStr;                                        /**< Pointer to HTTP input raw data string.                                               */
                                                               /**<   This shall include method, header and potentially body.                            */
  ST87EC_Lib_HttpTransferReadCallback_t * pHttpRxCallbackFunc; /**< Pointer to callback function called by EC Lib at the end of the transfer.            */
  uint8_t KeepAlive;                                           /**< Connection Keep Alive information:                                                   */
                                                               /**<   0 : Indicate to HTTP server to released connection when data is received           */
                                                               /**<   1 : Indicate to HTTP server to keep alive the connection after data reception      */
  uint32_t Timeout;                                            /**< Timeout in ms after which the request is cancelled.                                  */
} ST87EC_Lib_HttpTransferObject_t;

typedef enum {
    WMBUS_NTY_Ready     ,   /**< keep-alive sent when device wakes up but there're no data to send */
    WMBUS_NTY_Sent      ,   /**< data has been successfully sent and sequence is ready to accept a new transfer */
    WMBUS_NTY_Done      ,   /**< sequence completed */
    WMBUS_NTY_Fail      ,   /**< fail during sequence */
}ST87EC_Lib_WmbusNty_t;

typedef void (*ST87EC_Lib_WmbusCallback_t) (ST87EC_Lib_WmbusNty_t Nty, void * UsrPtr);

typedef struct {
    void * UsrPtr;
    ST87EC_Lib_WmbusCallback_t Callback;
    const uint8_t * DataPtr;
    uint16_t DataLen;
    ST87EC_Lib_LastPacket_t LastPacket;
    uint8_t Asynch;                       /**< 0x00: synch transfer, !0x00: asynch transfer (send immediately) */
}ST87EC_Lib_WmbusObject_t;

/**
 * Interface structure for Wifi Scanning.
 */
typedef void (ST87EC_Lib_WIFI_GetBeaconDataCallback_t) (char const * const pString);

typedef struct {
  char * pChanList;                                                      /**< Pointer to the string list of Wifi channels.                                     */
  uint32_t NbScanIter;                                                   /**< Number of scan round iterations.                                                 */
  ST87EC_Lib_WIFI_GetBeaconDataCallback_t * pGetBeaconDataCallbackFunc;  /**< Pointer to callback function called by EC Lib each time beacon dat is available. */
  uint32_t TimeoutMs;                                                    /**< Timeout in ms after which the request is cancelled.                              */
} ST87EC_Lib_WifiScanObject_t;

/**
 * Interface structure for the unitary AT command sending interface.
 */
typedef void (ST87EC_Lib_AtRespCallback_t) (char const * const pString);

typedef struct {
  char * pCommand;                                          /**< Pointer to the AT command string                                             */
  char * pRspTag;                                           /**< Pointer to the AT response tag string                                        */
  ST87EC_Lib_AtRespCallback_t * pAtRespCallbackFunc;        /**< Callback function containing AT command response string                      */
  uint32_t RawDataLen;                                      /**< Number of raw chars expected to be received as AT response raw data          */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled.                          */
} ST87EC_Lib_AtCmdObject_t;

/**
 * Interface structure for the asynchronous URC reception interface.
 */
typedef void (ST87EC_Lib_AsyncUrcCallback_t) (char const * const pString);

typedef struct {
  ST87EC_Lib_AsyncUrcRcvEnabling_t UrcRcvEnabling;          /**< Sets the targeted action for the async URC reception: enable or disable       */
  char * pCommand;                                          /**< Pointer to the AT command string initiating the URC reception                 */
  char * pUrcTag;                                           /**< Pointer to the async URC tag string                                           */
  ST87EC_Lib_AsyncUrcCallback_t * pAsyncUrcCallbackFunc;    /**< Callback function containing the async URC data response string               */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled.                           */
} ST87EC_Lib_AsyncUrcObject_t;

/**
 * Interface structure for the TLS Provisioning interface.
 */
typedef void (ST87EC_Lib_TlsProvCallback_t) (int32_t const DataLen, char const * const pString);

typedef struct {
  int8_t SecureId;                                          /**< Secure Id: security profile ID linked to certificates/key identity                          */
  ST87EC_Lib_TlsImportElemTypes_t ElementType;              /**< TLS Provisioning elements (Certificate, key,..)                                             */
  ST87EC_Lib_TlsProvEccTypes_t EccType;                     /**< Elliptic curve type ( SECP R1 or BrainpoolP R1): only relevant for asymmetric key element   */
  uint16_t InputLength;                                     /**< Data input length (in bytes. Note: as input data is in Hex, length of corresponding
                                                                    data binary is 2*InputLength)                                                            */
  char * pInputData;                                        /**< Data input. Format:
                                                                    -  for the PSK and PSK_ID: RAW
                                                                    -  for the private key: DER or RAW (chosen by user, see in st87ec_config.h)
                                                                    -  for the Certificates: DER                                                             */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled                                          */
} ST87EC_Lib_TlsImportElemObject_t;

typedef struct {
  int8_t SecureId;                                          /**< Secure Id: security profile ID linked to certificate/key identity                           */
  ST87EC_Lib_TlsGenerateElemTypes_t ElementType;            /**< TLS Provisioning elements (private key, CSR)                                                */
  ST87EC_Lib_TlsProvEccTypes_t EccType;                     /**< Elliptic curve type (Prime SECP R1 or BrainpoolP R1): only relevant for private key element */
  uint16_t InputLength;                                     /**< Data input length (in bytes. Relevant only for CSR: contains dummy CSR data length.
                                                                     Note: as input data is in Hex, length of corresponding data binary is 2*InputLength)    */
  char * pInputData;                                        /**< Data input (relevant only for CSR: contains dummy CSR data, shall be in DER format)         */
  ST87EC_Lib_TlsProvCallback_t * pTlsProvCallbackFunc;      /**< Callback function returning the following element info:
                                                                  -  for device private key element: length of the returned public key data and the
                                                                     public key data (DER or RAW format)
                                                                  -  for the CSR: length of the CSR data output and the CSR content data (DER format)        */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled                                          */
} ST87EC_Lib_TlsGenerateElemObject_t;

typedef struct {
  int8_t SecureId;                                          /**< Secure Id: security profile ID linked to certificate/key identity                           */
  ST87EC_Lib_TlsDelElemTypes_t ElementType;                 /**< TLS Provisioning elements for deletion (keys, certificates)                                 */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled                                          */
} ST87EC_Lib_TlsDeleteElemObject_t;

typedef struct {
  int8_t SecureId;                                          /**< Secure Id: security profile ID linked to certificate/key identity                           */
  ST87EC_Lib_TlsListElemTypes_t ElementType;                /**< TLS Provisioning elements for the listing (keys/PSK, certificates/PSKIDs)                   */
  ST87EC_Lib_TlsProvCallback_t * pTlsProvCallbackFunc;      /**< Callback function returning lists of TLS provisioning elements with
                                                                 the following format:
                                                                  -  for certs: response format is: sec_id,type,issued_to,issued_by,valid_from,valid_to
                                                                  -  for PSK ID,response format is: sec_id,type,psk_id
                                                                  -  for keys/PSK, response format is: sec_id,type,cipher,algo,size
                                                                  -  for further details see ST87Mxx AT command manual (TLSCERTLIST, TLSKEYLIST)             */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled                                          */
} ST87EC_Lib_TlsListElemObject_t;

typedef struct {
  int8_t SecureId;                                          /**< Secure Id: security profile ID linked to certificate/key identity                           */
  ST87EC_Lib_TlsDumpElemTypes_t ElementType;                /**< TLS Provisioning elements for available for dump (certificates, PSK Id, public keys)        */
  ST87EC_Lib_TlsProvEccTypes_t EccType;                     /**< Elliptic curve type ( Prime SECP R1 or BrainpoolP R1.
                                                                  Only relevant for asymmetric public key element)                                           */
  ST87EC_Lib_TlsProvCallback_t * pTlsProvCallbackFunc;      /**< Callback function returning the dumped data and length of TLS provisioning elements:
                                                                  -  for Certificates: data format is DER
                                                                  -  for PSK_IDs: data format is RAW
                                                                  -  for public keys: data format is the one chosen by user (DER or RAW)
                                                                  Note: as data is in Hex format, length of data in binary is 2*value of the returned length */
  uint32_t Timeout;                                         /**< Timeout in ms after which the request is cancelled                                          */
} ST87EC_Lib_TlsDumpElemObject_t;

/* Exported functions --------------------------------------------------------*/
/**
* @brief Callback called to inform User of an EC Lib internal error
*
* @param FailingSequence: Id number of the on-going sequence (0 otherwise).
* @param Error: generic error code.
*/
typedef void (ST87EC_Lib_GenericErrorCallback_t) (ST87EC_Lib_SequenceValue_t FailingSequence, int32_t Error);

/**
* @brief Callback called to get the ST87M01 system time
* 
* @param pString: pointer to the returned string (read only pointer).
*/
typedef void (ST87EC_Lib_GetTimeCallback_t) (char const * const pString);

/**
* @brief Callback called to get the GNSS ST87M01 position
*
* @param pString: pointer to the returned string (read only pointer).
*/
typedef void (ST87EC_Lib_GNSS_GetPosCallback_t) (char const * const pString);

/**
* @brief Callback called upon CoAP data reception
*
* @param pCoapRxData: pointer to the returned ST87EC_Lib_CoapRxData_t data structure.
*/
typedef void (ST87EC_Lib_CoapReceiveCallback_t) (ST87EC_Lib_CoapRxData_t * pCoapRxData);

/**
* @brief Callback called upon data reception of a LwM2M built-in object resource after a read request.
*
* @param pString: pointer to the returned string (read only pointer).
*/
typedef void (ST87EC_Lib_Lwm2mObjDataRecvCallback_t) (char const * const pString);
/**
* @brief Callback called upon notification of a R/W or X action on a LwM2M custom object.
* @param NotifyType: type of action (R/W or X)
* @param pString: pointer to the returned string (read only pointer).Its format depending on previous NotifyType parameter:
    - for a read notification, expected format is: server_id,object_id,instance_id,resource_count,resource0_id,resource1_id,...
    - for a write notification, expected format is: server_id,object_id,instance_id,resource_count,
                                                    resource0_id,resource0_type,resource0_length,resource0_value,
                                                    resource1_id,resource1_type,resource1_length,resource1_value,
                                                    ...
    - or a execute notification, expected format is: server_id,object_id,instance_id,resource_id
*/
typedef void (ST87EC_Lib_Lwm2mCustObjectNotifyCallback_t) (ST87EC_Lib_Lwm2mNotifyType_t NotifyType, char const * const pString);

ST87EC_Lib_Result_t ST87EC_Lib_Init(ST87EC_Lib_GenericErrorCallback_t * pGenericErrorCallbackFunc);
ST87EC_Lib_Result_t ST87EC_Lib_Reset(void);
ST87EC_Lib_Result_t ST87EC_Lib_Scheduler(void);
ST87EC_Lib_Result_t ST87EC_Lib_GetTime(ST87EC_Lib_GetTimeCallback_t * pGetTimeCallbackFunc, uint32_t TimeoutMs);
ST87EC_Lib_Result_t ST87EC_Lib_GetState(ST87EC_Lib_Status_t * pState);
ST87EC_Lib_Result_t ST87EC_Lib_GNSS_GetFix(uint32_t NbPosition, ST87EC_Lib_GNSS_GetPosCallback_t * pGetPosCallbackFunc, uint32_t TimeoutMs);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_MqttPublish(const ST87EC_Lib_MqttPubObject_t * pMqttPubObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_MqttSubscribe(const ST87EC_Lib_MqttSubscObject_t * pMqttSubscObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_MqttUnsubscribe(const ST87EC_Lib_MqttUnsubscObject_t * pMqttUnsubscObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_UdpTransferData(ST87EC_Lib_UdpTcpObject_t * pUdpObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_TcpTransferData(ST87EC_Lib_UdpTcpObject_t * pTcpObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_CoapOpen(char * pIpAddr, ST87EC_Lib_AddressType_t AddressType, uint32_t PortNb, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_CoapTransmit(ST87EC_Lib_CoapTxObject_t * pCoapTxObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_CoapReceive(ST87EC_Lib_CoapReceiveCallback_t * pCoapReceiveCallbackFunc, ST87EC_Lib_CoapRxData_t * pCoapRxData);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_CoapClose(uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_HttpOpen(char * pUrl, uint32_t PortNb, int32_t SecureId, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_HttpTransfer(ST87EC_Lib_HttpTransferObject_t * pHttpTransferObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_HttpClose(uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_WMBUS_Transfer(const ST87EC_Lib_WmbusObject_t * pWmbusObject);
ST87EC_Lib_Result_t ST87EC_Lib_WIFI_Scan(ST87EC_Lib_WifiScanObject_t * pWifiScanObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Loader(ST87EC_Lib_BinToLoad_t BinaryId, uint32_t BinLength, uint32_t Timeout);
ST87EC_Lib_BinTransferStatus_t ST87EC_Lib_NBIOT_GetBinDataForLoaderCallback(uint32_t * pBinDataAddr, uint8_t NbBytesToTransfer);
ST87EC_Lib_Result_t ST87EC_Lib_UnitaryATcommandSend(const ST87EC_Lib_AtCmdObject_t * pAtCmdObject);
ST87EC_Lib_Result_t ST87EC_Lib_AsyncUrcReception(const ST87EC_Lib_AsyncUrcObject_t * pAsyncUrcObject);
ST87EC_Lib_Result_t ST87EC_Lib_TlsProvisioningImportElement(const ST87EC_Lib_TlsImportElemObject_t * pTlsImportElemObject);
ST87EC_Lib_Result_t ST87EC_Lib_TlsProvisioningGenerateElement(const ST87EC_Lib_TlsGenerateElemObject_t * pTlsGenerateElemObject);
ST87EC_Lib_Result_t ST87EC_Lib_TlsProvisioningDeleteElement(const ST87EC_Lib_TlsDeleteElemObject_t * pTlsDeleteElemObject);
ST87EC_Lib_Result_t ST87EC_Lib_TlsProvisioningListElement(const ST87EC_Lib_TlsListElemObject_t * pTlsListElemObject);
ST87EC_Lib_Result_t ST87EC_Lib_TlsProvisioningDumpElement(const ST87EC_Lib_TlsDumpElemObject_t * pTlsDumpElemObject);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mOpen(ST87EC_Lib_AddressType_t IpVersion, ST87EC_Lib_Lwm2mCustObjectNotifyCallback_t * pLwm2mCustObjectNotifyCallbackFunc, uint32_t ServerNotifyTimeout, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mRegister(ST87EC_Lib_RegistrationReq_t RegistrationOption, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mBuildInObjectRead(char * pUri, ST87EC_Lib_Lwm2mObjDataRecvCallback_t * pLwm2mObjReadDataRecvCallbackFunc, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mBuildInObjectWrite(char * pUri, char * pData, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mObjectList(ST87EC_Lib_Lwm2mObjDataRecvCallback_t * pLwm2mObjListDataRecvCallbackFunc, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mCustomObjectAdd(char * pObjectList, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mCustomObjectRemove(uint32_t ObjectId, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mCustomObjectServerNotify(ST87EC_Lib_Lwm2mNotifyType_t NotifType, char * pNotificationData, uint32_t Timeout);
ST87EC_Lib_Result_t ST87EC_Lib_NBIOT_Lwm2mClose(uint32_t Timeout);

#endif /* ST87EC_LIB_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
