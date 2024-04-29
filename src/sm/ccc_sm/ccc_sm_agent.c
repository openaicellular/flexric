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

#include "ccc_sm_agent.h"

#include "../../util/alg_ds/alg/defer.h"
#include "ccc_sm_id.h"
#include "enc/ccc_enc_generic.h"
#include "dec/ccc_dec_generic.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{

  sm_agent_t base;

#ifdef JSON
  ccc_enc_json_t enc;
#elif PLAIN
  ccc_enc_plain_t enc;
#else
  static_assert(false, "No encryption type selected");
#endif

} sm_ccc_agent_t;

// Function pointers provided by the RAN for the
// 5 procedures, 
// subscription, indication, control, 
// E2 Setup and RIC Service Update. 
//
static
sm_ag_if_ans_subs_t on_subscription_ccc_sm_ag(sm_agent_t const* sm_agent, const sm_subs_data_t* data)
{
  assert(sm_agent != NULL);
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;
  assert(data != NULL);

  sm_ag_if_ans_subs_t ans = {0};

  e2sm_ccc_event_trigger_t event_trigger = ccc_dec_event_trigger(&sm->enc, data->len_et, data->event_trigger);
  defer({free_e2sm_ccc_event_trigger(&event_trigger);});

  if (event_trigger.format == FORMAT_3_E2SM_CCC_EV_TRIGGER_FORMAT){
    // Periodic
    assert(event_trigger.frmt_3.period > 0);
    subscribe_timer_t timer = {
        .type = CCC_V3_0_SUB_DATA_ENUM,
        .ms = event_trigger.frmt_3.period
    };
    // Only 1 supported
    e2sm_ccc_action_def_t* tmp = calloc(1, sizeof(e2sm_ccc_action_def_t));
    assert(tmp != NULL && "Memory exhausted");
    *tmp = ccc_dec_action_def(&sm->enc, data->len_ad, data->action_def);

    timer.act_def = tmp;

    ans.type = PERIODIC_SUBSCRIPTION_FLRC;
    ans.per.t = timer;
  } else {
    // Aperiodic
    wr_ccc_sub_data_t wr_ccc = {0};
    wr_ccc.ric_req_id = data->ric_req_id;
    wr_ccc.ccc.et = event_trigger;

    // Only 1 supported
    wr_ccc.ccc.sz_ad = 1;
    wr_ccc.ccc.ad = calloc(1, sizeof(e2sm_ccc_action_def_t));
    assert(wr_ccc.ccc.ad != NULL && "Memory exhausted");
    defer({ free_e2sm_ccc_action_def(wr_ccc.ccc.ad); free(wr_ccc.ccc.ad); });
    wr_ccc.ccc.ad[0] = ccc_dec_action_def(&sm->enc, data->len_ad, data->action_def);

    sm_ag_if_ans_t subs = sm->base.io.write_subs(&wr_ccc);
    assert(subs.type == SUBS_OUTCOME_SM_AG_IF_ANS_V0);
    assert(subs.subs_out.type == APERIODIC_SUBSCRIPTION_FLRC);
    ans = subs.subs_out;
  }

  return ans;
}

static
exp_ind_data_t on_indication_ccc_sm_ag(sm_agent_t const* sm_agent, void* act_def_v)
{
//  printf("on_indication CCC called \n");
  assert(sm_agent != NULL);
  assert(act_def_v != NULL && "Indication data needed for this SM");
  
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;

  e2sm_ccc_action_def_t* act_def = act_def_v;

  exp_ind_data_t ret = {.has_value = true};

  ccc_rd_ind_data_t ccc = {0};
  defer({free_ccc_ind_data(&ccc.ind); });

  ccc.act_def = act_def;
  bool const success = sm->base.io.read_ind(&ccc);
  if (success == false)
    return (exp_ind_data_t){.has_value = false};

  // Fill Indication Header
  byte_array_t ba_hdr = ccc_enc_ind_hdr(&sm->enc, &ccc.ind.hdr);
  assert(ba_hdr.len < 1024 && "Are you really encoding so much info?" );
  ret.data.ind_hdr = ba_hdr.buf;
  ret.data.len_hdr = ba_hdr.len;

  // Fill Indication Message
  byte_array_t ba_msg = ccc_enc_ind_msg(&sm->enc, &ccc.ind.msg);
  assert(ba_msg.len < 10*1024 && "Are you really encoding so much info?" );
  ret.data.ind_msg = ba_msg.buf;
  ret.data.len_msg = ba_msg.len;

  // we do not have Call Process ID
  ret.data.call_process_id = NULL;
  ret.data.len_cpid = 0;

  return ret;
}

static
sm_ctrl_out_data_t on_control_ccc_sm_ag(sm_agent_t const* sm_agent, sm_ctrl_req_data_t const* data)
{
  assert(sm_agent != NULL);
  assert(data != NULL);
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*) sm_agent;
  
  ccc_ctrl_req_data_t ccc_ctrl = {0};
  ccc_ctrl.hdr = ccc_dec_ctrl_hdr(&sm->enc, data->len_hdr, data->ctrl_hdr);
  ccc_ctrl.msg = ccc_dec_ctrl_msg(&sm->enc, data->len_msg, data->ctrl_msg);
  defer({ free_ccc_ctrl_req_data(&ccc_ctrl); });

  sm_ag_if_ans_t ret = sm->base.io.write_ctrl(&ccc_ctrl);
  assert(ret.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
  assert(ret.ctrl_out.type == CCC_V3_0_AGENT_IF_CTRL_ANS_V0);
  defer({ free_e2sm_ccc_ctrl_out(&ret.ctrl_out.ccc); });

  // Answer from the E2 Node
  byte_array_t ba = ccc_enc_ctrl_out(&sm->enc, &ret.ctrl_out.ccc);
  sm_ctrl_out_data_t ans = {.ctrl_out = ba.buf, .len_out = ba.len};
  
  return ans;
}

static
sm_e2_setup_data_t on_e2_setup_ccc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);
  
  //printf("on_e2_setup called \n");
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;

  // Call the RAN and fill the data
//  sm_ag_if_rd_t rd = {.type = E2_SETUP_AGENT_IF_ANS_V0};
//  rd.e2ap.type = RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0;

  ccc_e2_setup_t ccc = {0};
  // Will call the function read_ccc_setup_sm
  sm->base.io.read_setup(&ccc);

  e2sm_ccc_func_def_t* ran_func = &ccc.ran_func_def;
  defer({ free_e2sm_ccc_func_def(ran_func); });

  byte_array_t ba = ccc_enc_func_def(&sm->enc, ran_func);

  sm_e2_setup_data_t setup = {0};
  setup.len_rfd = ba.len;
  setup.ran_fun_def = ba.buf;
/*
  // RAN Function
  setup.rf.definition = cp_str_to_ba(SM_RAN_CTRL_SHORT_NAME);
  setup.rf.id = SM_ccc_ID;
  setup.rf.rev = SM_ccc_REV;

  setup.rf.oid = calloc(1, sizeof(byte_array_t) );
  assert(setup.rf.oid != NULL && "Memory exhausted");

  *setup.rf.oid = cp_str_to_ba(SM_RAN_CTRL_OID);
*/
  return setup;
}

static
sm_ric_service_update_data_t on_ric_service_update_ccc_sm_ag(sm_agent_t const* sm_agent)
{
  assert(sm_agent != NULL);

  assert(0!=0 && "Not implemented");

  printf("on_ric_service_update called \n");
  sm_ric_service_update_data_t dst = {0};
  return dst;
}

static
void free_ccc_sm_ag(sm_agent_t* sm_agent)
{
  assert(sm_agent != NULL);
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;
  free(sm);
}


// General SM information

// Definition
static
char const* def_ccc_sm_ag(void)
{
  return SM_CCC_SHORT_NAME;
}

// ID
static
uint16_t id_ccc_sm_ag(void)
{
  return SM_CCC_ID;
}

  // Revision
static
uint16_t rev_ccc_sm_ag (void)
{
  return SM_CCC_REV;
}

// OID
static
char const* oid_ccc_sm_ag (void)
{
  return SM_CCC_OID;
}



sm_agent_t* make_ccc_sm_agent(sm_io_ag_ran_t io)
{
  sm_ccc_agent_t* sm = calloc(1, sizeof(sm_ccc_agent_t));
  assert(sm != NULL && "Memory exhausted!!!");

  //*(uint16_t*)(&sm->base.ran_func_id) = SM_ccc_ID; 

  // Read
  sm->base.io.read_ind = io.read_ind_tbl[CCC_STATS_V3_0];
  sm->base.io.read_setup = io.read_setup_tbl[CCC_V3_0_AGENT_IF_E2_SETUP_ANS_V0];
 
  //Write
  sm->base.io.write_ctrl = io.write_ctrl_tbl[CCC_CTRL_REQ_V3_0];
  sm->base.io.write_subs = io.write_subs_tbl[CCC_SUBS_V3_0];

  sm->base.free_sm = free_ccc_sm_ag;
  sm->base.free_act_def = NULL; //free_act_def_ccc_sm_ag;

  sm->base.proc.on_subscription = on_subscription_ccc_sm_ag;
  sm->base.proc.on_indication = on_indication_ccc_sm_ag;
  sm->base.proc.on_control = on_control_ccc_sm_ag;
  sm->base.proc.on_ric_service_update = on_ric_service_update_ccc_sm_ag;
  sm->base.proc.on_e2_setup = on_e2_setup_ccc_sm_ag;
  sm->base.handle = NULL;

  // General SM information
  sm->base.info.def = def_ccc_sm_ag;
  sm->base.info.id =  id_ccc_sm_ag;
  sm->base.info.rev = rev_ccc_sm_ag;
  sm->base.info.oid = oid_ccc_sm_ag;


//  assert(strlen(SM_RAN_CTRL_SHORT_NAME) < sizeof( sm->base.ran_func_name) );
//  memcpy(sm->base.ran_func_name, SM_RAN_CTRL_SHORT_NAME, strlen(SM_RAN_CTRL_SHORT_NAME)); 

  return &sm->base;
}
/*
uint16_t id_ccc_sm_agent(sm_agent_t const* sm_agent )
{
  assert(sm_agent != NULL);
  sm_ccc_agent_t* sm = (sm_ccc_agent_t*)sm_agent;
  return sm->base.ran_func_id;
}
*/

