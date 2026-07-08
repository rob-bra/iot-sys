/**
  ******************************************************************************
  * @file    st87ec_sequence_sys.c
  * @author  APMS Application Team
  * @brief   EC System sequences functions
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

#include <stdlib.h>
#include <stdio.h>
#include "st87ec_lib.h"
#include "st87ec_sequence_sys.h"
#include "st87ec_wrapper.h"
#include "st87ec_engine.h"
#include "st87ec_cold_config.h"
#include "st87ec_lib_hal.h"
#include "st87ec_debug.h"

/* Exported functions --------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/
#define ST87EC_COLD_INIT_TIMEOUT              (10000)
#define ST87EC_COLD_VERSION_INDEX             (8)
#define ST87EC_COLD_VERSION_NVM_PAGE          5
#define ST87EC_COLD_VERSION_NVM_OFFSET        12


#define ST87EC_FORMAT_TEMPLIMIT_CMD(lOW,hIGH,eNABLE)  "AT#TEMPLIMIT=-40,85," STR(lOW) "," STR(hIGH) ",0," STR(eNABLE)
#define ST87EC_TEMPLIMIT_CMD  ST87EC_FORMAT_TEMPLIMIT_CMD(TEMP_LOW_SHUTDOWN,TEMP_HIGH_SHUTDONW,TEMP_SHUTDOWN)

#define ST87EC_FORMAT_VBATLIMIT_CMD(lOW,hIGH,eNABLE)  "AT#VBATLIMIT=2200,3000," STR(lOW) "," STR(hIGH) "," STR(eNABLE) ",0,0"
#define ST87EC_VBATLIMIT_CMD  ST87EC_FORMAT_VBATLIMIT_CMD(VBAT_LOW_SHUTDOWN,VBAT_HIGH_SHUTDOWN,VBAT_SHUTDOWN)

#define ST87EC_FORMAT_BANDSEL_CMD(bANDS)  "AT#BANDSEL=" bANDS
#define ST87EC_BANDSEL_CMD  ST87EC_FORMAT_BANDSEL_CMD(BANDLIST)

#define ST87EC_FORMAT_SLEEPMODE_CMD(eNABLE,hOLD,aWAKE)  "AT#SLEEPMODE=" STR(eNABLE) "," STR(hOLD) "," STR(aWAKE)
#define ST87EC_SLEEPMODE_CMD  ST87EC_FORMAT_SLEEPMODE_CMD(SLEEP_ENABLE,HOLD_TIME,AWAKE_TIME)

#define ST87EC_FORMAT_BANDCFG_CMD(bAND,nMO1,nMO2,nMO3) "AT#BANDCFG=" bAND,\
                                                       "AT#BANDCFG=" nMO1,\
                                                       "AT#BANDCFG=" nMO2,\
                                                       "AT#BANDCFG=" nMO3
#define ST87EC_BANDCFG_CMD  ST87EC_FORMAT_BANDCFG_CMD(BANDCFG,BANDCFG_NMO1,BANDCFG_NMO2,BANDCFG_NMO3)

#define ST87EC_FORMAT_CEDRXS_CMD(vAL)  "AT+CEDRXS=1,5," STR(vAL)
#define ST87EC_CEDRX_CMD  ST87EC_FORMAT_CEDRXS_CMD(EDRX_VALUE)

#define ST87EC_FORMAT_PTW_CMD(vAL)  "AT#PTW=" STR(vAL)
#define ST87EC_PTW_CMD  ST87EC_FORMAT_PTW_CMD(PTW_VALUE)

#define ST87EC_FORMAT_CPSMS_CMD(eNABLE,tAU,aCTIVE)  "AT+CPSMS=" STR(eNABLE) ",,,\"" STR(tAU) "\",\"" STR(aCTIVE) "\""
#define ST87EC_CPSMS_CMD  ST87EC_FORMAT_CPSMS_CMD(PSM_ENABLE,PERIODIC_TAU,ACTIVE_TIME)

#define ST87EC_FORMAT_RINGPIN_CMD(eNABLE,gPIO,pOL,dELAY) "AT#RINGPIN=" STR(eNABLE) "," STR(gPIO) "," STR(pOL) "," STR(dELAY)
#define ST87EC_RINGPIN_CMD ST87EC_FORMAT_RINGPIN_CMD(RING_PIN_ENABLE,RING_PIN_GPIO,RING_PIN_POLARITY,RING_PIN_DELAY)

#define ST87EC_FORMAT_NVMRD_CMD(pAGE,oFF) "AT#NVMRD=" STR(pAGE) "," STR(oFF) ",1"
#define ST87EC_NVMRD_CMD ST87EC_FORMAT_NVMRD_CMD(ST87EC_COLD_VERSION_NVM_PAGE,ST87EC_COLD_VERSION_NVM_OFFSET)

#define ST87EC_FORMAT_NVMWR_CMD(pAGE,oFF,vER) "AT#NVMWR=" STR(pAGE) "," STR(oFF) ",1," STR(vER)
#define ST87EC_NVMWR_CMD ST87EC_FORMAT_NVMWR_CMD(ST87EC_COLD_VERSION_NVM_PAGE,ST87EC_COLD_VERSION_NVM_OFFSET,ST87EC_COLD_CONFIG_VERSION)

#define ST87EC_FORMAT_IPPARAM_CMD(nBpACKETsENTvAL, dOMAINnAME)  "AT#IPPARAMS=1,0,65535,60," STR(nBpACKETsENTvAL) "," STR(dOMAINnAME)
#define ST87EC_IPPARAM_CMD  ST87EC_FORMAT_IPPARAM_CMD(NB_PACKET_SENT_ENABLE,DOMAIN_NAME)

#define ST87EC_FORMAT_LWM2M_PARAMS_CMD(cLIENTnAME,sERVERiD,sERVERuRL,lIFETIME)  "AT#LWPARAMS=" STR(cLIENTnAME) ","               \
                                                                                               STR(sERVERiD) ",1,"               \
                                                                                               STR(sERVERuRL) ",U,"              \
                                                                                               STR(lIFETIME) ",0,0,1"
#define ST87EC_LWM2M_PARAMS_CMD  ST87EC_FORMAT_LWM2M_PARAMS_CMD(LWM2M_CLIENT_NAME,LWM2M_SERVER_ID,LWM2M_SERVER_URL,LWM2M_LIFETIME)

#define ST87EC_FORMAT_URCMODE_CMD(uRCmODE)  "AT#WSCAN=2," STR(uRCmODE)
#define ST87EC_WSCAN_URCMODE_CMD  ST87EC_FORMAT_URCMODE_CMD(URC_MODE)

#define ST87EC_FORMAT_HOPTIME_CMD(hOPtIME)  "AT#WSCAN=3," STR(hOPtIME)
#define ST87EC_WSCAN_HOPTIME_CMD  ST87EC_FORMAT_HOPTIME_CMD(HOPPING_TIME)

#define ST87EC_FORMAT_ANTSEL_CMD(aNTsEL)  "AT#WSCAN=4," STR(aNTsEL)
#define ST87EC_WSCAN_ANTSEL_CMD  ST87EC_FORMAT_ANTSEL_CMD(ANT_SEL)

#define ST87EC_NVM_WRITE_CMD  "AT#RESET=1"

#define ST87EC_TLSPROV_IMPORT_PSK_CMD  "AT#TLSKEYADD=%d,1,2,4,%d,%s", EcLibVars.SeqTlsProvisioning.Params.SecureId,       \
             EcLibVars.SeqTlsProvisioning.Params.InputLength,                                                             \
             EcLibVars.SeqTlsProvisioning.Params.pInputData
#define ST87EC_TLSPROV_IMPORT_PRIVKEY_CMD  "AT#TLSKEYADD=%d,%d,2,%d,%d,%s", EcLibVars.SeqTlsProvisioning.Params.SecureId, \
             ((uint8_t)EcLibVars.SeqTlsProvisioning.Params.EccType) << 2,                                                 \
             ((uint8_t)TLS_PROV_KEY_FORMAT) << 2,                                                                         \
             EcLibVars.SeqTlsProvisioning.Params.InputLength,                                                             \
             EcLibVars.SeqTlsProvisioning.Params.pInputData
#define ST87EC_TLSPROV_IMPORT_CERT_CMD  "AT#TLSCERTADD=%d,%d,%d,%s", EcLibVars.SeqTlsProvisioning.Params.SecureId,        \
             (uint8_t)EcLibVars.SeqTlsProvisioning.Params.ImportElementType,                                              \
             EcLibVars.SeqTlsProvisioning.Params.InputLength,                                                             \
             EcLibVars.SeqTlsProvisioning.Params.pInputData

#define ST87EC_TLSPROV_GENERATE_PRIVKEY_CMD  "AT#TLSKEYADD=%d,%d,2,%d,%d", EcLibVars.SeqTlsProvisioning.Params.SecureId,  \
             (0x80 + (((uint8_t)EcLibVars.SeqTlsProvisioning.Params.EccType) << 2)),                                      \
             (0x3 + (((uint8_t)TLS_PROV_KEY_FORMAT) << 2)),                                                               \
             EcLibVars.SeqTlsProvisioning.Params.InputLength
#define ST87EC_TLSPROV_GENERATE_CSR_CMD  "AT#TLSCERTSIGN=%d,128,%d,%s", EcLibVars.SeqTlsProvisioning.Params.SecureId,     \
             EcLibVars.SeqTlsProvisioning.Params.InputLength,                                                             \
             EcLibVars.SeqTlsProvisioning.Params.pInputData

#define ST87EC_TLSPROV_CDEL_CMD  "AT#TLSCERTDEL=%d,%d", EcLibVars.SeqTlsProvisioning.Params.SecureId,              \
             ((uint8_t)EcLibVars.SeqTlsProvisioning.Params.DelElementType)
#define ST87EC_TLSPROV_CDEL_ALL_CMD  "AT#TLSCERTDEL=%d", EcLibVars.SeqTlsProvisioning.Params.SecureId
#define ST87EC_TLSPROV_KDEL_ALL_CMD  "AT#TLSKEYDEL=%d", EcLibVars.SeqTlsProvisioning.Params.SecureId

#define ST87EC_FORMAT_TLSPROV_LIST_CMD(tYPE)  ST87EC_Wrapper_SendCmd("AT#TLS" tYPE "LIST=%d",                       \
             EcLibVars.SeqTlsProvisioning.Params.SecureId);
#define ST87EC_TLSPROV_LIST_CMD                                                                                     \
             if (EcLibVars.SeqTlsProvisioning.Params.ListElementType == TLS_PROV_ELEM_KEYS)                         \
             {                                                                                                      \
               ST87EC_FORMAT_TLSPROV_LIST_CMD("KEY")                                                                \
             }                                                                                                      \
             else                                                                                                   \
             {                                                                                                      \
               ST87EC_FORMAT_TLSPROV_LIST_CMD("CERT")                                                               \
             }
#define ST87EC_FORMAT_TLSPROV_LIST_ALL_CMD(tYPE)  ST87EC_Wrapper_SendCmd("AT#TLS" tYPE "LIST");
#define ST87EC_TLSPROV_LIST_ALL_CMD                                                                                 \
             if (EcLibVars.SeqTlsProvisioning.Params.ListElementType == TLS_PROV_ELEM_KEYS)                         \
             {                                                                                                      \
               ST87EC_FORMAT_TLSPROV_LIST_ALL_CMD("KEY")                                                            \
             }                                                                                                      \
             else                                                                                                   \
             {                                                                                                      \
               ST87EC_FORMAT_TLSPROV_LIST_ALL_CMD("CERT")                                                           \
             }

#define ST87EC_TLSPROV_CDUMP_CMD  "AT#TLSCERTADD=%d,%d", EcLibVars.SeqTlsProvisioning.Params.SecureId,              \
             (0x80 + (uint8_t)EcLibVars.SeqTlsProvisioning.Params.DumpElementType)
#define ST87EC_TLSPROV_KDUMP_CMD  "AT#TLSKEYADD=%d,%d,2,%d", EcLibVars.SeqTlsProvisioning.Params.SecureId,          \
             (0x80 + (((uint8_t)EcLibVars.SeqTlsProvisioning.Params.EccType) << 2)),                                \
             (0x2 + (((uint8_t)TLS_PROV_KEY_FORMAT)<<2))

#define ST87EC_START_BOOT_RSP_TIMER                                                                                 \
             if (len_sent > 0)                                                                                      \
             {                                                                                                      \
               /* Start waiting for AT response timer (and launching UART Boot Handshake to come if no Rsp...) */   \
               EcLibVars.BootIf.AtRspWaitingTimerId = ST87EC_Wrapper_StartTimer(300);                               \
               if (EcLibVars.BootIf.AtRspWaitingTimerId == ST87EC_TIMER_ERROR)                                      \
               {                                                                                                    \
                 EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_STEP_NONE;                                           \
                 len_sent = 0; /* Raise error */                                                                    \
               }                                                                                                    \
               else                                                                                                 \
               {                                                                                                    \
                 EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_WAIT_FOR_AT_RSP;                                     \
               }                                                                                                    \
             }

#define ST87EC_SEND_BOOT_CMD(...)                                                                                   \
             len_sent = ST87EC_Wrapper_SendCmdToBoot(__VA_ARGS__);                                                  \
             EcLibVars.BootIf.HandshakeOkTrialsCnt = 0;                                                             \
             ST87EC_START_BOOT_RSP_TIMER

#define ST87EC_RESEND_BOOT_CMD                                                                                      \
             len_sent = ST87EC_Wrapper_SendByte(NULL, 0);                                                           \
             ST87EC_START_BOOT_RSP_TIMER

/* Private types -------------------------------------------------------------*/
typedef struct
{
  uint8_t NbCmd;            /**< Number of AT commands to send for upload of the given binary */
  char * CmdStrArray[3];    /**< Array of AT command strings to be sent for upload of the given binary */
} ST87EC_SequenceSys_SpecificAtCmds_t;

/* Private variables ---------------------------------------------------------*/

/* Cold init sequence command list */
const char* ST87EC_COLD_INIT_COMMANDS[]=
{
  /* Config for EC lib */
  "ATE0",                              /* Set Echo Mode command to OFF */
  "AT+CMEE=1",                         /* Report Mobile Termination Error*/
  "AT+CEREG=5",                        /* Registration status verbosity */
  "AT+CSCON=1",                        /* Connection status verbosity */
  "AT#SLEEPIND=0x1F",                  /* Sleep indication verbosity */
  "AT#WDGMODE=0",                      /* Watchdog setting */

  /* HW config */
  ST87EC_TEMPLIMIT_CMD,                /* Limit of temperature setting */
  ST87EC_VBATLIMIT_CMD,                /* Vbat limit setting */

  /* NBIOT */
  "AT+CFUN=0",                         /* Turn off modem before setting */
  ST87EC_BANDSEL_CMD,                  /* Band selection list */
  ST87EC_BANDCFG_CMD,                  /* Band configuration */
  "AT#SCAN=1,-104,1,360,1,360",        /* Extended coverage enabled */
  ST87EC_CEDRX_CMD,                    /* EDRX configuration */
  ST87EC_PTW_CMD,                      /* Paging time window configuration */
  ST87EC_CPSMS_CMD,                    /* Power saving setting */

  /* ST87 configuration */
  ST87EC_SLEEPMODE_CMD,                /* Sleep mode configuration */
  ST87EC_RINGPIN_CMD,                  /* Ring pin configuration */
  "AT#WAKEUPEVENT=15,3",               /* Wake up event configuration on UART */

  /* Connection */
  ST87EC_IPPARAM_CMD,                  /* IP configuration */
#if (LWM2M_COLD_CONFIG_ENABLE == 1)
  ST87EC_LWM2M_PARAMS_CMD,             /* LwM2M params configuration */
#endif

  /* Wifi scanning cold config */
  ST87EC_WSCAN_URCMODE_CMD,            /* URC mode configuration */
  ST87EC_WSCAN_HOPTIME_CMD,            /* Hoping time configuration */
  ST87EC_WSCAN_ANTSEL_CMD,             /* Antenna selection */

  /* Update NVM */
  ST87EC_NVMWR_CMD,                    /* Write Cold ini version in NVM */
  ST87EC_NVM_WRITE_CMD                 /* Save changes in NVM */
};

#define ST87EC_COLD_INIT_CMD_SIZE   (sizeof(ST87EC_COLD_INIT_COMMANDS)/4)

/* Loader sequence core specific command list */
const ST87EC_SequenceSys_SpecificAtCmds_t ST87EC_SequenceSys_SpecificAtCmdTable[] =
  {
    {0, {"", "", ""}},     /* ST87_SEC_BINARY   */
    {0, {"", "", ""}},     /* ST87_ALP_BINARY   */
    {0, {"", "", ""}},     /* ST87_MODEM_BINARY */
    {0, {"", "", ""}},     /* ST87_GNSS_BINARY  */
    {0, {"", "", ""}}      /* ST87_CAP_BINARY   */
  };

/* Internal functions --------------------------------------------------------*/
static inline ST87EC_Lib_Result_t ST87EC_SequenceSys_ManageBootAtRsp(void);
static ST87EC_Lib_Result_t ST87EC_SequenceSys_BinaryUpload(void);


/**
* @brief Sequence Cold parameter initialization
*
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_ColdParamInit(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;
  uint32_t tmp;

  /* Check timer expiration */
  if ((EcLibVars.SequenceSys.ColdInit.FsmState != COLDINIT_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SequenceSys.ColdInit.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_ERROR;
  }

  switch (EcLibVars.SequenceSys.ColdInit.FsmState)
  {
    case COLDINIT_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SequenceSys.ColdInit.TimerId = ST87EC_Wrapper_StartTimer(ST87EC_COLD_INIT_TIMEOUT);

      if (EcLibVars.SequenceSys.ColdInit.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_CHECK_VERSION;
      }
      else
      {
        /* Read the NVM version in CUST NVMto check if cold init is necessary */
        ST87EC_Wrapper_SendCmd(ST87EC_NVMRD_CMD);
        EcLibVars.RspInfo[NVMREAD_INDEX].Validity = RSP_AWAITED;
        EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_CHECK_VERSION;
      }
      break;

    case COLDINIT_STATE_CHECK_VERSION:
      /* Version check ==  ST87EC_COLD_CONFIG_VERSION */
      if (EcLibVars.RspInfo[NVMREAD_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.RspInfo[NVMREAD_INDEX].Validity = RSP_NONE;
        sscanf((const char *)&EcLibVars.RspData[ST87EC_COLD_VERSION_INDEX],"%x",(int *)&tmp);
        EcLibVars.ColdInitVersion = (uint8_t)tmp;
        if (EcLibVars.ColdInitVersion != ST87EC_COLD_CONFIG_VERSION)
        {
          EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_SEND_COMMANDS;
        }
        else
        {
          EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_END;
        }
      }
      break;

    case COLDINIT_STATE_SEND_COMMANDS:
      if (EcLibVars.SequenceSys.ColdInit.SequenceIndex < ST87EC_COLD_INIT_CMD_SIZE)
      {
        /* Wait to purge previous "OK" response */
        ST87EC_Wrapper_Delay(100);
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;

        EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_SEND_COMMANDS;
        ST87EC_Wrapper_SendCmd(ST87EC_COLD_INIT_COMMANDS[EcLibVars.SequenceSys.ColdInit.SequenceIndex]);
        EcLibVars.SequenceSys.ColdInit.SequenceIndex++;
        EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_WAIT_OK;
      }
      else /* Last init command sent */
      {
        EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_END;
      }
      break;

    case COLDINIT_STATE_WAIT_OK:
      /* Wait for OK or error */
      if (EcLibVars.RspReceived != SPECIFIC_RSP_NONE)
      {
        if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
        {
          /* Send next init command */
          EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_SEND_COMMANDS;
          EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        }
        if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
        {
          /* Error received */
          EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_ERROR;
          EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        }
      }
      break;

    default:
    case COLDINIT_STATE_ERROR:
      result = RESULT_KO;

    case COLDINIT_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SequenceSys.ColdInit.TimerId);

      /* Stop the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SequenceSys.ColdInit.FsmState = COLDINIT_STATE_INIT;
      EcLibVars.SequenceSys.ColdInit.SequenceIndex = 0;
      break;
  }
  return (result);
}

/**
* @brief Sequence for the Get Time request
*
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_GetTime(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SequenceSys.GetTime.FsmState != GETTIME_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SequenceSys.GetTime.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SequenceSys.GetTime.FsmState = GETTIME_STATE_ERROR;
  }

  switch (EcLibVars.SequenceSys.GetTime.FsmState)
  {
    case GETTIME_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SequenceSys.GetTime.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SequenceSys.GetTime.Params.TimeoutMs);

      if (EcLibVars.SequenceSys.GetTime.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SequenceSys.GetTime.FsmState = GETTIME_STATE_ERROR;
      }
      else
      {
        EcLibVars.RspInfo[CCLK_INDEX].Validity = RSP_AWAITED; /* Indicate response from cmd to be sent is awaited */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        ST87EC_Wrapper_SendCmd("AT+CCLK?");

        EcLibVars.SequenceSys.GetTime.FsmState = GETTIME_STATE_WAIT_FOR_RSP;
      }
      break;

    case GETTIME_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SequenceSys.GetTime.FsmState = GETTIME_STATE_ERROR;
      }
      else if (EcLibVars.RspInfo[CCLK_INDEX].Validity == RSP_RECEIVED)
      {
        /* Stop the timer */
        result = ST87EC_Wrapper_StopTimer(EcLibVars.SequenceSys.GetTime.TimerId);

        /* Awaited response is received, call the customer callback function with received data in parameter (+CCLK_TAG_SIZE to avoid the "+CCLK: " tag) */
        EcLibVars.SequenceSys.GetTime.Params.pGetTimeCallbackFunc(&EcLibVars.RspData[CCLK_TAG_SIZE]);

        EcLibVars.RspInfo[CCLK_INDEX].Validity = RSP_NONE;

        /* Update the sequence status in the EC lib status array and FSM */
        EcLibVars.OnGoingSequence = SEQUENCE_NONE;
        EcLibVars.SequenceSys.GetTime.FsmState = GETTIME_STATE_INIT;
      }
      else {}

      EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
      break;

    default:
    case GETTIME_STATE_ERROR:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SequenceSys.GetTime.TimerId);
      /* Call the customer callback with a NULL pointer */
      EcLibVars.SequenceSys.GetTime.Params.pGetTimeCallbackFunc(NULL);

      /* Stop the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SequenceSys.GetTime.FsmState = GETTIME_STATE_INIT;

      result = RESULT_KO;
      break;
  }
  return (result);
}


/**
* @brief Sequence for ST87 binaries loader
*
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_Loader(void)
{
  uint32_t len_sent = 1;
  ST87EC_Lib_Result_t status = RESULT_OK;
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SequenceSys.Loader.FsmState != LOADER_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SequenceSys.Loader.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_ERROR;
  }

  switch (EcLibVars.SequenceSys.Loader.FsmState)
  {
    case LOADER_STATE_INIT:
      EcLibVars.SequenceSys.Loader.OriBaudrate = ST87EC_Lib_Hal_GetHostUartBaudrate();
      EcLibVars.BootIf.LastMsgBeforeBootUpload = false;
      /* Start the timeout */
      EcLibVars.SequenceSys.Loader.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SequenceSys.Loader.Params.Timeout);
      if (EcLibVars.SequenceSys.Loader.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_ERROR;
      }
      else
      {
        EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_SEND_COMMANDS;
        /* Send preliminary AT cmd if needed, prior to starting binary transfer */
        if (ST87EC_SequenceSys_SpecificAtCmdTable[EcLibVars.SequenceSys.Loader.Params.BinaryId].NbCmd > 0)
        {
          EcLibVars.SequenceSys.Loader.Command = LOADER_CMD_SEND_SPECIFIC_CMDS;
        }
        else
        {
          EcLibVars.SequenceSys.Loader.Command = LOADER_CMD_RESET_IN_BOOT_MODE;
        }
        EcLibVars.SequenceSys.Loader.NbCmdSent = 0;
      }
      break;

    case LOADER_STATE_SEND_COMMANDS:
      EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
      //ST87EC_Wrapper_Delay(100);
      switch (EcLibVars.SequenceSys.Loader.Command)
      {
        case LOADER_CMD_SEND_SPECIFIC_CMDS:
          len_sent = ST87EC_Wrapper_SendCmd(ST87EC_SequenceSys_SpecificAtCmdTable[EcLibVars.SequenceSys.Loader.Params.BinaryId].CmdStrArray[EcLibVars.SequenceSys.Loader.NbCmdSent]);
          EcLibVars.SequenceSys.Loader.NbCmdSent++;
          break;
        case LOADER_CMD_RESET_IN_BOOT_MODE:
          len_sent = ST87EC_Wrapper_SendCmd("AT#LDRST=2,2");
          break;
        case LOADER_CMD_SET_BOOT_IN_ATPARSE_STATE:
          /* From now, ST87 is in Boot mode */
          ST87EC_Wrapper_Delay(1000); /* Wait for mode switching from Normal to Boot */
          EcLibVars.BootMode = BOOT_AT_MODE;
          ST87EC_SEND_BOOT_CMD("ATI0");
          break;
        case LOADER_CMD_CONFIG_ST87_LOADING_BAUDRATE:
          /* Set suitable baudrate ST87 side for binary loading */
          ST87EC_SEND_BOOT_CMD("AT+IPR=%d", 460800);
          break;
        case LOADER_CMD_CONFIG_HOST_LOADING_BAUDRATE:
          /* Set suitable baudrate Host side for binary loading */
          ST87EC_Lib_Hal_SetHostUartBaudrate(460800);
          ST87EC_Wrapper_Delay(300);
          /* Send an AT command to check UART is well set up */
          ST87EC_SEND_BOOT_CMD("ATI0");
          break;
        case LOADER_CMD_SET_BOOT_IN_UARTLOAD_STATE:
          EcLibVars.BootIf.LastMsgBeforeBootUpload = true;
          ST87EC_SEND_BOOT_CMD("AT#UP");
          break;
        case LOADER_CMD_UPLOAD_REBOOT:
          status = ST87EC_SequenceSys_BinaryUpload();
          if (status == RESULT_OK)
          {
            ST87EC_SEND_BOOT_CMD("AT#BOOT");
          }
          break;
        default:
          break;
      }
      if ((len_sent == 0) || (status == RESULT_KO))
      {
        EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_ERROR;
      }
      else
      {
        EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_WAIT_RSP;
      }
      break;

    case LOADER_STATE_WAIT_RSP:
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        /* Go to next command processing or, if fulfilled, move FSM to next state */
        if ((uint32_t)EcLibVars.SequenceSys.Loader.Command < (uint32_t)LOADER_CMD_UPLOAD_REBOOT)
        {
          if (EcLibVars.SequenceSys.Loader.NbCmdSent >= ST87EC_SequenceSys_SpecificAtCmdTable[EcLibVars.SequenceSys.Loader.Params.BinaryId].NbCmd)
          {
            (uint32_t)EcLibVars.SequenceSys.Loader.Command++;
          }
          EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_SEND_COMMANDS;
        }
        else
        {
          EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_END_SEQUENCE;
        }
      }
      else {}

      /* Manage ST87 Boot AT command responses */
      if (EcLibVars.BootIf.SendAtToBootStep != LOADER_AT_STEP_NONE)
      {
        status = ST87EC_SequenceSys_ManageBootAtRsp();
        if (status == RESULT_KO)
        {
          EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_ERROR;
        }
      }
      break;

    case LOADER_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
      EcLibVars.BootIf.LastMsgBeforeBootUpload = false;
    case LOADER_STATE_END_SEQUENCE:
      EcLibVars.BootMode = NORMAL_MODE;
      /* Go back to Host default baudrate */
      ST87EC_Lib_Hal_SetHostUartBaudrate(EcLibVars.SequenceSys.Loader.OriBaudrate);
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SequenceSys.Loader.TimerId);
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SequenceSys.Loader.FsmState = LOADER_STATE_INIT;
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        /* Reset ST87 module */
        ST87EC_Wrapper_DriveResetPin(RESET_PIN_PULSE);
        ST87EC_Wrapper_Delay(500);  /* Wait after ST87 Hw reset prior to proceeding */
        result = RESULT_KO;
      }
      break;

    default:
      break;
  }

  return (result);
}


/**
* @brief Sub-function managing the response from AT commands sent to Boot
*
* @param : None
* @retval Function execution status
*/
static inline ST87EC_Lib_Result_t ST87EC_SequenceSys_ManageBootAtRsp(void)
{
  uint32_t len_sent = 0xFFFFFFFF;
  ST87EC_Lib_Result_t result = RESULT_OK;
  ST87EC_Wrapper_TimerStatus_t timer_sts1, timer_sts2;

  switch (EcLibVars.BootIf.SendAtToBootStep)
  {
    case LOADER_AT_WAIT_FOR_AT_RSP:
      timer_sts1 = ST87EC_Wrapper_GetTimerStatus(EcLibVars.BootIf.AtRspWaitingTimerId,1);
      if (timer_sts1 == TIMER_STATUS_BUSY)  /* Timer is on-going */
      {
        /* Boot specific '0' tag (i.e OK) string detection */
        //if (0)   /* error UT */
        if ((EcLibVars.RspReceived == SPECIFIC_RSP_OK) || (EcLibVars.RspReceived == SPECIFIC_RSP_CME))
        {
          ST87EC_Wrapper_StopTimer(EcLibVars.BootIf.AtRspWaitingTimerId);
          EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_STEP_NONE;
        }
      }
      else if (timer_sts1 == TIMER_STATUS_ELAPSED)
      {
        /* No response received from ST87 Boot: initiate UART Boot Handshake */
        EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_START_BOOT_UART_HANDSHAKE;
        EcLibVars.BootIf.HandshakeKoTrialsCnt = 0;
      }
      else {/* TIMER_STATUS_IDLE */}
      break;

    case LOADER_AT_START_BOOT_UART_HANDSHAKE:
      ST87EC_TRACE("\r\nSending Handshake hotkey...\r\n");
      uint8_t hotkey[2U] = {0xC5, 0xFF};
      len_sent = ST87EC_Wrapper_SendByte(hotkey, 2U);
      EcLibVars.BootIf.HandshakeRspWaitingTimerId = ST87EC_Wrapper_StartTimer(100);
      if (EcLibVars.BootIf.AtRspWaitingTimerId == ST87EC_TIMER_ERROR)
      {
        EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_STEP_NONE;
        result = RESULT_KO;
      }
      else
      {
        EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_WAIT_FOR_HANDSHAKE_RSP;
      }
      break;

    case LOADER_AT_WAIT_FOR_HANDSHAKE_RSP:
      timer_sts2 = ST87EC_Wrapper_GetTimerStatus(EcLibVars.BootIf.HandshakeRspWaitingTimerId,1);
      if (timer_sts2 == TIMER_STATUS_BUSY) /* Timer is on-going */
      {
        /* Boot specific '9A' handshake OK string found! */
        //if (1)  /* error UT (forcing 9A detect) */
        if (EcLibVars.RspReceived == SPECIFIC_RSP_HANDSHAKE_OK)
        {
          ST87EC_TRACE("\r\nHandshake ACK received!\r\n");
          EcLibVars.BootIf.HandshakeOkTrialsCnt++;
          if (EcLibVars.BootIf.HandshakeOkTrialsCnt > 2)
          {
            ST87EC_TRACE("\r\nNb of OK handshake with NO Resp to AT cmd reached! End seq with error.\r\n");
            EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_STEP_NONE;
            result = RESULT_KO;
          }
          else
          {
            ST87EC_TRACE("\r\nRetry AT Cmd...\r\n");
            ST87EC_Wrapper_StopTimer(EcLibVars.BootIf.HandshakeRspWaitingTimerId);
            /* Retry sending AT command to Boot */
            ST87EC_RESEND_BOOT_CMD
          }
        }
      }
      else if (timer_sts2 == TIMER_STATUS_ELAPSED)
      {
        ST87EC_TRACE("\r\nHandshake ACK not received!\r\n");
        EcLibVars.BootIf.HandshakeKoTrialsCnt++;
        if (EcLibVars.BootIf.HandshakeKoTrialsCnt > 2)
        {
          ST87EC_TRACE("\r\nNb of KO handshake reached! End seq with error.\r\n");
          EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_STEP_NONE;
          result = RESULT_KO;
        }
        else
        {
          ST87EC_TRACE("\r\nReset ST87 and retry handshake...\r\n");
          /* Reset ST87 module and retry handshake */
          ST87EC_Wrapper_DriveResetPin(RESET_PIN_PULSE);
          ST87EC_Wrapper_Delay(500);  /* Wait after ST87 Hw reset prior to proceeding */
          EcLibVars.BootIf.SendAtToBootStep = LOADER_AT_START_BOOT_UART_HANDSHAKE;
        }
      }
      else {/* TIMER_STATUS_IDLE */}
      break;

    default:
      break;
  }
  if (len_sent == 0U)
  {
    result = RESULT_KO;
  }

  return result;
}


/**
* @brief Sub-function handling the binary upload protocol
*
* @param : None
* @retval Function execution status
*/
static ST87EC_Lib_Result_t ST87EC_SequenceSys_BinaryUpload(void)
{
  int8_t nb_bytes_to_upload;
  uint8_t * zero_buff;
  ST87EC_Lib_Result_t read_sts;
  ST87EC_Lib_Result_t result = RESULT_OK;

  volatile ST87EC_Lib_BinTransferStatus_t get_bin_data_sts;
  uint32_t bin_data_addr;

  ST87EC_TRACE("\r\nBIN '%d' UPLOAD starting...\r\n", EcLibVars.SequenceSys.Loader.Params.BinaryId);

  EcLibVars.BootIf.NbBytesUploaded = 0;

  while (1)
  {
    read_sts = ST87EC_Wrapper_GetSt87BootNbBytesToUpload((uint8_t *)(&nb_bytes_to_upload));
    if (read_sts == RESULT_OK)
    {
      if (nb_bytes_to_upload < 0)
      {
        /* ST87 Boot sent an error code: exit with error */
        result = RESULT_KO;
        break;
      }
      else if (nb_bytes_to_upload == 0)
      {
        if (EcLibVars.BootIf.NbBytesUploaded == EcLibVars.SequenceSys.Loader.Params.BinaryLength)
        {
          /* All binary content has been uploaded OK:
              Notify by calling callback(NbByte=0), reset UART buffers,
              Then set Boot mode back to AT and exit with OK */
          get_bin_data_sts = ST87EC_Lib_NBIOT_GetBinDataForLoaderCallback(&bin_data_addr, (uint8_t)nb_bytes_to_upload);
          if (get_bin_data_sts == ST87_BIN_DATA_TRANSFER_OK)
          {
            ST87EC_Wrapper_UartInit();
            EcLibVars.BootMode = BOOT_AT_MODE;
            break;
          }
          else
          {
            /* Issue found: exit with error */
            result = RESULT_KO;
            break;
          }
        }
        else
        {
          /* Length mismatch (bin corruption): exit with error */
          result = RESULT_KO;
          break;
        }
      }
      else /* nb_bytes_to_upload >0: ST87 asking for data */
      {
        if (((uint8_t)nb_bytes_to_upload + EcLibVars.BootIf.NbBytesUploaded) <= EcLibVars.SequenceSys.Loader.Params.BinaryLength)
        {
          get_bin_data_sts = ST87_BIN_DATA_TRANSFER_BUSY;
          bin_data_addr = 0;
          while (get_bin_data_sts == ST87_BIN_DATA_TRANSFER_BUSY)
          {
            get_bin_data_sts = ST87EC_Lib_NBIOT_GetBinDataForLoaderCallback(&bin_data_addr, (uint8_t)nb_bytes_to_upload);
          }
          if (get_bin_data_sts == ST87_BIN_DATA_TRANSFER_OK)
          {
            if(nb_bytes_to_upload != ST87EC_Wrapper_SendByte((uint8_t *)(bin_data_addr), nb_bytes_to_upload))
            {
              /* Nb bytes sent over ST87 UART differs from expected nb of bytes to upload: exit with error */
              result = RESULT_KO;
              break;
            }
          }
          else /* ST87_BIN_DATA_TRANSFER_KO */
          {
            /* Issue found when getting binary data to transfer: exit with error */
            result = RESULT_KO;
            break;
          }
        }
        else
        {
          /* Length mismatch (bin corruption):
              answer with padding bytes 0x0 that will lead the device to softly closing the procedure */
          zero_buff = (uint8_t *)malloc((uint8_t)nb_bytes_to_upload);
          memset(zero_buff, 0x0, (uint8_t)nb_bytes_to_upload);
          ST87EC_Wrapper_SendByte(zero_buff, (uint8_t)nb_bytes_to_upload);
          free(zero_buff);
        }
        EcLibVars.BootIf.NbBytesUploaded += (uint8_t)nb_bytes_to_upload;
      }
    }
    else if (read_sts == RESULT_KO)
    {
      result = RESULT_KO;
      break;
    }
    else {/* Waiting for ST87 Boot byte */}
  } /* end while(1) */

  EcLibVars.BootMode = BOOT_AT_MODE;
  if (result == RESULT_KO)
  {
    ST87EC_TRACE("\r\nBIN UPLOAD ended KO.\r\n");
  }
  else
  {
    ST87EC_TRACE("\r\nBIN UPLOAD ended OK.\r\n");
  }

  return result;
}


/**
* @brief Sub-function handling the sending of single AT command to ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_AtCmdSending(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SeqAtCmdSend.FsmState != ATCMD_SEND_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqAtCmdSend.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_ERROR;
  }

  switch (EcLibVars.SeqAtCmdSend.FsmState)
  {
    case ATCMD_SEND_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqAtCmdSend.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqAtCmdSend.Params.Timeout);
      if (EcLibVars.SeqAtCmdSend.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_ERROR;
      }
      else
      {
        /* If a response to the AT cmd is expected: 2 possibilities:
           - either (main case) tag is known: get it
           - or no tag is present: get the rsp of an expected length
           And get ready for its reception */
        if (EcLibVars.SeqAtCmdSend.Params.pRspTag != NULL)
        {
          EcLibVars.RspInfo[ATCMD_SENDING_INDEX].Hash = ST87EC_Lib_ComputeHash(EcLibVars.SeqAtCmdSend.Params.pRspTag, strlen(EcLibVars.SeqAtCmdSend.Params.pRspTag));
          EcLibVars.RspInfo[ATCMD_SENDING_INDEX].Validity = RSP_AWAITED;
        }
        else if (EcLibVars.SeqAtCmdSend.Params.RawDataLen > 0)
        {
          /* No tag present: immediately catch coming raw data
             payload with the correct expected length */
          EcLibVars.RawDataRsp.ReceivedLength = 0;
          EcLibVars.RawDataRsp.ExpectedLength = EcLibVars.SeqAtCmdSend.Params.RawDataLen + 2; /* +2 as expected data string will start with \r\n */
          EcLibVars.RspInfo[ATCMD_SENDING_INDEX].Validity = RSP_NONE;
        }
        /* Send AT command */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        ST87EC_Wrapper_SendCmd(EcLibVars.SeqAtCmdSend.Params.pCommand);
        EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_WAIT_FOR_RSP;
      }
      break;

    case ATCMD_SEND_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        if ( (EcLibVars.SeqAtCmdSend.Params.pRspTag == NULL)
          && (EcLibVars.SeqAtCmdSend.Params.RawDataLen == 0))
        {
          /* No AT cmd response expected: sequence can be finished */
          EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_END;
        }
        else
        {
          if (EcLibVars.RspInfo[ATCMD_SENDING_INDEX].Validity == RSP_RECEIVED)
          {
            /* A tag has been found: process response and finish sequence */
            if (EcLibVars.SeqAtCmdSend.Params.pAtRespCallbackFunc != NULL)
            {
              EcLibVars.SeqAtCmdSend.Params.pAtRespCallbackFunc(EcLibVars.RspData);
            }
            EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_END;
          }
          else if ( (EcLibVars.RspInfo[ATCMD_SENDING_INDEX].Validity == RSP_NONE)
                 && (EcLibVars.RawDataRsp.ExpectedLength == 0))
          {
            /* As soon as RawDataRsp.ExpectedLength is cleared, it means raw data is available */
            EcLibVars.RawDataRsp.Data[EcLibVars.RawDataRsp.ReceivedLength] = '\0';
            if (EcLibVars.SeqAtCmdSend.Params.pAtRespCallbackFunc != NULL)
            {
              EcLibVars.SeqAtCmdSend.Params.pAtRespCallbackFunc(EcLibVars.RawDataRsp.Data + 2); /* +2 in order not to send initial \r\n char in string */
            }
            EcLibVars.SeqAtCmdSend.FsmState = ATCMD_SEND_STATE_END;
          }
        }
      }
      break;
    case ATCMD_SEND_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case ATCMD_SEND_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqAtCmdSend.TimerId);
      /* End the sequence */
      EcLibVars.RspInfo[ATCMD_SENDING_INDEX].Validity = RSP_NONE;
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      break;

    default:
      break;
  }
  return result;
}


/**
* @brief Sub-function handling the enabling of the asynchonous URC reception from ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_AsyncUrcRcvStartStop(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SeqAsyncUrcRcv.FsmState != ASYNC_URC_RCV_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqAsyncUrcRcv.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_ERROR;
  }

  switch (EcLibVars.SeqAsyncUrcRcv.FsmState)
  {
    case ASYNC_URC_RCV_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqAsyncUrcRcv.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqAsyncUrcRcv.Params.Timeout);
      if (EcLibVars.SeqAsyncUrcRcv.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_ERROR;
      }
      else
      {
        if (EcLibVars.SeqAsyncUrcRcv.Params.pUrcTag != NULL)
        {
          /* Prepare URC tag and get ready for URC reception */
          EcLibVars.RspInfo[ASYNC_URC_RCV_INDEX].Hash = ST87EC_Lib_ComputeHash(EcLibVars.SeqAsyncUrcRcv.Params.pUrcTag, strlen(EcLibVars.SeqAsyncUrcRcv.Params.pUrcTag));
        }

        if (EcLibVars.SeqAsyncUrcRcv.Params.pCommand != NULL)
        {
          /* Send the AT command enabling the URC reception */
          EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
          ST87EC_Wrapper_SendCmd(EcLibVars.SeqAsyncUrcRcv.Params.pCommand);
          EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_WAIT_FOR_CNF;
        }
        else
        {
          /* No AT command needed to be sent: sequence is finished
             and directly go into waiting / stop waiting for URC */
          EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_END;
        }
      }
      break;

    case ASYNC_URC_RCV_STATE_WAIT_FOR_CNF:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        /* AT command safely ack'ed: sequence is now finished
           and go into waiting / stop waiting for URC */
        EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_END;
      }
      break;

    case ASYNC_URC_RCV_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case ASYNC_URC_RCV_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqAsyncUrcRcv.TimerId);
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      if ((EcLibVars.SeqAsyncUrcRcv.Params.UrcRcvEnabling == ASYNC_URC_RCV_ENABLE) && (EcLibVars.ErrorOccurredInSeq == 0))
      {
        EcLibVars.RspInfo[ASYNC_URC_RCV_INDEX].Validity = RSP_AWAITED;
        EcLibVars.SeqAsyncUrcRcv.FsmState = ASYNC_URC_RCV_STATE_WAIT_FOR_URC;
      }
      else /* UrcRcvEnabling == ASYNC_URC_RCV_DISABLE,... */
      {
        EcLibVars.RspInfo[ASYNC_URC_RCV_INDEX].Validity = RSP_NONE;
      }
      break;

    default:
      break;
  }
  return result;
}


/**
* @brief Sequence managing asynchronous URC reception registered via ST87EC_Lib_AsyncUrcReception API
*
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_AsyncUrcReceive(void)
{
  if (EcLibVars.RspInfo[ASYNC_URC_RCV_INDEX].Validity == RSP_RECEIVED)
  {
    if (EcLibVars.SeqAsyncUrcRcv.Params.pAsyncUrcCallbackFunc != NULL)
    {
      EcLibVars.SeqAsyncUrcRcv.Params.pAsyncUrcCallbackFunc(EcLibVars.RspData);
    }
    EcLibVars.RspInfo[ASYNC_URC_RCV_INDEX].Validity = RSP_AWAITED;
  }

  return RESULT_OK;
}


/**
* @brief Sub-function handling the sequence for the import of TLS Provisioning elements to ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_TlsProvisioningImportElement(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SeqTlsProvisioning.FsmState != TLS_PROV_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqTlsProvisioning.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
  }

  switch (EcLibVars.SeqTlsProvisioning.FsmState)
  {
    case TLS_PROV_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqTlsProvisioning.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqTlsProvisioning.Params.Timeout);
      if (EcLibVars.SeqTlsProvisioning.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else
      {
        /* Send the corresponding AT command */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        if (EcLibVars.SeqTlsProvisioning.Params.ImportElementType == TLS_PROV_ELEM_PSK)
        {
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_IMPORT_PSK_CMD);
        }
        else if (EcLibVars.SeqTlsProvisioning.Params.ImportElementType == TLS_PROV_ELEM_PRIV_KEY)
        {
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_IMPORT_PRIVKEY_CMD);
        }
        else /* Device/Root certificates, PSK_IDs */
        {
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_IMPORT_CERT_CMD);
        }
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_RSP;
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        ST87EC_Wrapper_SendCmd(ST87EC_NVM_WRITE_CMD);
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_NVM_WRITE;
      }
      else {}
      break;

    case TLS_PROV_STATE_WAIT_FOR_NVM_WRITE:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
      }
      else {}
      break;

    case TLS_PROV_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case TLS_PROV_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqTlsProvisioning.TimerId);
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_INIT;
      break;

    default:
      break;
  }
  return result;
}


/**
* @brief Sub-function handling the sequence for the generation of TLS Provisioning elements in ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_TlsProvisioningGenerateElement(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;
 int tmp0, tmp1, tmp2, tmp3, tmp4;

  /* Check timer expiration */
  if ((EcLibVars.SeqTlsProvisioning.FsmState != TLS_PROV_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqTlsProvisioning.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
  }

  switch (EcLibVars.SeqTlsProvisioning.FsmState)
  {
    case TLS_PROV_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqTlsProvisioning.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqTlsProvisioning.Params.Timeout);
      if (EcLibVars.SeqTlsProvisioning.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else
      {
        /* Send the corresponding AT command */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        if (EcLibVars.SeqTlsProvisioning.Params.GenerateElementType == TLS_PROV_ELEM_DEV_PRIV_KEY)
        {
          EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity = RSP_AWAITED;
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_GENERATE_PRIVKEY_CMD);
        }
        else if (EcLibVars.SeqTlsProvisioning.Params.GenerateElementType == TLS_PROV_ELEM_CSR)
        {
          EcLibVars.RspInfo[TLSPROV_CSIGN_INDEX].Validity = RSP_AWAITED;
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_GENERATE_CSR_CMD);
        }
        else {}
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_RSP;
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else if (EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity = RSP_NONE;
        /* As soon as URC is received, parse it to get data length, then
           immediately catch coming raw data with the correct expected length. */
        sscanf((const char *)&EcLibVars.RspData[TLSPROV_KADD_TAG_SIZE - 1], "%d,%d,%d,%d,%d", &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
        if ((tmp4*2) < ST87EC_RAW_BUFFER_SIZE)  /* *2 on tmp4 to convert in bytes (as tmp2 contains the nb of double-digits in Hex format) */
        {
          EcLibVars.RawDataRsp.ReceivedLength = 0;
          EcLibVars.RawDataRsp.ExpectedLength = (uint16_t)(tmp4*2);
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_DATA_RSP;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      else if (EcLibVars.RspInfo[TLSPROV_CSIGN_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.RspInfo[TLSPROV_CSIGN_INDEX].Validity = RSP_NONE;
        /* As soon as URC is received, parse it to get data length, then
           immediately catch coming raw data with the correct expected length. */
        sscanf((const char *)&EcLibVars.RspData[TLSPROV_CSIGN_TAG_SIZE - 1], "%d,%d,%d", &tmp0, &tmp1, &tmp2);
        if ((tmp2*2) < ST87EC_RAW_BUFFER_SIZE)  /* *2 on tmp2 to convert in bytes (as tmp2 contains the nb of double-digits in Hex format) */
        {
          EcLibVars.RawDataRsp.ReceivedLength = 0;
          EcLibVars.RawDataRsp.ExpectedLength = (uint16_t)(tmp2*2);
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_DATA_RSP;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      else {}
      break;

    case TLS_PROV_STATE_WAIT_FOR_DATA_RSP:
      if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        /* As soon as RawDataRsp.ExpectedLength is cleared, it means raw data is available */
        if (EcLibVars.RawDataRsp.ExpectedLength == 0)
        {
          EcLibVars.RawDataRsp.Data[EcLibVars.RawDataRsp.ReceivedLength] = '\0';
          /* Call the customer callback with the returned length (in nb of double-digits in Hex format) and data as parameter */
          EcLibVars.SeqTlsProvisioning.Params.pTlsProvCallback(((int32_t)EcLibVars.RawDataRsp.ReceivedLength/2), EcLibVars.RawDataRsp.Data); /* Use API callback to send back the data */

          EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
          if (EcLibVars.SeqTlsProvisioning.Params.GenerateElementType == TLS_PROV_ELEM_DEV_PRIV_KEY)
          {
            EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
            ST87EC_Wrapper_SendCmd(ST87EC_NVM_WRITE_CMD);
            EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_NVM_WRITE;
          }
          else if (EcLibVars.SeqTlsProvisioning.Params.GenerateElementType == TLS_PROV_ELEM_CSR)
          {
            EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
          }
          else {}
        }
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_NVM_WRITE:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
      }
      break;

    case TLS_PROV_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case TLS_PROV_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqTlsProvisioning.TimerId);
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_INIT;
      break;

    default:
      break;
  }
  return result;
}


/**
* @brief Sub-function handling the sequence for the deletion of TLS Provisioning elements in ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_TlsProvisioningDeleteElement(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SeqTlsProvisioning.FsmState != TLS_PROV_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqTlsProvisioning.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
  }

  switch (EcLibVars.SeqTlsProvisioning.FsmState)
  {
    case TLS_PROV_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqTlsProvisioning.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqTlsProvisioning.Params.Timeout);
      if (EcLibVars.SeqTlsProvisioning.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else
      {
        /* Send the corresponding AT command */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        if (EcLibVars.SeqTlsProvisioning.Params.DelElementType == TLS_PROV_DEL_ELEM_ALL_CERTS)
        {
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_CDEL_ALL_CMD);
        }
        else if (EcLibVars.SeqTlsProvisioning.Params.DelElementType == TLS_PROV_DEL_ELEM_KEY)
        {
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_KDEL_ALL_CMD);
        }
        else
        {
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_CDEL_CMD);
        }
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_RSP;
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        /* Note: if +CME ERROR: 1302: means nothing has to be deleted for given SecId (empty)! */
        if (ST87EC_Lib_GetCmeErrorCode() == CME_ERR_CODE_1302)
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        ST87EC_Wrapper_SendCmd(ST87EC_NVM_WRITE_CMD);
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_NVM_WRITE;
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_NVM_WRITE:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
      }
      break;

    case TLS_PROV_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case TLS_PROV_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqTlsProvisioning.TimerId);
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_INIT;
      break;

    default:
      break;
  }
  return result;
}


/**
* @brief Sub-function handling the sequence for the listing of TLS Provisioning elements from ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_TlsProvisioningListElement(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SeqTlsProvisioning.FsmState != TLS_PROV_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqTlsProvisioning.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
  }

  switch (EcLibVars.SeqTlsProvisioning.FsmState)
  {
    case TLS_PROV_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqTlsProvisioning.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqTlsProvisioning.Params.Timeout);
      if (EcLibVars.SeqTlsProvisioning.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else
      {
        /* Send the corresponding AT command */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        EcLibVars.RspInfo[TLSPROV_CLIST_INDEX].Validity = RSP_AWAITED;
        EcLibVars.RspInfo[TLSPROV_KLIST_INDEX].Validity = RSP_AWAITED;
        if (EcLibVars.SeqTlsProvisioning.Params.SecureId == (-1))
        {
          /* No SecureId given, means list all */
          ST87EC_TLSPROV_LIST_ALL_CMD
        }
        else
        {
          ST87EC_TLSPROV_LIST_CMD
        }
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_RSP;
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        if (ST87EC_Lib_GetCmeErrorCode() == CME_ERR_CODE_1302)
        {
          /* List is empty! */
          /* Send 0 length and NULL data via API callback and end sequence */
          EcLibVars.SeqTlsProvisioning.Params.pTlsProvCallback(0, '\0');
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      else if (EcLibVars.RspInfo[TLSPROV_CLIST_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.SeqTlsProvisioning.Params.pTlsProvCallback((-1), &EcLibVars.RspData[TLSPROV_CLIST_TAG_SIZE -1]); /* Use API callback to send back the data */
        EcLibVars.RspInfo[TLSPROV_CLIST_INDEX].Validity = RSP_AWAITED;
      }
      else if (EcLibVars.RspInfo[TLSPROV_KLIST_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.SeqTlsProvisioning.Params.pTlsProvCallback((-1), &EcLibVars.RspData[TLSPROV_KLIST_TAG_SIZE -1]); /* Use API callback to send back the data */
        EcLibVars.RspInfo[TLSPROV_KLIST_INDEX].Validity = RSP_AWAITED;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
      }
      break;

    case TLS_PROV_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case TLS_PROV_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqTlsProvisioning.TimerId);
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_INIT;
      EcLibVars.RspInfo[TLSPROV_CLIST_INDEX].Validity = RSP_NONE;
      EcLibVars.RspInfo[TLSPROV_KLIST_INDEX].Validity = RSP_NONE;
      break;

    default:
      break;
  }
  return result;
}


/**
* @brief Sub-function handling the sequence for the dumping of TLS Provisioning elements from ST87Mxx
*
* @param : None
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceSys_TlsProvisioningDumpElement(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;
  int tmp0, tmp1, tmp2, tmp3, tmp4;

  /* Check timer expiration */
  if ((EcLibVars.SeqTlsProvisioning.FsmState != TLS_PROV_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SeqTlsProvisioning.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
  }

  switch (EcLibVars.SeqTlsProvisioning.FsmState)
  {
    case TLS_PROV_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SeqTlsProvisioning.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SeqTlsProvisioning.Params.Timeout);
      if (EcLibVars.SeqTlsProvisioning.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
      }
      else
      {
        /* Send the corresponding AT command */
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        if (EcLibVars.SeqTlsProvisioning.Params.DumpElementType == TLS_PROV_DUMP_ELEM_PUB_KEY)
        {
          EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity = RSP_AWAITED;
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_KDUMP_CMD);
        }
        else /* elem to dump is either root CA or device certificate or PSK_ID */
        {
          EcLibVars.RspInfo[TLSPROV_CADD_INDEX].Validity = RSP_AWAITED;
          ST87EC_Wrapper_SendCmd(ST87EC_TLSPROV_CDUMP_CMD);
        }
        EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_RSP;
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_RSP:
      /* Check if awaited response is received */
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
        if (ST87EC_Lib_GetCmeErrorCode() == CME_ERR_CODE_1302)
        {
          /* Dumped element empty! */
          /* Send 0 length and NULL data via API callback and end sequence */
          EcLibVars.SeqTlsProvisioning.Params.pTlsProvCallback(0, '\0');
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      else if (EcLibVars.RspInfo[TLSPROV_CADD_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.RspInfo[TLSPROV_CADD_INDEX].Validity = RSP_NONE;
        /* As soon as URC is received, parse it to get data length, then
           immediately catch coming raw data with the correct expected length. */
        sscanf((const char *)&EcLibVars.RspData[TLSPROV_CADD_TAG_SIZE - 1], "%d,%d,%d", &tmp0, &tmp1, &tmp2);
        if ((tmp2*2) < ST87EC_RAW_BUFFER_SIZE)  /* *2 on tmp2 to convert in bytes (as tmp2 contains the nb of double-digits in Hex format) */
        {
          EcLibVars.RawDataRsp.ReceivedLength = 0;
          EcLibVars.RawDataRsp.ExpectedLength = (uint16_t)(tmp2*2);
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_DATA_RSP;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      else if (EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity = RSP_NONE;
        /* As soon as URC is received, parse it to get data length, then
           immediately catch coming raw data with the correct expected length. */
        sscanf((const char *)&EcLibVars.RspData[TLSPROV_KADD_TAG_SIZE - 1], "%d,%d,%d,%d,%d", &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
        if ((tmp4*2) < ST87EC_RAW_BUFFER_SIZE)  /* *2 on tmp4 to convert in bytes (as tmp2 contains the nb of double-digits in Hex format) */
        {
          EcLibVars.RawDataRsp.ReceivedLength = 0;
          EcLibVars.RawDataRsp.ExpectedLength = (uint16_t)(tmp4*2);
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_WAIT_FOR_DATA_RSP;
        }
        else
        {
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_ERROR;
        }
      }
      break;

    case TLS_PROV_STATE_WAIT_FOR_DATA_RSP:
      if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        /* As soon as RawDataRsp.ExpectedLength is cleared, it means raw data is available */
        if (EcLibVars.RawDataRsp.ExpectedLength == 0)
        {
          EcLibVars.RawDataRsp.Data[EcLibVars.RawDataRsp.ReceivedLength] = '\0';
          /* Call the customer callback with the returned length (in nb of double-digits in Hex format) and data as parameter */
          EcLibVars.SeqTlsProvisioning.Params.pTlsProvCallback(((int32_t)EcLibVars.RawDataRsp.ReceivedLength/2), EcLibVars.RawDataRsp.Data); /* Use API callback to send back the data */

          EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
          EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_END;
        }
      }
      break;

    case TLS_PROV_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
    case TLS_PROV_STATE_END:
      /* Stop the timer */
      ST87EC_Wrapper_StopTimer(EcLibVars.SeqTlsProvisioning.TimerId);
      if (EcLibVars.ErrorOccurredInSeq == 1)
      {
        result = RESULT_KO;
      }
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SeqTlsProvisioning.FsmState = TLS_PROV_STATE_INIT;
      EcLibVars.RspInfo[TLSPROV_CADD_INDEX].Validity = RSP_NONE;
      EcLibVars.RspInfo[TLSPROV_KADD_INDEX].Validity = RSP_NONE;
      break;

    default:
      break;
  }

  return result;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

