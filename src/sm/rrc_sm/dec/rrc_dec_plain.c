#include "rrc_dec_plain.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

rrc_event_trigger_t rrc_dec_event_trigger_plain(size_t len, uint8_t const ev_tr[len])
{
  rrc_event_trigger_t ev = {0};
  memcpy(&ev.ms, ev_tr, sizeof(ev.ms));
  return ev;
}

rrc_action_def_t rrc_dec_action_def_plain(size_t len, uint8_t const action_def[len])
{
  assert(0!=0 && "Not implemented");
  assert(action_def != NULL);
  rrc_action_def_t act_def;// = {0};
  return act_def;
}

rrc_ind_hdr_t rrc_dec_ind_hdr_plain(size_t len, uint8_t const ind_hdr[len])
{
  assert(len == sizeof(rrc_ind_hdr_t)); 
  rrc_ind_hdr_t ret;
  memcpy(&ret, ind_hdr, len);
  return ret;
}

rrc_ind_msg_t rrc_dec_ind_msg_plain(size_t len, uint8_t const ind_msg[len])
{
  assert(len >= sizeof(uint32_t) + sizeof(int64_t));
//      uint16_t) + sizeof(uint8_t)); // rrc_ind_msg_t)); 
  rrc_ind_msg_t ret;

  void* it = (void*)ind_msg; 
  memcpy(&ret.len_ue_stats, ind_msg, sizeof(ret.len_ue_stats)); 
  it += sizeof(ret.len_ue_stats);

  if(ret.len_ue_stats > 0){
    ret.ue_stats = calloc(ret.len_ue_stats, sizeof(rrc_ue_stats_impl_t));
    assert(ret.ue_stats != NULL && "Memory exhausted!");
  }

  for(uint32_t i = 0; i < ret.len_ue_stats; ++i){
    memcpy(&ret.ue_stats[i] , it, sizeof(rrc_ue_stats_impl_t));
    it += sizeof(rrc_ue_stats_impl_t);
  }

  memcpy(&ret.tstamp, it, sizeof(ret.tstamp));
  it += sizeof(ret.tstamp);

//  memcpy(&ret.slot, it, sizeof(ret.slot));
//  it += sizeof(ret.slot);

  assert(it == ind_msg + len && "Data layout mismatch");

  return ret;
}

rrc_call_proc_id_t rrc_dec_call_proc_id_plain(size_t len, uint8_t const call_proc_id[len])
{
  assert(0!=0 && "Not implemented");
  assert(call_proc_id != NULL);
}

rrc_ctrl_hdr_t rrc_dec_ctrl_hdr_plain(size_t len, uint8_t const ctrl_hdr[len])
{
  assert(len == sizeof(rrc_ctrl_hdr_t)); 
  rrc_ctrl_hdr_t ret;
  memcpy(&ret, ctrl_hdr, len);
  return ret;
}

rrc_ctrl_msg_t rrc_dec_ctrl_msg_plain(size_t len, uint8_t const ctrl_msg[len])
{
  assert(len == sizeof(rrc_ctrl_msg_t)); 
  rrc_ctrl_msg_t ret;
  memcpy(&ret, ctrl_msg, len);
  return ret;
}

rrc_ctrl_out_t rrc_dec_ctrl_out_plain(size_t len, uint8_t const ctrl_out[len]) 
{
  assert(len > 0);
  assert(ctrl_out!= NULL);
  assert(len == sizeof( rrc_ctrl_out_t ) );
  rrc_ctrl_out_t out = {0}; 
  memcpy(&out, ctrl_out, len);

  return out;
}

rrc_func_def_t rrc_dec_func_def_plain(size_t len, uint8_t const func[len])
{
  assert(0!=0 && "Not implemented");
  assert(func != NULL);
}