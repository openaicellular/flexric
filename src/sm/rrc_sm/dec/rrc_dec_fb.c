#include "rrc_dec_fb.h"
#include "../ie/e2sm_rrc_v00_verifier.h"

#include <assert.h>
#include <stdio.h>

rrc_event_trigger_t rrc_dec_event_trigger_fb(size_t len, uint8_t const ev_tr[len])
{
  assert(len > 0);
  assert(ev_tr != NULL);

  int rc = E2SM_RRCStats_EventTrigger_verify_as_root(ev_tr, len);
  assert(rc == 0 && "RRC_SM: Invalid Event Trigger.");

  E2SM_RRCStats_EventTrigger_table_t ev_tr_fb = E2SM_RRCStats_EventTrigger_as_root(ev_tr);

  assert(ev_tr_fb != NULL);

  const uint8_t ms_val = E2SM_RRCStats_EventTrigger_trig(ev_tr_fb);

  rrc_event_trigger_t ret = {0};
  switch (ms_val)
  {
  case E2SM_RRCStats_TriggerNature_oneMs:
    ret.ms = 1;
    break;
  case E2SM_RRCStats_TriggerNature_twoMs:
    ret.ms = 2;
    break;
  case E2SM_RRCStats_TriggerNature_fiveMs:
    ret.ms = 5;
    break;
  default:
    assert(0!=0 && "RRC_SM: Invalid Event Trigger MS");
    break;
  }
  return ret;
}

rrc_action_def_t rrc_dec_action_def_fb(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  rrc_action_def_t avoid_warning;
  return avoid_warning;
}

rrc_ind_hdr_t rrc_dec_ind_hdr_fb(size_t len, uint8_t const ind_hdr[len])
{
  assert(ind_hdr != NULL);
  assert(len > 0);

  int rc = E2SM_RRCStats_IndicationHeader_verify_as_root(ind_hdr, len);
  assert(rc == 0 && "RRC_SM: Invalid Indication Header.");

  E2SM_RRCStats_IndicationHeader_table_t ind_hdr_fb = E2SM_RRCStats_IndicationHeader_as_root(ind_hdr);

  if(E2SM_RRCStats_IndicationHeader_hdr_type(ind_hdr_fb) != E2SM_RRCStats_IndicationHeaderUnion_indHeaderF1){
    assert(0!=0 && "RRC_SM: Invalid Indication Header Format");
  }
  E2SM_RRCStats_IndicationHeaderFormat1_table_t hdr = E2SM_RRCStats_IndicationHeader_hdr(ind_hdr_fb);

  rrc_ind_hdr_t ret = {0};
  ret.dummy = E2SM_RRCStats_IndicationHeaderFormat1_param(hdr);
  return ret;
}

rrc_ind_msg_t rrc_dec_ind_msg_fb(size_t len, uint8_t const ind_msg[len])
{
  assert(ind_msg != NULL);
  assert(len > 0);

  int rc = E2SM_RRCStats_IndicationMessage_verify_as_root(ind_msg, len);
  assert(rc==0 && "RRC_S: Invalid Indication Message.");

  E2SM_RRCStats_IndicationMessage_table_t ind_msg_fb = E2SM_RRCStats_IndicationMessage_as_root(ind_msg);

  rrc_ind_msg_t ret = {0};
  ret.tstamp = E2SM_RRCStats_IndicationMessage_tstamp(ind_msg_fb);
  E2SM_RRCStats_UEStats_vec_t ue_stats_vec = E2SM_RRCStats_IndicationMessage_ue_stats(ind_msg_fb);
  ret.len_ue_stats = E2SM_RRCStats_UEStats_vec_len(ue_stats_vec);

  if(ret.len_ue_stats > 0){
    ret.ue_stats = calloc(ret.len_ue_stats, sizeof(rrc_ue_stats_impl_t));
    assert(ret.ue_stats != NULL && "Memory Exhusted!");

    for(u_int32_t i=0; i<ret.len_ue_stats; i++){
      E2SM_RRCStats_UEStats_table_t ue_stats = E2SM_RRCStats_UEStats_vec_at(ue_stats_vec, i);
      ret.ue_stats[i].rnti = E2SM_RRCStats_UEStats_rnti(ue_stats);
      ret.ue_stats[i].rbid = E2SM_RRCStats_UEStats_rbid(ue_stats);

      flatbuffers_int8_vec_t drb_active = E2SM_RRCStats_UEStats_drb_active(ue_stats);
      for(int j=0;j<8;j++){
        ret.ue_stats[i].DRB_active[j] = flatbuffers_int8_vec_at(drb_active, j);
      }

      ret.ue_stats[i].StatusRrc = E2SM_RRCStats_UEStats_status_rrc(ue_stats);
      ret.ue_stats[i].setup_pdu_session = E2SM_RRCStats_UEStats_setup_pdu_session(ue_stats);
      ret.ue_stats[i].nb_of_pdusessions = E2SM_RRCStats_UEStats_nb_of_pdu_sessions(ue_stats);
      ret.ue_stats[i].ue_rrc_inactivity_timer = E2SM_RRCStats_UEStats_ue_rrc_inactivity_timer(ue_stats);
    }
  }

  return ret;
}

rrc_call_proc_id_t rrc_dec_call_proc_id_fb(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
  rrc_call_proc_id_t avoid_warning;
  return avoid_warning;
}

rrc_ctrl_hdr_t rrc_dec_ctrl_hdr_fb(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(ctrl_hdr != NULL);
  assert(len > 0);

  int rc = E2SM_RRCStats_ControlHeader_verify_as_root(ctrl_hdr, len);
  assert(rc == 0 && "RRC_SM: Invalid Control Header.");

  E2SM_RRCStats_ControlHeader_table_t ctrl_hdr_fb = E2SM_RRCStats_ControlHeader_as_root(ctrl_hdr);

  if(E2SM_RRCStats_ControlHeader_hdr_type(ctrl_hdr_fb) != E2SM_RRCStats_ControlHeaderUnion_ctrlHeaderF1){
    assert(0!=0 && "RRC_SM: Invalid Control Header Format");
  }
  E2SM_RRCStats_ControlHeaderFormat1_table_t hdr = E2SM_RRCStats_ControlHeader_hdr(ctrl_hdr_fb);

  rrc_ctrl_hdr_t ret = {0};
  ret.dummy = E2SM_RRCStats_ControlHeaderFormat1_param(hdr);
  return ret;
}

rrc_ctrl_msg_t rrc_dec_ctrl_msg_fb(size_t len, uint8_t const ctrl_msg[len])
{
  assert(ctrl_msg != NULL);
  assert(len > 0);

  int rc = E2SM_RRCStats_ControlMessage_verify_as_root(ctrl_msg, len);
  assert(rc == 0 && "RRC_SM: Invalid Message Header.");

  E2SM_RRCStats_ControlMessage_table_t ctrl_msg_fb = E2SM_RRCStats_ControlMessage_as_root(ctrl_msg);

  if(E2SM_RRCStats_ControlMessage_msg_type(ctrl_msg_fb) != E2SM_RRCStats_ControlMessageUnion_ctrlMessageF1){
    assert(0!=0 && "RRC_SM: Invalid Control Message Format");
  }
  E2SM_RRCStats_ControlMessageFormat1_table_t msg = E2SM_RRCStats_ControlMessage_msg(ctrl_msg_fb);

  rrc_ctrl_msg_t ret = {0};
  ret.action = E2SM_RRCStats_ControlMessageFormat1_action(msg);
  return ret;
}

rrc_ctrl_out_t rrc_dec_ctrl_out_fb(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(len > 0);
  assert(ctrl_out != NULL);

  int rc = E2SM_RRCStats_ControlOutcome_verify_as_root(ctrl_out, len);
  assert(rc == 0 && "RRC_SM: Invalid Control Outcome.");

  E2SM_RRCStats_ControlOutcome_table_t ctrl_out_fb = E2SM_RRCStats_ControlOutcome_as_root(ctrl_out);

  assert(ctrl_out_fb != NULL);

  const uint8_t ans_val = E2SM_RRCStats_ControlOutcome_ans(ctrl_out_fb);

  rrc_ctrl_out_t ret = {0};
  switch (ans_val)
  {
  case E2SM_RRCStats_ControlOutcomeResponse_RRC_CTRL_OUT_OK:
    ret.ans = RRC_CTRL_OUT_OK;
    break;
  case E2SM_RRCStats_ControlOutcomeResponse_RRC_CTRL_OUT_END:
    ret.ans = RRC_CTRL_OUT_END;
    break;
  default:
    assert(0!=0 && "RRC_SM: Invalid Control Outcome Ans");
    break;
  }
  return ret;
}

rrc_func_def_t rrc_dec_func_def_fb(size_t len, uint8_t const func[len])
{
  assert(0!=0 && "Not implemented");
  assert(func != NULL);
  rrc_func_def_t  avoid_warning;
  return avoid_warning;
}

