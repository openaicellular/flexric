#include "rrc_enc_plain.h"

#include <assert.h>
#include <stdlib.h>

#include <assert.h>

byte_array_t rrc_enc_event_trigger_plain(rrc_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  byte_array_t  ba = {0};
 
  ba.len = sizeof(event_trigger->ms);
  ba.buf = malloc(ba.len);
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &event_trigger->ms, ba.len);

  return ba;
}

byte_array_t rrc_enc_action_def_plain(rrc_action_def_t const* action_def)
{
  assert(0!=0 && "Not implemented");

  assert(action_def != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t rrc_enc_ind_hdr_plain(rrc_ind_hdr_t const* ind_hdr)
{
  assert(ind_hdr != NULL);

  byte_array_t ba = {0};

  ba.len = sizeof(rrc_ind_hdr_t);
  ba.buf = malloc(sizeof(rrc_ind_hdr_t));
  assert(ba.buf != NULL && "memory exhausted");
  memcpy(ba.buf, ind_hdr, sizeof(rrc_ind_hdr_t));

  return ba;
}

byte_array_t rrc_enc_ind_msg_plain(rrc_ind_msg_t const* ind_msg)
{
  assert(ind_msg != NULL);

  byte_array_t ba = {0};

  size_t const sz = sizeof(ind_msg->len_ue_stats) + 
                    sizeof(ind_msg->tstamp) +
                    sizeof(rrc_ue_stats_impl_t )*ind_msg->len_ue_stats;

  ba.buf = malloc(sz);
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, &ind_msg->len_ue_stats, sizeof(ind_msg->len_ue_stats) );

  void* it = ba.buf + sizeof(ind_msg->len_ue_stats);

  for(uint32_t i = 0 ; i < ind_msg->len_ue_stats; ++i){
    memcpy(it, &ind_msg->ue_stats[i], sizeof(rrc_ue_stats_impl_t));
    it += sizeof(rrc_ue_stats_impl_t);
  }

  memcpy(it, &ind_msg->tstamp, sizeof(ind_msg->tstamp));
  it += sizeof(ind_msg->tstamp);
  assert(it == ba.buf + sz && "Mismatch of data layout" );

  ba.len = sz;
  return ba;
}


byte_array_t rrc_enc_call_proc_id_plain(rrc_call_proc_id_t const* call_proc_id)
{
  assert(0!=0 && "Not implemented");

  assert(call_proc_id != NULL);
  byte_array_t  ba = {0};
  return ba;
}

byte_array_t rrc_enc_ctrl_hdr_plain(rrc_ctrl_hdr_t const* ctrl_hdr)
{
  assert(ctrl_hdr != NULL);
  byte_array_t  ba = {0};
  ba.buf = malloc(sizeof(rrc_ind_msg_t)); 
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, ctrl_hdr, sizeof(rrc_ctrl_hdr_t));

  ba.len = sizeof(rrc_ctrl_hdr_t);
  return ba;
}

byte_array_t rrc_enc_ctrl_msg_plain(rrc_ctrl_msg_t const* ctrl_msg)
{
  assert(ctrl_msg != NULL);

  byte_array_t  ba = {0};
  ba.buf = malloc(sizeof(rrc_ctrl_msg_t)); 
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, ctrl_msg, sizeof(rrc_ctrl_msg_t));

  ba.len = sizeof(rrc_ctrl_hdr_t);
  return ba;
}

byte_array_t rrc_enc_ctrl_out_plain(rrc_ctrl_out_t const* ctrl_out) 
{
  assert(ctrl_out != NULL );
  byte_array_t  ba = {0};
  ba.buf = malloc(sizeof(rrc_ctrl_out_t  ) );
  assert(ba.buf != NULL && "Memory exhausted");

  memcpy(ba.buf, ctrl_out, sizeof(rrc_ctrl_out_t));
  ba.len = sizeof(rrc_ctrl_out_t);

  return ba;
}

byte_array_t rrc_enc_func_def_plain(rrc_func_def_t const* func)
{
  assert(0!=0 && "Not implemented");

  assert(func!= NULL);
  byte_array_t  ba = {0};
  return ba;
}

