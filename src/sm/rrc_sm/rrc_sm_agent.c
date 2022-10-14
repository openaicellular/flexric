#include "rrc_sm_agent.h"
#include "rrc_sm_id.h"
#include "enc/rrc_enc_generic.h"
#include "dec/rrc_dec_generic.h"
#include "../../util/alg_ds/alg/defer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  rrc_enc_asn_t enc;
#elif FLATBUFFERS 
  rrc_enc_fb_t enc;
#elif PLAIN
  rrc_enc_plain_t enc;
#else
  static_assert(false, "No encryptioin type selected");
#endif

} sm_rrc_agent_t;


// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
subscribe_timer_t  on_subscription_rrc_sm_ag(sm_agent_t* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);

  sm_rrc_agent_t* sm = (sm_rrc_agent_t*)sm_agent;
 
  rrc_event_trigger_t ev = rrc_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);

  subscribe_timer_t timer = {.ms = ev.ms };
  return timer;
}

static
sm_ind_data_t on_indication_rrc_sm_ag(sm_agent_t* sm_agent)
{
  //printf("on_indication called \n");

  assert(sm_agent != NULL);
  sm_rrc_agent_t* sm = (sm_rrc_agent_t*)sm_agent;

  sm_ind_data_t ret = {0};

  // Fill Indication Header
  rrc_ind_hdr_t hdr = {.dummy = 0 };
  byte_array_t ba_hdr = rrc_enc_ind_hdr(&sm->enc, &hdr );
  ret.ind_hdr = ba_hdr.buf;
  ret.len_hdr = ba_hdr.len;

  // Fill Indication Message 
  sm_ag_if_rd_t rd_if = {0};
  rd_if.type = RRC_STATS_V0;
  sm->base.io.read(&rd_if);

// Liberate the memory if previously allocated by the RAN. It sucks
  rrc_ind_data_t* ind = &rd_if.rrc_stats;
  defer({ free_rrc_ind_hdr(&ind->hdr) ;});
  defer({ free_rrc_ind_msg(&ind->msg) ;});
  defer({ free_rrc_call_proc_id(ind->proc_id);});

  byte_array_t ba = rrc_enc_ind_msg(&sm->enc, &rd_if.rrc_stats.msg);
  ret.ind_msg = ba.buf;
  ret.len_msg = ba.len;

  // Fill Call Process ID
  ret.call_process_id = NULL;
  ret.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_rrc_sm_ag(sm_agent_t* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_rrc_agent_t* sm = (sm_rrc_agent_t*) sm_agent;

  rrc_ctrl_hdr_t hdr = rrc_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  defer({ free_rrc_ctrl_hdr(&hdr); });
  assert(hdr.dummy == 0 && "Only dummy == 0 supported ");

  rrc_ctrl_msg_t msg = rrc_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_rrc_ctrl_msg(&msg); });
  assert(msg.action == 42 && "Only action number 42 supported");

  sm_ag_if_wr_t wr = {.type = RRC_CTRL_REQ_V0 };
  wr.rrc_ctrl.msg = cp_rrc_ctrl_msg(&msg);

  // Call the RAN
 sm_ag_if_ans_t ans = sm->base.io.write(&wr);
 assert(ans.type == RRC_AGENT_IF_CTRL_ANS_V0);
  defer({ free_rrc_ctrl_out(&ans.rrc); });

  // Encode the answer from the RAN
  byte_array_t ba = rrc_enc_ctrl_out(&sm->enc, &ans.rrc);

  sm_ctrl_out_data_t ret = {0}; 
  ret.len_out = ba.len;
  ret.ctrl_out = ba.buf;


  return ret;
}

static
sm_e2_setup_t on_e2_setup_rrc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  //printf("on_e2_setup called \n");
  sm_rrc_agent_t* sm = (sm_rrc_agent_t*)sm_agent;

  sm_e2_setup_t setup = {.len_rfd =0, .ran_fun_def = NULL  }; 

  setup.len_rfd = strlen(sm->base.ran_func_name);
  setup.ran_fun_def = calloc(1, strlen(sm->base.ran_func_name));
  assert(setup.ran_fun_def != NULL);
  memcpy(setup.ran_fun_def, sm->base.ran_func_name, strlen(sm->base.ran_func_name));

  return setup;
}

static
void on_ric_service_update_rrc_sm_ag(sm_agent_t* sm_agent, sm_ric_service_update_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);


  printf("on_ric_service_update called \n");
}

static
void free_rrc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_rrc_agent_t* sm = (sm_rrc_agent_t*)sm_agent;
  free(sm);
}


sm_agent_t* make_rrc_sm_agent(sm_io_ag_t io)
{
  sm_rrc_agent_t* sm = calloc(1, sizeof(*sm));
  assert(sm != NULL && "Memory exhausted!!!");

  *(uint16_t*)(&sm->base.ran_func_id) = SM_RRC_ID; 

  sm->base.io = io;
  sm->base.free_sm = free_rrc_sm_ag;

  // O-RAN E2SM 5 Procedures
  sm->base.proc.on_subscription = on_subscription_rrc_sm_ag;
  sm->base.proc.on_indication = on_indication_rrc_sm_ag;
  sm->base.proc.on_control = on_control_rrc_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_rrc_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_rrc_sm_ag;

  assert(strlen(SM_RRC_STR) < sizeof( sm->base.ran_func_name) );
  memcpy(sm->base.ran_func_name, SM_RRC_STR, strlen(SM_RRC_STR)); 

  return &sm->base;
}

