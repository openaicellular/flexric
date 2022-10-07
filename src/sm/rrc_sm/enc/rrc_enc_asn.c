#include "../ie/asn/E2SM-RRCStats-EventTriggerDefinition.h"
#include "../ie/asn/E2SM-RRCStats-EventTriggerDefinition-Format1.h"
#include "../ie/asn/E2SM-RRCStats-IndicationHeader.h"
#include "../ie/asn/E2SM-RRCStats-IndicationHeader-Format1.h"
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

#include "rrc_enc_asn.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

byte_array_t rrc_enc_event_trigger_asn(rrc_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);

  E2SM_RRCStats_EventTriggerDefinition_t *pdu = calloc(1,sizeof(E2SM_RRCStats_EventTriggerDefinition_t));
  assert ( pdu != NULL && "Memory exhausted" );

  pdu->present = E2SM_RRCStats_EventTriggerDefinition_PR_eventDefinition_Format1;
  pdu->choice.eventDefinition_Format1 = calloc (1,sizeof(E2SM_RRCStats_EventTriggerDefinition_Format1_t));
  switch (event_trigger->ms){
    case 1: pdu->choice.eventDefinition_Format1->triggerNature = RRCStats_TriggerNature_oneMs;
            break;
    case 2: pdu->choice.eventDefinition_Format1->triggerNature = RRCStats_TriggerNature_twoMs;
            break;
    case 5: pdu->choice.eventDefinition_Format1->triggerNature = RRCStats_TriggerNature_fiveMs;
            break;
    default : assert (0 != 0 && "Invalid value");
  }
  
  byte_array_t ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_RRCStats_EventTriggerDefinition, pdu, ba.buf, ba.len);
  assert(er.encoded > -1);
  ba.len = er.encoded;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_EventTriggerDefinition,pdu);
  free(pdu);

  return ba;
}

byte_array_t rrc_enc_action_def_asn(rrc_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;

}

byte_array_t rrc_enc_ind_hdr_asn(rrc_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  E2SM_RRCStats_IndicationHeader_t *pdu = calloc(1,sizeof(E2SM_RRCStats_IndicationHeader_t));
  assert ( pdu !=NULL && "Memory exhausted" );

  pdu->present = E2SM_RRCStats_IndicationHeader_PR_indicationHeader_Format1;
  pdu->choice.indicationHeader_Format1 = calloc(1,sizeof(E2SM_RRCStats_IndicationHeader_Format1_t));
  pdu->choice.indicationHeader_Format1->indicationHeaderParam = 0;

  byte_array_t  ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_RRCStats_IndicationHeader, pdu, ba.buf, ba.len);
  assert(er.encoded > -1);
  ba.len = er.encoded;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_IndicationHeader,pdu);
  free(pdu);

  return ba;
}

byte_array_t rrc_enc_ind_msg_asn(rrc_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  E2SM_RRCStats_IndicationMessage_t *pdu = calloc(1,sizeof(E2SM_RRCStats_IndicationMessage_t));
  assert (pdu !=NULL && "Memory exhausted" );

  pdu->present = E2SM_RRCStats_IndicationMessage_PR_indicationMessage_Format1;
  pdu->choice.indicationMessage_Format1 = calloc(1,sizeof(E2SM_RRCStats_IndicationMessage_Format1_t));
  assert ( pdu->choice.indicationMessage_Format1 !=NULL && "Memory exhausted" );
  pdu->choice.indicationMessage_Format1->indicationMsgParam.tstamp = ind_msg->tstamp;
  
  for ( size_t i=0; i < ind_msg->len_ue_stats; ++i ){
    RRCUeStats_t *ueStat = calloc(1,sizeof(RRCUeStats_t));
    ueStat->rnti = ind_msg->ue_stats[i].rnti;
    ueStat->rbid = ind_msg->ue_stats[i].rbid;
    for ( size_t j=0; j < 8; ++j ){
      long *drb = calloc(1, sizeof(long));
      *drb = (long) ind_msg->ue_stats[i].DRB_active[j];
      int rc = ASN_SEQUENCE_ADD(&ueStat->drbActive.list, drb);
      assert(rc==0);
    }
    ueStat->statusRrc = ind_msg->ue_stats[i].StatusRrc;
    ueStat->setupPduSession = ind_msg->ue_stats[i].setup_pdu_session;
    ueStat->nbOfPdusessions = ind_msg->ue_stats[i].nb_of_pdusessions;
    ueStat->ueRrcInactivityTimer = ind_msg->ue_stats[i].ue_rrc_inactivity_timer;
  
    int rc = ASN_SEQUENCE_ADD(&pdu->choice.indicationMessage_Format1->indicationMsgParam.ueStats.list, ueStat);
    assert(rc==0);
  }
  
  byte_array_t ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_RRCStats_IndicationMessage, pdu, ba.buf, ba.len);
  assert(er.encoded > -1);
  ba.len = er.encoded;

  assert(pdu != NULL);
  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_IndicationMessage,pdu);
  free(pdu);

  return ba;
}

byte_array_t rrc_enc_call_proc_id_asn(rrc_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t rrc_enc_ctrl_hdr_asn(rrc_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);

  E2SM_RRCStats_ControlHeader_t *pdu = calloc(1,sizeof(E2SM_RRCStats_ControlHeader_t));
  assert ( pdu !=NULL && "Memory exhausted" );

  pdu->present = E2SM_RRCStats_ControlHeader_PR_controlHeader_Format1;
  pdu->choice.controlHeader_Format1 = calloc(1,sizeof(E2SM_RRCStats_ControlHeader_Format1_t));
  pdu->choice.controlHeader_Format1->controlHeaderParam = 0;

  byte_array_t  ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_RRCStats_ControlHeader, pdu, ba.buf, ba.len);
  assert(er.encoded > -1);
  ba.len = er.encoded;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_ControlHeader,pdu);
  free(pdu);

  return ba;
}

byte_array_t rrc_enc_ctrl_msg_asn(rrc_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  E2SM_RRCStats_ControlMessage_t *pdu = calloc(1,sizeof(E2SM_RRCStats_ControlMessage_t));
  assert ( pdu !=NULL && "Memory exhausted" );

  pdu->present = E2SM_RRCStats_ControlMessage_PR_controlMessage_Format1;
  pdu->choice.controlMessage_Format1 = calloc(1,sizeof(E2SM_RRCStats_ControlMessage_Format1_t));
  pdu->choice.controlMessage_Format1->controlMsgParam = 42;

  byte_array_t  ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, &asn_DEF_E2SM_RRCStats_ControlMessage, pdu, ba.buf, ba.len);
  assert(er.encoded > -1);
  ba.len = er.encoded;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_ControlMessage,pdu);
  free(pdu);

  return ba;
}

byte_array_t rrc_enc_ctrl_out_asn(rrc_ctrl_out_t const* ctrl_out) 
{
  assert(ctrl_out != NULL);

  E2SM_RRCStats_ControlOutcome_t *pdu = calloc(1,sizeof(E2SM_RRCStats_ControlOutcome_t));
  assert ( pdu != NULL && "Memory exhausted" );

  pdu->present = E2SM_RRCStats_ControlOutcome_PR_controlOutcome_Format1;
  pdu->choice.controlOutcome_Format1 = calloc (1,sizeof(E2SM_RRCStats_ControlOutcome_Format1_t));
  switch (ctrl_out->ans){
    case RRC_CTRL_OUT_OK: pdu->choice.controlOutcome_Format1->outcome = RRCStats_ControlOutcome_rrcCtrlOutcomeOk;
            break;
    case RRC_CTRL_OUT_END: pdu->choice.controlOutcome_Format1->outcome = RRCStats_ControlOutcome_rrcCtrlOutcomeEnd;
            break;
    default : assert (0 != 0 && "Invalid value");
  }
  
  byte_array_t ba = {.buf = malloc(2048), .len = 2048};
  const enum asn_transfer_syntax syntax = ATS_UNALIGNED_BASIC_PER;
  asn_enc_rval_t er = asn_encode_to_buffer(NULL, syntax, 
    &asn_DEF_E2SM_RRCStats_ControlOutcome, pdu, ba.buf, ba.len);
  assert(er.encoded > -1);
  ba.len = er.encoded;

  ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_E2SM_RRCStats_ControlOutcome,pdu);
  free(pdu);

  return ba;
}

byte_array_t rrc_enc_func_def_asn(rrc_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}