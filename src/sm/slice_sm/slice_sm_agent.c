#include "slice_sm_agent.h"
#include "slice_sm_id.h"
#include "enc/slice_enc_generic.h"
#include "dec/slice_dec_generic.h"
#include "../../util/alg_ds/alg/defer.h"


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  slice_enc_asn_t enc;
#elif FLATBUFFERS 
  slice_enc_fb_t enc;
#elif PLAIN
  slice_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_slice_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
sm_ag_if_ans_subs_t on_subscription_slice_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_slice_agent_t* sm = (sm_slice_agent_t*)sm_agent;
 
  slice_event_trigger_t ev = slice_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  sm_ag_if_ans_subs_t ans = {.type = PERIODIC_SUBSCRIPTION_FLRC};
  ans.per.t.ms = ev.ms;
  return ans;
}


static
exp_ind_data_t on_indication_slice_sm_ag(sm_agent_t const* sm_agent, void* act_def)
{
  //printf("on_indication SLICE called \n");
  assert(sm_agent != NULL);
  assert(act_def == NULL && "Subscription data not needed for this SM");
  sm_slice_agent_t* sm = (sm_slice_agent_t*)sm_agent;

  exp_ind_data_t ret = {.has_value = true};

  // Fill Indication Header
  slice_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = slice_enc_ind_hdr(&sm->enc, &hdr );
  ret.data.ind_hdr = ba_hdr.buf;
  ret.data.len_hdr = ba_hdr.len;

  slice_ind_data_t slice = {0};
// Liberate the memory if previously allocated by the RAN. It sucks
  defer({ free_slice_ind_data(&slice); });

  if(sm->base.io.read_ind(&slice) == false)
    return (exp_ind_data_t){.has_value = false};

  byte_array_t ba = slice_enc_ind_msg(&sm->enc, &slice.msg);
  ret.data.ind_msg = ba.buf;
  ret.data.len_msg = ba.len;

  // Fill Call Process ID
  ret.data.call_process_id = NULL;
  ret.data.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_slice_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_slice_agent_t* sm = (sm_slice_agent_t*) sm_agent;

//  sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
//  wr.ctrl.type = SLICE_CTRL_REQ_V0; 

  slice_ctrl_req_data_t ctrl = {0};
  ctrl.hdr = slice_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  defer({ free_slice_ctrl_hdr(&ctrl.hdr ); });

  ctrl.msg = slice_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_slice_ctrl_msg(&ctrl.msg); });

  sm_ag_if_ans_t ans = sm->base.io.write_ctrl(&ctrl);
  assert(ans.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  assert(ans.ctrl_out.type == SLICE_AGENT_IF_CTRL_ANS_V0);
 

  defer({free_slice_ctrl_out(&ans.ctrl_out.slice); });

  byte_array_t ba = slice_enc_ctrl_out(&sm->enc, &ans.ctrl_out.slice);

  sm_ctrl_out_data_t ret = {0};
  ret.len_out = ba.len;
  ret.ctrl_out = ba.buf;

  return ret;
}

static
sm_e2_setup_data_t on_e2_setup_slice_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
//  printf("on_e2_setup called \n");

  sm_slice_agent_t* sm = (sm_slice_agent_t*)sm_agent;
  (void)sm;

  // ToDo: Call the RAN to fill the RAN Function

  sm_e2_setup_data_t setup = {.len_rfd =0, .ran_fun_def = NULL }; 

  size_t const sz = strnlen(SM_SLICE_STR, 256);
  assert(sz < 256 && "Buffer overeflow?");

  setup.len_rfd = sz;
  setup.ran_fun_def = calloc(1, sz);
  assert(setup.ran_fun_def != NULL);

  memcpy(setup.ran_fun_def, SM_SLICE_STR , sz);
 
/*
  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));
*/

  // RAN Function
//  setup.rf.def = cp_str_to_ba(SM_SLICE_SHORT_NAME);
//  setup.rf.id = SM_SLICE_ID;
//  setup.rf.rev = SM_SLICE_REV;

//  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
//  assert(setup.rf.oid != NULL && "Memory exhausted");

//  *setup.rf.oid = cp_str_to_ba(SM_SLICE_OID);

  return setup;
}

static
sm_ric_service_update_data_t on_ric_service_update_slice_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0 != 0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_slice_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_slice_agent_t* sm = (sm_slice_agent_t*)sm_agent;
  free(sm);
}

// General SM information

// Definition
static
char const* def_slice_sm_ag(void)
{
  return SM_SLICE_STR;
}

// ID
static
uint16_t id_slice_sm_ag(void)
{
  return SM_SLICE_ID; 
}

  // Revision
static
uint16_t rev_slice_sm_ag (void)
{
  return SM_SLICE_REV;
}

// OID
static
char const* oid_slice_sm_ag (void)
{
  return SM_SLICE_OID;
}


sm_agent_t* make_slice_sm_agent(sm_io_ag_ran_t io)
{
  sm_slice_agent_t* sm = calloc(1, sizeof(sm_slice_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

//  *(uint16_t*)(&sm->base.ran_func_id) = SM_SLICE_ID; 

//  sm->base.io = io;

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[SLICE_STATS_V0];
  sm->base.io.read_setup = io.read_setup_tbl[SLICE_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[SLICE_CTRL_REQ_V0];
  sm->base.io.write_subs = io.write_subs_tbl[SLICE_SUBS_V0];

  sm->base.free_sm = free_slice_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_slice_sm_ag;

  sm->base.proc.on_subscription = on_subscription_slice_sm_ag;
  sm->base.proc.on_indication = on_indication_slice_sm_ag;
  sm->base.proc.on_control = on_control_slice_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_slice_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_slice_sm_ag;
  sm->base.handle = NULL;

  // General SM information
  sm->base.info.def = def_slice_sm_ag;
  sm->base.info.id =  id_slice_sm_ag;
  sm->base.info.rev = rev_slice_sm_ag;
  sm->base.info.oid = oid_slice_sm_ag;


//  *(uint16_t*)(&sm->base.ran_func_id) = SM_SLICE_ID; 
//  assert(strlen(SM_SLICE_STR) < sizeof( sm->base.ran_func_name) );
//  memcpy(sm->base.ran_func_name, SM_SLICE_STR, strlen(SM_SLICE_STR)); 

  return &sm->base;
}
/*
uint16_t id_slice_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_slice_agent_t* sm = (sm_slice_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}
*/

