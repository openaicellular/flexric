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


#include "../../rnd/fill_rnd_data_mac.h"
#include "../../../src/sm/mac_sm/mac_sm_agent.h"
#include "../../../src/sm/mac_sm/mac_sm_ric.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static
mac_ind_data_t cp;

static
mac_ctrl_req_data_t cp_ctrl;
/////
// AGENT
////

static
bool read_ind_mac(void* read)
{
  assert(read != NULL);
//  assert(read->type == INDICATION_MSG_AGENT_IF_ANS_V0);
//  assert(read->ind.type == MAC_STATS_V0);

  mac_ind_data_t* ind = (mac_ind_data_t*)read; // &read->ind.mac;

  fill_mac_ind_data(ind);
  cp.hdr = cp_mac_ind_hdr(&ind->hdr);
  cp.msg = cp_mac_ind_msg(&ind->msg);
  return true;
}


static
sm_ag_if_ans_t write_ctrl_mac(void const* data)
{
  assert(data != NULL);

  mac_ctrl_req_data_t const* mac_req_ctrl = (mac_ctrl_req_data_t const* )data; // &data->slice_req_ctrl;
  mac_ctrl_msg_t const* msg = &mac_req_ctrl->msg;

  for (size_t i = 0; i < msg->ran_conf_len; i++) {
    // TODO
    printf("ran_conf[%ld].isset_pusch_mcs %d\n", i, msg->ran_conf[i].isset_pusch_mcs);
    printf("ran_conf[%ld].pusch_mcs %d\n", i, msg->ran_conf[i].pusch_mcs);
    printf("ran_conf[%ld].rnti %d\n", i, msg->ran_conf[i].rnti);
  }

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = MAC_AGENT_IF_CTRL_ANS_V0;
  ans.ctrl_out.mac.ans = MAC_CTRL_OUT_OK;
  return ans;
}


/////////////////////////////
// Check Functions
// //////////////////////////

static
void check_eq_ran_function(sm_agent_t const* ag, sm_ric_t const* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);
  assert(ag->info.id() == ric->ran_func_id);
}

// RIC -> E2
static
void check_subscription(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  char sub[] = "2_ms";
  sm_subs_data_t data = ric->proc.on_subscription(ric, &sub);

  sm_ag_if_ans_subs_t const subs = ag->proc.on_subscription(ag, &data); 
  assert(subs.type == PERIODIC_SUBSCRIPTION_FLRC);
  assert(subs.per.t.ms == 2);

  free_sm_subs_data(&data);
}

// E2 -> RIC
static
void check_indication(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  exp_ind_data_t exp = ag->proc.on_indication(ag, NULL);
  assert(exp.has_value == true);
  sm_ag_if_rd_ind_t msg = ric->proc.on_indication(ric, &exp.data);

  assert(msg.type == MAC_STATS_V0);
  mac_ind_data_t* data = &msg.mac;

  assert(eq_mac_ind_hdr(&data->hdr, &cp.hdr) == true);
  assert(eq_mac_ind_msg(&data->msg, &cp.msg) == true);
  assert(eq_mac_call_proc_id(data->proc_id, cp.proc_id) == true);

  free_mac_ind_hdr(&data->hdr);
  free_mac_ind_msg(&data->msg);

  free_exp_ind_data(&exp);
}


// RIC -> E2

static
void check_ctrl(sm_agent_t* ag, sm_ric_t* ric)
{
  assert(ag != NULL);
  assert(ric != NULL);

  //sm_ag_if_wr_t wr = {.type = CONTROL_SM_AG_IF_WR };
  //wr.ctrl.type = SLICE_CTRL_REQ_V0 ;

  sm_ag_if_wr_t ctrl_msg = {0};
  ctrl_msg.type = CONTROL_SM_AG_IF_WR;
  ctrl_msg.ctrl.type = MAC_CTRL_REQ_V0;
  mac_ctrl_req_data_t* mac_req_ctrl = &ctrl_msg.ctrl.mac_ctrl;
  fill_mac_ctrl(mac_req_ctrl);

  cp_ctrl.hdr = cp_mac_ctrl_hdr(&mac_req_ctrl->hdr);
  cp_ctrl.msg = cp_mac_ctrl_msg(&mac_req_ctrl->msg);

  sm_ctrl_req_data_t ctrl_req = ric->proc.on_control_req(ric, &ctrl_msg);

  sm_ctrl_out_data_t out_data = ag->proc.on_control(ag, &ctrl_req);

  sm_ag_if_ans_ctrl_t ans = ric->proc.on_control_out(ric, &out_data);
  assert(ans.type == MAC_AGENT_IF_CTRL_ANS_V0);

  if(ctrl_req.len_hdr > 0)
    free(ctrl_req.ctrl_hdr);

  if(ctrl_req.len_msg > 0)
    free(ctrl_req.ctrl_msg);

  if(out_data.len_out > 0)
    free(out_data.ctrl_out);

  free_mac_ctrl_out(&ans.mac);

  free_mac_ctrl_hdr(&mac_req_ctrl->hdr);
  free_mac_ctrl_msg(&mac_req_ctrl->msg);

  free_mac_ctrl_hdr(&cp_ctrl.hdr);
  free_mac_ctrl_msg(&cp_ctrl.msg);
}

int main()
{
  sm_io_ag_ran_t io_ag = {0};
  io_ag.read_ind_tbl[MAC_STATS_V0] = read_ind_mac;
  io_ag.write_ctrl_tbl[MAC_STATS_V0] = write_ctrl_mac;

  sm_agent_t* sm_ag = make_mac_sm_agent(io_ag);
  sm_ric_t* sm_ric = make_mac_sm_ric();

  for(int i = 0; i < 64*1024; ++i) {
    check_eq_ran_function(sm_ag, sm_ric);
    check_subscription(sm_ag, sm_ric);
    check_indication(sm_ag, sm_ric);
    check_ctrl(sm_ag, sm_ric);
  }

  sm_ag->free_sm(sm_ag);
  sm_ric->free_sm(sm_ric);

  printf("Success\n");
  return EXIT_SUCCESS;
}

