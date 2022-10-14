#include "../ie/asn/E2SM-RRCStats-EventTriggerDefinition-Format1.h"
#include "../ie/asn/E2SM-RRCStats-EventTriggerDefinition.h"
#include "../ie/asn/E2SM-RRCStats-IndicationHeader-Format1.h"
#include "../ie/asn/E2SM-RRCStats-IndicationHeader.h"
#include "../ie/asn/RRCStats-Header.h"
#include "../ie/asn/E2SM-RRCStats-IndicationMessage.h"
#include "../ie/asn/E2SM-RRCStats-IndicationMessage-Format1.h"
#include "../ie/asn/RRCUeStats.h"
#include "../ie/asn/RRCStats-Message.h"
#include "../ie/asn/RRCStats-TriggerNature.h"
#include "../ie/asn/E2SM-RRCStats-ControlHeader.h"
#include "../ie/asn/E2SM-RRCStats-ControlHeader-Format1.h"
#include "../ie/asn/E2SM-RRCStats-ControlMessage.h"
#include "../ie/asn/E2SM-RRCStats-ControlMessage-Format1.h"
#include "../ie/asn/E2SM-RRCStats-ControlOutcome.h"
#include "../ie/asn/E2SM-RRCStats-ControlOutcome-Format1.h"

#include "rrc_dec_asn.h"

#include <assert.h>

rrc_event_trigger_t rrc_dec_event_trigger_asn(size_t len, uint8_t const buf[len])
{
  assert(len>0);
  assert(buf != NULL);
  E2SM_RRCStats_EventTriggerDefinition_t *pdu = calloc(1, sizeof(E2SM_RRCStats_EventTriggerDefinition_t));
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_RRCStats_EventTriggerDefinition, (void**)&pdu, buf, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");

  rrc_event_trigger_t ret = {0};

  assert ( pdu->present == E2SM_RRCStats_EventTriggerDefinition_PR_eventDefinition_Format1 && "Invalid Choice"  );

  switch (pdu->choice.eventDefinition_Format1->triggerNature )
  {
    case RRCStats_TriggerNature_oneMs: ret.ms = 0;
                                       break;
    case RRCStats_TriggerNature_twoMs: ret.ms = 1;
                                       break;
    case RRCStats_TriggerNature_fiveMs: ret.ms = 5;
                                       break;
    default : assert ( 0!=0 && "Invalid value");
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_EventTriggerDefinition,pdu);
  free(pdu);

  return ret;
}

rrc_action_def_t rrc_dec_action_def_asn(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  rrc_action_def_t avoid_warning;
  return avoid_warning;
}

rrc_ind_hdr_t rrc_dec_ind_hdr_asn(size_t len, uint8_t const ind_hdr[len])
{
  assert(len>0);
  assert(ind_hdr != NULL);

  E2SM_RRCStats_IndicationHeader_t *pdu = calloc(1, sizeof(E2SM_RRCStats_IndicationHeader_t));
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_RRCStats_IndicationHeader, (void**)&pdu, ind_hdr, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");

  assert ( pdu->present == E2SM_RRCStats_IndicationHeader_PR_indicationHeader_Format1 && "Invalid format" );

  rrc_ind_hdr_t ret = {0};

  ret.dummy = pdu->choice.indicationHeader_Format1->indicationHeaderParam;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_IndicationHeader,pdu);
  free(pdu);

  return ret;
}

rrc_ind_msg_t rrc_dec_ind_msg_asn(size_t len, uint8_t const ind_msg[len])
{
  assert(len>0);
  assert(ind_msg != NULL);

  E2SM_RRCStats_IndicationMessage_t *pdu = calloc(1, sizeof(E2SM_RRCStats_IndicationMessage_t));
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_RRCStats_IndicationMessage, (void**)&pdu, ind_msg, len);

  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");

  assert ( pdu->present == E2SM_RRCStats_IndicationMessage_PR_indicationMessage_Format1 && "Invalid choice" );

  rrc_ind_msg_t ret = {0};

  ret.tstamp = pdu->choice.indicationMessage_Format1->indicationMsgParam.tstamp;
  ret.len_ue_stats = pdu->choice.indicationMessage_Format1->indicationMsgParam.ueStats.list.count;

  if(ret.len_ue_stats > 0){
    ret.ue_stats = calloc(ret.len_ue_stats, sizeof(rrc_ue_stats_impl_t));

    for ( size_t i=0; i < ret.len_ue_stats; ++i ){
      RRCUeStats_t *ue_stats = (RRCUeStats_t *)pdu->choice.indicationMessage_Format1->indicationMsgParam.ueStats.list.array[i];
      ret.ue_stats[i].rnti = ue_stats->rnti;
      ret.ue_stats[i].rbid = ue_stats->rbid;
      for ( size_t j; j < 8; ++j ){
        ret.ue_stats[i].DRB_active[j] = *ue_stats->drbActive.list.array[j];
      }
      ret.ue_stats[i].StatusRrc = ue_stats->statusRrc;
      ret.ue_stats[i].setup_pdu_session = ue_stats->setupPduSession;
      ret.ue_stats[i].nb_of_pdusessions = ue_stats->nbOfPdusessions;
      ret.ue_stats[i].ue_rrc_inactivity_timer = ue_stats->ueRrcInactivityTimer;
    }
  }
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_IndicationMessage,pdu);
  free(pdu);

  return ret;
}

rrc_call_proc_id_t rrc_dec_call_proc_id_asn(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
  rrc_call_proc_id_t avoid_warning;
  return avoid_warning;
}

rrc_ctrl_hdr_t rrc_dec_ctrl_hdr_asn(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len>0);
  assert(ctrl_hdr != NULL);

  E2SM_RRCStats_ControlHeader_t *pdu = calloc(1, sizeof(E2SM_RRCStats_ControlHeader_t));
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_RRCStats_ControlHeader, (void**)&pdu, ctrl_hdr, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");

  assert ( pdu->present == E2SM_RRCStats_ControlHeader_PR_controlHeader_Format1 && "Invalid format" );

  rrc_ctrl_hdr_t ret = {0};

  ret.dummy = pdu->choice.controlHeader_Format1->controlHeaderParam;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_ControlHeader,pdu);
  free(pdu);

  return ret;
}

rrc_ctrl_msg_t rrc_dec_ctrl_msg_asn(size_t len, uint8_t const ctrl_msg[len])
{
  assert(len>0);
  assert(ctrl_msg != NULL);

  E2SM_RRCStats_ControlMessage_t *pdu = calloc(1, sizeof(E2SM_RRCStats_ControlMessage_t));
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_RRCStats_ControlMessage, (void**)&pdu, ctrl_msg, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");

  assert ( pdu->present == E2SM_RRCStats_ControlMessage_PR_controlMessage_Format1 && "Invalid format" );

  rrc_ctrl_msg_t ret = {0};

  ret.action = pdu->choice.controlMessage_Format1->controlMsgParam;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_ControlMessage,pdu);
  free(pdu);

  return ret;
}

rrc_ctrl_out_t rrc_dec_ctrl_out_asn(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(len>0);
  assert(ctrl_out != NULL);
  E2SM_RRCStats_ControlOutcome_t *pdu = calloc(1, sizeof(E2SM_RRCStats_ControlOutcome_t));
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_E2SM_RRCStats_ControlOutcome, (void**)&pdu, ctrl_out, len);
  assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGEND_BASIC_PER syntax?");

  rrc_ctrl_out_t ret = {0};

  assert ( pdu->present == E2SM_RRCStats_ControlOutcome_PR_controlOutcome_Format1 && "Invalid Choice"  );

  switch (pdu->choice.controlOutcome_Format1->outcome )
  {
    case RRCStats_ControlOutcome_rrcCtrlOutcomeOk: ret.ans = RRC_CTRL_OUT_OK;
                                       break;
    case RRCStats_ControlOutcome_rrcCtrlOutcomeEnd: ret.ans = RRC_CTRL_OUT_END;
                                       break;
    default : assert ( 0!=0 && "Invalid value");
  }

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_ControlOutcome,pdu);
  free(pdu);

  return ret;
}

rrc_func_def_t rrc_dec_func_def_asn(size_t len, uint8_t const func[len])
{
  assert(0!=0 && "Not implemented");
  assert(func != NULL);
  rrc_func_def_t  avoid_warning;
  return avoid_warning;
}

