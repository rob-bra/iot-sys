/**
  ******************************************************************************
  * @file    st87ec_sequence_wifi.c
  * @author  APMS Application Team
  * @brief   EC Wifi scanning sequence functions
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

#include <string.h>
#include <stdio.h>
#include "st87ec_sequence_wifi.h"
#include "st87ec_wrapper.h"

/* Exported functions --------------------------------------------------------*/

/* Constants -----------------------------------------------------------------*/
#define ST87EC_FORMAT_WIFI_SCAN_START_CMD(CHANLIST) "AT#WSCAN=1,%s", CHANLIST
#define ST87EC_WIFI_SCAN_START_CMD ST87EC_FORMAT_WIFI_SCAN_START_CMD( \
                                   EcLibVars.SequenceWifi.Scan.Params.pChanList)

/* Global variables ----------------------------------------------------------*/


/**
* @brief Sequence for the Wifi scanning request
*
* @retval Function execution status
*/
ST87EC_Lib_Result_t ST87EC_SequenceWifi_Scan(void)
{
  ST87EC_Lib_Result_t result = RESULT_OK;

  /* Check timer expiration */
  if ((EcLibVars.SequenceWifi.Scan.FsmState != WSCAN_STATE_INIT)
      && (ST87EC_Wrapper_GetTimerStatus(EcLibVars.SequenceWifi.Scan.TimerId,1) == TIMER_STATUS_ELAPSED))
  {
    EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_ERROR;
  }

  switch (EcLibVars.SequenceWifi.Scan.FsmState)
  {
    case WSCAN_STATE_INIT:
      /* Start the timeout */
      EcLibVars.SequenceWifi.Scan.TimerId = ST87EC_Wrapper_StartTimer(EcLibVars.SequenceWifi.Scan.Params.TimeoutMs);
      if (EcLibVars.SequenceWifi.Scan.TimerId == ST87EC_TIMER_ERROR)
      {
        /* Error occurred */
        EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_ERROR;
      }
      else
      {
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        /* Send the Wifi scanning start AT cmd */
        ST87EC_Wrapper_SendCmd(ST87EC_WIFI_SCAN_START_CMD);
        EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_WAIT_FOR_START_CNF;
      }
      break;

    case WSCAN_STATE_WAIT_FOR_START_CNF:
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
         EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.RspInfo[WSCAN_DATA_INDEX].Validity = RSP_AWAITED;
        EcLibVars.RspInfo[WSCAN_RESTART_INDEX].Validity = RSP_AWAITED;
        EcLibVars.SequenceWifi.Scan.Started = 1;  /* indicate Wifi scan has started */
        EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_SCAN_WAIT_FOR_URCS;
      }
      else {}
      break;

    case WSCAN_STATE_SCAN_WAIT_FOR_URCS:
      /* Check if awaited URCs are received */
      if (EcLibVars.RspInfo[WSCAN_DATA_INDEX].Validity == RSP_RECEIVED)
      {
        /* Wifi scan data URC is received, call the customer callback function with received data as parameter ( +tag_length to remove the "#WSCAN:" ) */
        EcLibVars.SequenceWifi.Scan.Params.pGetBeaconDataCallbackFunc(&EcLibVars.RspData[WSCAN_DATA_TAG_SIZE]);
        EcLibVars.RspInfo[WSCAN_DATA_INDEX].Validity = RSP_AWAITED;
      }
      else if (EcLibVars.RspInfo[WSCAN_RESTART_INDEX].Validity == RSP_RECEIVED)
      {
        EcLibVars.SequenceWifi.Scan.IterCnt++;
        //ST87EC_MCSH_Printf("\r\n   Iter=%d\r\n",EcLibVars.SequenceWifi.Scan.IterCnt);
        EcLibVars.RspInfo[WSCAN_RESTART_INDEX].Validity = RSP_AWAITED;
        /* Stop reception if counter reaches NbScanIter value */
        if (EcLibVars.SequenceWifi.Scan.IterCnt >= EcLibVars.SequenceWifi.Scan.Params.NbScanIter)
        {
          EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_SCAN_STOP;
        }
      }
      else {}
      break;

    case WSCAN_STATE_SCAN_STOP:
        /* indicate Wifi scan has stopped here
           (so that FSM does not loop in the stop request in case it fails (CME error) */
        EcLibVars.SequenceWifi.Scan.Started = 0;
        /* Disable URCs listening and launch scan stop cmd */
        EcLibVars.RspInfo[WSCAN_DATA_INDEX].Validity = RSP_NONE;
        EcLibVars.RspInfo[WSCAN_RESTART_INDEX].Validity = RSP_NONE;
        EcLibVars.RspReceived = SPECIFIC_RSP_NONE;
        ST87EC_Wrapper_SendCmd("AT#WSCAN=0");
        EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_WAIT_FOR_STOP_CNF;
      break;

    case WSCAN_STATE_WAIT_FOR_STOP_CNF:
      if (EcLibVars.RspReceived == SPECIFIC_RSP_CME)
      {
         EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_ERROR;
      }
      else if (EcLibVars.RspReceived == SPECIFIC_RSP_OK)
      {
        EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_END_SEQUENCE;
      }
      else {}
      break;

    case WSCAN_STATE_ERROR:
      EcLibVars.ErrorOccurredInSeq = 1;
      /* If Wifi scanning is on-going when error occurs, execute a Stop
         prior to proceeding with sequence end */
      if (EcLibVars.SequenceWifi.Scan.Started == 1)
      {
         EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_SCAN_STOP;
      }
      else
      {
        EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_END_SEQUENCE;
      }
      break;

    case WSCAN_STATE_END_SEQUENCE:
      /* Stop the timer */
      result = ST87EC_Wrapper_StopTimer(EcLibVars.SequenceWifi.Scan.TimerId);
      /* End the sequence */
      EcLibVars.OnGoingSequence = SEQUENCE_NONE;
      EcLibVars.SequenceWifi.Scan.FsmState = WSCAN_STATE_INIT;

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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

