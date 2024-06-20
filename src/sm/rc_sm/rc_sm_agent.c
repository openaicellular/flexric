/*
 * Licensed to the OpenAirInterface (OAI) Software Alliance under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The OpenAirInterface Software Alliance licenses this file to You under
 * the OAI Public License, Version 1.1  (the "License"); you may not use this file
 * except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.openairinterface.org/?page_id=698
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *-------------------------------------------------------------------------------
 * For more information about the OpenAirInterface (OAI) Software Alliance:
 *      contact@openairinterface.org
 */

#include "rc_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "rc_sm_id.h"
#include "enc/rc_enc_generic.h"
#include "dec/rc_dec_generic.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef ASN
  rc_enc_asn_t enc;
#elif FLATBUFFERS 
  rc_enc_fb_t enc;
#elif PLAIN
  rc_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_rc_agent_t;

// 7.4.6
// REPORT Service Style 5: On Demand Report
static
bool on_demand_service_style(e2sm_rc_event_trigger_t et)
{
  return et.format == FORMAT_5_E2SM_RC_EV_TRIGGER_FORMAT ;
}

// Function pointers provided by the RAN for the 
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 

static
exp_ind_data_t rc_enc_ind(sm_rc_agent_t const* sm, rc_ind_data_t const* ind)
{
  exp_ind_data_t ret = {.has_value = true};
  // Fill Indication Header
  byte_array_t ba_hdr = rc_enc_ind_hdr(&sm->enc, &ind->hdr);
  //  assert(ba_hdr.len < 1024 && "Are you really encoding so much info?" );
  ret.data.ind_hdr = ba_hdr.buf;
  ret.data.len_hdr = ba_hdr.len;

  // Fill Indication Message
  byte_array_t ba_msg = rc_enc_ind_msg(&sm->enc, &ind->msg);
  // assert(ba_msg.len < 10*1024 && "Are you really encoding so much info?" );
  ret.data.ind_msg = ba_msg.buf;
  ret.data.len_msg = ba_msg.len;

  // we do not have Call Process ID
  ret.data.call_process_id = NULL;
  ret.data.len_cpid = 0;

  return ret;
}

static
exp_ind_data_t on_indication_per_rc_sm_ag(sm_rc_agent_t* sm, e2sm_rc_action_def_t const* act_def)
{
  rc_rd_ind_data_t rc = {.act_def = act_def};
  defer({free_rc_ind_data(&rc.ind); });

  bool const success = sm->base.io.read_ind(&rc);
  if (success == false)
    return (exp_ind_data_t){.has_value = false};

  exp_ind_data_t ret = rc_enc_ind(sm, &rc.ind);
  assert(ret.data.ind_hdr != NULL);

  return ret; 
}

static
exp_ind_data_t on_indication_aper_rc_sm_ag(sm_rc_agent_t* sm, rc_ind_data_t* ind_data)
{
  exp_ind_data_t ret = rc_enc_ind(sm, ind_data);

  free_rc_ind_data(ind_data);
  free(ind_data);

  assert(ret.data.ind_hdr != NULL);
  return ret;
}

static
exp_ind_data_t on_indication_rc_sm_ag(sm_agent_t const* sm_agent, on_ind_t on_ind)
{
  assert(sm_agent != NULL);
  assert(on_ind.type == PERIODIC_ON_INDICATION_EVENT || on_ind.type == APERIODIC_ON_INDICATION_EVENT);
  
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;

  if(on_ind.type == PERIODIC_ON_INDICATION_EVENT)
    return on_indication_per_rc_sm_ag(sm, on_ind.act_def);
 
  // APERIODIC_ON_INDICATION_EVENT 
  return on_indication_aper_rc_sm_ag(sm, on_ind.ind_data);
}

static
sm_ag_if_ans_subs_t on_subscription_rc_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;
  assert(data != NULL);

  //sm_ag_if_wr_t wr = {.type = SUBSCRIPTION_SM_AG_IF_WR};
  //wr.subs.type = RAN_CTRL_SUBS_V1_03;
  //wr.subs.wr_rc.ric_req_id = data->ric_req_id; 

  wr_rc_sub_data_t wr_rc = {0};
  wr_rc.ric_req_id = data->ric_req_id; 

  wr_rc.rc.et = rc_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);
  defer({ free_e2sm_rc_event_trigger(&wr_rc.rc.et); });

  // Only 1 supported
  wr_rc.rc.sz_ad = 1;
  wr_rc.rc.ad = calloc(wr_rc.rc.sz_ad, sizeof(e2sm_rc_action_def_t));
  assert(wr_rc.rc.ad != NULL && "Memory exhausted");
  defer({ free_e2sm_rc_action_def(wr_rc.rc.ad); free(wr_rc.rc.ad); });

  wr_rc.rc.ad[0] = rc_dec_action_def(&sm->enc, data->len_ad, data->action_def);

  if(on_demand_service_style(wr_rc.rc.et)){
    sm_ag_if_ans_subs_t ans = {.type = ON_DEMAND_REPORT_RC_SM_FLRC};
    on_ind_t on_ind = {.type = PERIODIC_ON_INDICATION_EVENT, .act_def = wr_rc.rc.ad};
    ans.rc_ind = on_indication_rc_sm_ag(sm_agent, on_ind);
    return ans;
  }

  sm_ag_if_ans_t subs = sm->base.io.write_subs(&wr_rc);
  assert(subs.type == SUBS_OUTCOME_SM_AG_IF_ANS_V0);
  assert(subs.subs_out.type == APERIODIC_SUBSCRIPTION_FLRC && "Only aperiodic events supported for RC currently");
  return subs.subs_out;
}

static
sm_ctrl_out_data_t on_control_rc_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;

  //sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
  //wr.ctrl.type = RAN_CONTROL_CTRL_V1_03;

//  rc_ctrl_req_data_t* rc_ctrl = &wr.ctrl.rc_ctrl;

  rc_ctrl_req_data_t rc_ctrl = {0};
  rc_ctrl.hdr = rc_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  rc_ctrl.msg = rc_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_rc_ctrl_req_data(&rc_ctrl); });

  sm_ag_if_ans_t ret = sm->base.io.write_ctrl(&rc_ctrl);
  assert(ret.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  assert(ret.ctrl_out.type == RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0);
  defer({ free_e2sm_rc_ctrl_out(&ret.ctrl_out.rc); });

  // Answer from the E2 Node
  byte_array_t ba = rc_enc_ctrl_out(&sm->enc, &ret.ctrl_out.rc);
  sm_ctrl_out_data_t ans = {.ctrl_out = ba.buf, .len_out = ba.len};

  //printf("on_control called \n");
  return ans;
}

static
sm_e2_setup_data_t on_e2_setup_rc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  //printf("on_e2_setup called \n");
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;

  // Call the RAN and fill the data  
//  sm_ag_if_rd_t rd = {.type = E2_SETUP_AGENT_IF_ANS_V0};
//  rd.e2ap.type = RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0;

  rc_e2_setup_t rc = {0};
  // Will call the function read_rc_setup_sm 
  sm->base.io.read_setup(&rc);

  e2sm_rc_func_def_t* ran_func = &rc.ran_func_def; 
  defer({ free_e2sm_rc_func_def(ran_func); });

  byte_array_t ba = rc_enc_func_def(&sm->enc, ran_func);

  sm_e2_setup_data_t setup = {0}; 
  setup.len_rfd = ba.len;
  setup.ran_fun_def = ba.buf;
/*
  // RAN Function
  setup.rf.definition = cp_str_to_ba(SM_RAN_CTRL_SHORT_NAME);
  setup.rf.id = SM_RC_ID;
  setup.rf.rev = SM_RC_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_RAN_CTRL_OID);
*/
  return setup;
}

static
sm_ric_service_update_data_t on_ric_service_update_rc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_rc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;
  free(sm);
}


// General SM information

// Definition
static
char const* def_rc_sm_ag(void)
{
  return SM_RAN_CTRL_SHORT_NAME;
}

// ID
static
uint16_t id_rc_sm_ag(void)
{
  return   SM_RC_ID; 
}

  // Revision
static
uint16_t rev_rc_sm_ag (void)
{
  return SM_RC_REV;
}

// OID
static
char const* oid_rc_sm_ag (void)
{
  return SM_RAN_CTRL_OID;
}



sm_agent_t* make_rc_sm_agent(sm_io_ag_ran_t io)
{
  sm_rc_agent_t* sm = calloc(1, sizeof(sm_rc_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  //*(uint16_t*)(&sm->base.ran_func_id) = SM_RC_ID; 

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[RAN_CTRL_STATS_V1_03];
  sm->base.io.read_setup = io.read_setup_tbl[RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[RAN_CONTROL_CTRL_V1_03];
  sm->base.io.write_subs = io.write_subs_tbl[RAN_CTRL_SUBS_V1_03];

  sm->base.free_sm = free_rc_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_rc_sm_ag;

  sm->base.proc.on_subscription = on_subscription_rc_sm_ag;
  sm->base.proc.on_indication = on_indication_rc_sm_ag;
  sm->base.proc.on_control = on_control_rc_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_rc_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_rc_sm_ag;
  sm->base.handle = NULL;

  // General SM information
  sm->base.info.def = def_rc_sm_ag;
  sm->base.info.id =  id_rc_sm_ag;
  sm->base.info.rev = rev_rc_sm_ag;
  sm->base.info.oid = oid_rc_sm_ag;


//  assert(strlen(SM_RAN_CTRL_SHORT_NAME) < sizeof( sm->base.ran_func_name) );
//  memcpy(sm->base.ran_func_name, SM_RAN_CTRL_SHORT_NAME, strlen(SM_RAN_CTRL_SHORT_NAME)); 

  return &sm->base;
}
/*
uint16_t id_rc_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_rc_agent_t* sm = (sm_rc_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}
*/

