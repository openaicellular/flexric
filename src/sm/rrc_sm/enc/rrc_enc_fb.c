
#include "rrc_enc_fb.h"
#include "../ie/flatbuffers_common_reader.h"
#include "../ie/flatbuffers_common_builder.h"
#include "../ie/e2sm_rrc_v00_builder.h"
#include "../ie/e2sm_rrc_v00_verifier.h"

#include <assert.h>
#include <stdio.h>

byte_array_t rrc_enc_action_def_fb(rrc_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t rrc_enc_event_trigger_fb(rrc_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  
  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  E2SM_RRCStats_EventTrigger_start_as_root(B);
  switch (event_trigger->ms)
  {
  case 1:
    E2SM_RRCStats_EventTrigger_trig_add(B, E2SM_RRCStats_TriggerNature_oneMs);
    break;
  case 2:
    E2SM_RRCStats_EventTrigger_trig_add(B, E2SM_RRCStats_TriggerNature_twoMs);
    break;
  case 5:
    E2SM_RRCStats_EventTrigger_trig_add(B, E2SM_RRCStats_TriggerNature_fiveMs);
    break;
  
  default:
    assert(0!=0 && "RRC_SM: Invalid Event Trigger MS");
    break;
  }

  E2SM_RRCStats_IndicationMessage_end_as_root(B);

  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(&builder, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if((ret = E2SM_RRCStats_EventTrigger_verify_as_root(buf, size))){
    printf("RRC_SM: Event Trigger is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t rrc_enc_ind_hdr_fb(rrc_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  E2SM_RRCStats_IndicationHeader_start_as_root(B);
  E2SM_RRCStats_IndicationHeaderFormat1_start(B);
  E2SM_RRCStats_IndicationHeaderFormat1_param_add(B, 0);
  E2SM_RRCStats_IndicationHeader_hdr_indHeaderF1_add(B, E2SM_RRCStats_IndicationHeaderFormat1_end(B));
  E2SM_RRCStats_IndicationHeader_end_as_root(B);
  
  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if((ret = E2SM_RRCStats_IndicationHeader_verify_as_root(buf, size))){
    printf("RRCSM: Indication Header is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t rrc_enc_ind_msg_fb(rrc_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  if(ind_msg->len_ue_stats != 0)
    assert(ind_msg->ue_stats != NULL);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  E2SM_RRCStats_IndicationMessage_start_as_root(B);
  E2SM_RRCStats_IndicationMessage_tstamp_add(B, ind_msg->tstamp);

  E2SM_RRCStats_IndicationMessage_ue_stats_start(B);
  for(uint32_t i=0;i<ind_msg->len_ue_stats;i++){
    E2SM_RRCStats_UEStats_start(B);
    E2SM_RRCStats_UEStats_rnti_add(B, ind_msg->ue_stats[i].rnti);
    E2SM_RRCStats_UEStats_rbid_add(B, ind_msg->ue_stats[i].rbid);

    E2SM_RRCStats_UEStats_drb_active_start(B);
    for(int j=0;j<8;j++)
      E2SM_RRCStats_UEStats_drb_active_push(B, (int8_t*)&ind_msg->ue_stats[i].DRB_active[j]);
    E2SM_RRCStats_UEStats_drb_active_end(B);

    E2SM_RRCStats_UEStats_status_rrc_add(B, ind_msg->ue_stats[i].StatusRrc);
    E2SM_RRCStats_UEStats_setup_pdu_session_add(B, ind_msg->ue_stats[i].setup_pdu_session);
    E2SM_RRCStats_UEStats_nb_of_pdu_sessions_add(B, ind_msg->ue_stats[i].nb_of_pdusessions);
    E2SM_RRCStats_UEStats_ue_rrc_inactivity_timer_add(B, ind_msg->ue_stats[i].ue_rrc_inactivity_timer);
    
    E2SM_RRCStats_IndicationMessage_ue_stats_push(B, E2SM_RRCStats_UEStats_end(B)); 
  }
  E2SM_RRCStats_IndicationMessage_ue_stats_end(B);
  
  E2SM_RRCStats_IndicationMessage_end_as_root(B);

  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if((ret = E2SM_RRCStats_IndicationMessage_verify_as_root(buf, size))){
    printf("RRCSM: Indication Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t rrc_enc_call_proc_id_fb(rrc_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t rrc_enc_ctrl_hdr_fb(rrc_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  E2SM_RRCStats_ControlHeader_start_as_root(B);
  E2SM_RRCStats_ControlHeaderFormat1_start(B);
  E2SM_RRCStats_ControlHeaderFormat1_param_add(B, 0);
  E2SM_RRCStats_ControlHeader_hdr_ctrlHeaderF1_add(B, E2SM_RRCStats_ControlHeaderFormat1_end(B));
  E2SM_RRCStats_ControlHeader_end_as_root(B);
  
  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if((ret = E2SM_RRCStats_ControlHeader_verify_as_root(buf, size))){
    printf("RRC_SM: Control Header is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t rrc_enc_ctrl_msg_fb(rrc_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  E2SM_RRCStats_ControlMessage_start_as_root(B);
  E2SM_RRCStats_ControlMessageFormat1_start(B);
  E2SM_RRCStats_ControlMessageFormat1_action_add(B, 42);
  E2SM_RRCStats_ControlMessage_msg_ctrlMessageF1_add(B, E2SM_RRCStats_ControlMessageFormat1_end(B));
  E2SM_RRCStats_ControlMessage_end_as_root(B);

  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(B, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if((ret = E2SM_RRCStats_ControlMessage_verify_as_root(buf, size))){
    printf("RRCSM: Control Message is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t rrc_enc_ctrl_out_fb(rrc_ctrl_out_t const* ctrl_out) 
{
  assert(ctrl_out != NULL);
  
  flatcc_builder_t builder;
  flatcc_builder_t* B = &builder;
  flatcc_builder_init(B);

  E2SM_RRCStats_ControlOutcome_start_as_root(B);
  switch (ctrl_out->ans)
  {
  case RRC_CTRL_OUT_OK:
    E2SM_RRCStats_ControlOutcome_ans_add(B, E2SM_RRCStats_ControlOutcomeResponse_RRC_CTRL_OUT_OK);
    break;
  case RRC_CTRL_OUT_END:
    E2SM_RRCStats_ControlOutcome_ans_add(B, E2SM_RRCStats_ControlOutcomeResponse_RRC_CTRL_OUT_END);
    break;
  
  default:
    assert(0!=0 && "RRC_SM: Invalid Control Outcome");
    break;
  }

  E2SM_RRCStats_ControlOutcome_end_as_root(B);

  size_t size = 0;
  uint8_t *buf = flatcc_builder_finalize_buffer(&builder, &size);
  byte_array_t ba = { .buf = buf, .len = size };

  int ret;
  if((ret = E2SM_RRCStats_ControlOutcome_verify_as_root(buf, size))){
    printf("RRC_SM: Control Outcome is invalid: %s\n", flatcc_verify_error_string(ret));
    assert(0);
  }

  flatcc_builder_clear(B);

  return ba;
}

byte_array_t rrc_enc_func_def_fb(rrc_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func != NULL);
  byte_array_t  ba = {0};
  return ba;
}

