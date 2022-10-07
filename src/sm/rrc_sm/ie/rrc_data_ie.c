#include "rrc_data_ie.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//////////////////////////////////////
// RIC Event Trigger Definition
/////////////////////////////////////

void free_rrc_event_trigger(rrc_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
}

rrc_event_trigger_t cp_rrc_event_trigger( rrc_event_trigger_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 

  rrc_event_trigger_t et = {0};
  return et;
}

bool eq_rrc_event_trigger(rrc_event_trigger_t* m0, rrc_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Action Definition 
/////////////////////////////////////

void free_rrc_action_def(rrc_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

rrc_action_def_t cp_rrc_action_def(rrc_action_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rrc_action_def_t ad = {0};
  return ad;
}

bool eq_rrc_action_def(rrc_event_trigger_t* m0,  rrc_event_trigger_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Indication Header 
/////////////////////////////////////


void free_rrc_ind_hdr(rrc_ind_hdr_t* src)
{
  assert(src != NULL);
  (void)src;
}

rrc_ind_hdr_t cp_rrc_ind_hdr(rrc_ind_hdr_t* src)
{
  assert(src != NULL);
  rrc_ind_hdr_t dst = {0}; 
  dst.dummy = src->dummy;
  return dst;
}

bool eq_rrc_ind_hdr(rrc_ind_hdr_t* m0, rrc_ind_hdr_t* m1)
{
  assert(m0 != 0);
  assert(m1 != 0);

  if(m0->dummy != m1->dummy)
    return false;
  return true;
}


//////////////////////////////////////
// RIC Indication Message 
/////////////////////////////////////


rrc_ue_stats_impl_t cp_rrc_ue_stats_impl(rrc_ue_stats_impl_t const* src)
{
  assert(src != NULL);

  rrc_ue_stats_impl_t dst = { .rnti = src->rnti,
                              .rbid = src->rbid,
                              .DRB_active[0] = src->DRB_active[0],
                              .DRB_active[1] = src->DRB_active[1],
                              .DRB_active[2] = src->DRB_active[2],
                              .DRB_active[3] = src->DRB_active[3],
                              .DRB_active[4] = src->DRB_active[4],
                              .DRB_active[5] = src->DRB_active[5],
                              .DRB_active[6] = src->DRB_active[6],
                              .DRB_active[7] = src->DRB_active[7],
                              .StatusRrc = src->StatusRrc,
                              .setup_pdu_session = src->setup_pdu_session,
                              .nb_of_pdusessions = src->nb_of_pdusessions,
                              .ue_rrc_inactivity_timer = src->ue_rrc_inactivity_timer
                            }; 

  return dst;
}


static
bool check_rrc_rbid(uint8_t rbid)
{
  assert(rbid < 11); // [0,2] for srb and [3,10] for drb
  return true;
}


static
bool check_rrc_invariants(rrc_ind_msg_t* msg)
{
  assert(msg != NULL);

  for(uint32_t i = 0; i < msg->len_ue_stats; ++i){
    rrc_ue_stats_impl_t* rb = &msg->ue_stats[i]; 
    check_rrc_rbid(rb->rbid);
  }

  return true;
}

void free_rrc_ind_msg(rrc_ind_msg_t* msg)
{
  assert(msg != NULL);
  assert(check_rrc_invariants(msg));

  if(msg->len_ue_stats > 0)
    free(msg->ue_stats);
}

rrc_ind_msg_t cp_rrc_ind_msg(rrc_ind_msg_t* src)
{
  assert(src != NULL);
  assert(check_rrc_invariants(src));

  rrc_ind_msg_t dst = {0}; 
  dst.len_ue_stats = src->len_ue_stats;

  if(dst.len_ue_stats > 0){
    dst.ue_stats = calloc(dst.len_ue_stats, sizeof(rrc_ue_stats_impl_t) );
    assert(dst.ue_stats != NULL && "Memory exhausted"); 
  }

  for(size_t i = 0; i < dst.len_ue_stats; ++i){
    dst.ue_stats[i] = cp_rrc_ue_stats_impl(&src->ue_stats[i]); 
  }

  dst.tstamp = src->tstamp; 
  return dst;
}

bool eq_rrc_ind_msg(rrc_ind_msg_t* m0, rrc_ind_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  assert(check_rrc_invariants(m0));
  assert(check_rrc_invariants(m1));

  if(m0->tstamp != m1->tstamp ||
      m0->len_ue_stats != m1->len_ue_stats ){
    assert(0 != 0 && "Impossible!");
    return false;
  }

  for(uint32_t i =0; i < m0->len_ue_stats; ++i){
    rrc_ue_stats_impl_t* rb0 = &m0->ue_stats[i]; 
    rrc_ue_stats_impl_t* rb1 = &m1->ue_stats[i]; 

    if( 
        rb0->rnti !=  rb1->rnti ||
        rb0->rbid != rb1->rbid){

    assert(0 != 0 && "Impossible!");
      return false;
    }
  }
  return true;
} 

//////////////////////////////////////
// RIC Call Process ID 
/////////////////////////////////////

void free_rrc_call_proc_id(rrc_call_proc_id_t* src)
{
  // Note that the src could be NULL
  free(src);
}

rrc_call_proc_id_t cp_rrc_call_proc_id( rrc_call_proc_id_t* src)
{
  assert(src != NULL); 
  rrc_call_proc_id_t dst = {0};

  dst.dummy = src->dummy;

  return dst;
}

bool eq_rrc_call_proc_id(rrc_call_proc_id_t* m0, rrc_call_proc_id_t* m1)
{
  if(m0 == NULL && m1 == NULL)
    return true;
  if(m0 == NULL)
    return false;
  if(m1 == NULL)
    return false;

  if(m0->dummy != m1->dummy)
    return false;

  return true;
}


//////////////////////////////////////
// RIC Control Header 
/////////////////////////////////////

void free_rrc_ctrl_hdr( rrc_ctrl_hdr_t* src)
{
  assert(src != NULL);
  // No memory allocated
  (void)src;
}

rrc_ctrl_hdr_t cp_rrc_ctrl_hdr(rrc_ctrl_hdr_t* src)
{
  assert(src != NULL);
  assert(0!=0 && "Not implemented" ); 
  rrc_ctrl_hdr_t ret = {0};
  return ret;
}

bool eq_rrc_ctrl_hdr(rrc_ctrl_hdr_t* m0, rrc_ctrl_hdr_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RIC Control Message 
/////////////////////////////////////


void free_rrc_ctrl_msg( rrc_ctrl_msg_t* src)
{
  assert(src != NULL);
  // No memory allocated, so nothing to free
  (void)src;
}

rrc_ctrl_msg_t cp_rrc_ctrl_msg(rrc_ctrl_msg_t* src)
{
  assert(src != NULL);

  rrc_ctrl_msg_t ret = {0};

  ret.action = src->action;

  return ret;
}

bool eq_rrc_ctrl_msg(rrc_ctrl_msg_t* m0, rrc_ctrl_msg_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);
  
  if(m0->action == m1->action)
    return true;

  return false;
}


//////////////////////////////////////
// RIC Control Outcome 
/////////////////////////////////////

void free_rrc_ctrl_out(rrc_ctrl_out_t* src)
{
  assert(src != NULL);
  (void)src;
  // No heap memory allocated
}

rrc_ctrl_out_t cp_rrc_ctrl_out(rrc_ctrl_out_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rrc_ctrl_out_t ret = {0}; 
  return ret;
}

bool eq_rrc_ctrl_out(rrc_ctrl_out_t* m0, rrc_ctrl_out_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 

  return true;
}


//////////////////////////////////////
// RAN Function Definition 
/////////////////////////////////////

void free_rrc_func_def( rrc_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
}

rrc_func_def_t cp_rrc_func_def(rrc_func_def_t* src)
{
  assert(src != NULL);

  assert(0!=0 && "Not implemented" ); 
  rrc_func_def_t ret = {0};
  return ret;
}

bool eq_rrc_func_def(rrc_func_def_t* m0, rrc_func_def_t* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  assert(0!=0 && "Not implemented" ); 
  return true;
}

///////////////
// RIC Indication
///////////////

rrc_ind_data_t cp_rrc_ind_data( rrc_ind_data_t const* src)
{
  assert(src != NULL);
  rrc_ind_data_t dst = {0};
  dst.hdr = cp_rrc_ind_hdr(&src->hdr);
  dst.msg = cp_rrc_ind_msg(&src->msg);
  
  if(src->proc_id != NULL){
    dst.proc_id = malloc(sizeof(rrc_call_proc_id_t)); 
    assert(dst.proc_id != NULL && "Memory exhausted");
    *dst.proc_id = cp_rrc_call_proc_id(src->proc_id);
  }

  return dst;
}

void free_rrc_ind_data(rrc_ind_data_t* ind)
{
  assert(ind != NULL);
  free_rrc_ind_hdr(&ind->hdr);
  free_rrc_ind_msg(&ind->msg);
  free_rrc_call_proc_id(ind->proc_id);
}


