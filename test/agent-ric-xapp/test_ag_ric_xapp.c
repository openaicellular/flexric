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

#include "../../src/agent/e2_agent_api.h"
#include "../../src/ric/near_ric_api.h"
#include "../../src/xApp/e42_xapp_api.h"
#include "../../src/sm/slice_sm/slice_sm_id.h"
#include "../../src/sm/gtp_sm/gtp_sm_id.h"
#include "../../src/sm/kpm_sm_v2.02/kpm_sm_id.h"
#include "../../src/util/alg_ds/alg/defer.h"
#include "../../src/util/time_now_us.h"
#include "../sm/common/fill_ind_data.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static
void read_RAN(sm_ag_if_rd_t* data)
{
  assert(data->type == MAC_STATS_V0 || 
        data->type == RLC_STATS_V0 ||  
        data->type == PDCP_STATS_V0 || 
        data->type == SLICE_STATS_V0 || 
        data->type == KPM_STATS_V0 ||
        data->type == GTP_STATS_V0);

  if(data->type == MAC_STATS_V0 ){
    fill_mac_ind_data(&data->mac_stats);
  } else if(data->type == RLC_STATS_V0) {
    fill_rlc_ind_data(&data->rlc_stats);
  } else if (data->type == PDCP_STATS_V0 ){
    fill_pdcp_ind_data(&data->pdcp_stats);
  } else if(data->type == SLICE_STATS_V0 ){
    fill_slice_ind_data(&data->slice_stats);
  } else if(data->type == GTP_STATS_V0 ){
    fill_gtp_ind_data(&data->gtp_stats);
  } else if(data->type == KPM_STATS_V0 ){
    fill_kpm_ind_data(&data->kpm_stats);
  } else {
    assert("Invalid data type");
  }
}

static
sm_ag_if_ans_t write_RAN(sm_ag_if_wr_t const* data)
{
  assert(data != NULL);
  if(data->type == MAC_CTRL_REQ_V0){
    //printf("Control message called in the RAN \n");
    sm_ag_if_ans_t ans = {.type = MAC_AGENT_IF_CTRL_ANS_V0};
    ans.mac.ans = MAC_CTRL_OUT_OK;
    return ans;
  } else if(data->type == SLICE_CTRL_REQ_V0 ){

    slice_ctrl_req_data_t const* slice_req_ctrl = &data->slice_req_ctrl;
    slice_ctrl_msg_t const* msg = &slice_req_ctrl->msg;

    if(msg->type == SLICE_CTRL_SM_V0_ADD){
        printf("[E2 Agent]: SLICE CONTROL ADD rx\n");
    } else if (msg->type == SLICE_CTRL_SM_V0_DEL){
        printf("[E2 Agent]: SLICE CONTROL DEL rx\n");
    } else if (msg->type == SLICE_CTRL_SM_V0_UE_SLICE_ASSOC){
        printf("[E2 Agent]: SLICE CONTROL ASSOC rx\n");
    } else {
      assert(0!=0 && "Unknown msg_type!");
    }

    sm_ag_if_ans_t ans = {.type =  SLICE_AGENT_IF_CTRL_ANS_V0};
    return ans;
  } else {
    assert(0 != 0 && "Not supported function ");
  }
  sm_ag_if_ans_t ans = {0};
  return ans;
}


static
void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == KPM_STATS_V0); 

  int64_t now = time_now_us();
  
  // Note that KPM has 1 second resolution in its indication header, while `now` is in microseconds. 
  // Only reasonable latency value to print is a rounded one to seconds.
  printf("KPM ind_msg latency > %ld s\n", now/1000000 - (int64_t)rd->kpm_stats.hdr.collectStartTime);
}


static
void sm_cb_mac(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == MAC_STATS_V0); 

  int64_t now = time_now_us();
  printf("MAC ind_msg latency = %ld μs\n", now - rd->mac_stats.msg.tstamp);
}

static
void sm_cb_rlc(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == RLC_STATS_V0); 

  int64_t now = time_now_us();

  printf("RLC ind_msg latency = %ld μs\n", now - rd->rlc_stats.msg.tstamp);
}

static
void sm_cb_gtp(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == GTP_STATS_V0); 

  int64_t now = time_now_us();
  printf("GTP ind_msg latency = %ld μs\n", now - rd->gtp_stats.msg.tstamp);
}

static
sm_ag_if_wr_t create_add_slice(void)
{
  sm_ag_if_wr_t ctrl_msg = {.type = SLICE_CTRL_REQ_V0 };
  ctrl_msg.slice_req_ctrl.hdr.dummy = 2;
 
  slice_ctrl_msg_t* sl_ctrl_msg = &ctrl_msg.slice_req_ctrl.msg;
  sl_ctrl_msg->type = SLICE_CTRL_SM_V0_ADD;
  char sched_name[] = "My scheduler";
  size_t const sz = strlen(sched_name);
  sl_ctrl_msg->u.add_mod_slice.dl.len_sched_name = sz;
  sl_ctrl_msg->u.add_mod_slice.dl.sched_name = calloc(1,sz+1);
  memcpy(sl_ctrl_msg->u.add_mod_slice.dl.sched_name, sched_name, sz);

  sl_ctrl_msg->u.add_mod_slice.dl.len_slices = 1;
  sl_ctrl_msg->u.add_mod_slice.dl.slices = calloc(1, sizeof(fr_slice_t) );
  sl_ctrl_msg->u.add_mod_slice.dl.slices[0].id = 37;
  sl_ctrl_msg->u.add_mod_slice.dl.slices[0].params.type = SLICE_ALG_SM_V0_STATIC;
  sl_ctrl_msg->u.add_mod_slice.dl.slices[0].params.u.sta.pos_high = 10;
  sl_ctrl_msg->u.add_mod_slice.dl.slices[0].params.u.sta.pos_low = 0;
 
  return ctrl_msg; 

}

static
sm_ag_if_wr_t create_assoc_slice(void)
{
  sm_ag_if_wr_t ctrl_msg = { .type = SLICE_CTRL_REQ_V0 };
  ctrl_msg.slice_req_ctrl.hdr.dummy = 2;
 
  slice_ctrl_msg_t* sl_ctrl_msg = &ctrl_msg.slice_req_ctrl.msg;
  sl_ctrl_msg->type = SLICE_CTRL_SM_V0_UE_SLICE_ASSOC;

  ue_slice_conf_t* ue_slice = &sl_ctrl_msg->u.ue_slice;

  ue_slice->len_ue_slice = 2;
  ue_slice->ues = calloc(2, sizeof(ue_slice_assoc_t));
  for(size_t i = 0; i < 2; ++i){
    ue_slice_assoc_t* assoc = &ue_slice->ues[i];
    assoc->dl_id = 42;
    assoc->ul_id = 42;
    assoc->rnti = 121;
  }
  return ctrl_msg;
}

int main(int argc, char *argv[])
{
  // Init the Agent
  const int mcc = 208; 
  const int mnc = 92; 
  const int mnc_digit_len = 2;
  const int nb_id = 42;
  const int cu_du_id = 0;
  ngran_node_t ran_type = ngran_gNB;
  sm_io_ag_t io = {.read = read_RAN, .write = write_RAN};

  fr_args_t args = init_fr_args(argc, argv);  // Parse arguments
  
  init_agent_api( mcc, mnc, mnc_digit_len, nb_id, cu_du_id, ran_type, io, &args);
  sleep(1);

  // Init the RIC
  init_near_ric_api(&args);
  sleep(2);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });

  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len );

  e2_node_connected_t* n = &nodes.n[0];
  for(size_t i = 0; i < n->len_rf; ++i)
    printf("Registered ran func id = %d \n ", n->ack_rf[i].id );

//  inter_xapp_e i = ms_1;
//  // returns a handle for KPM
//  sm_ans_xapp_t h = report_sm_xapp_api(&nodes.n[0].id, SM_KPM_ID, i, sm_cb_kpm);
//  assert(h.success == true);
//  sleep(1);

  inter_xapp_e i_1 = ms_1;
  // returns a handle
  sm_ans_xapp_t h_1 = report_sm_xapp_api(&nodes.n[0].id, n->ack_rf[0].id, i_1, sm_cb_mac);
  assert(h_1.success == true);
  sleep(2);
//
//  inter_xapp_e i_2 = ms_1;
//  // returns a handle
//  sm_ans_xapp_t h_2 = report_sm_xapp_api(&nodes.n[0].id, n->ack_rf[1].id, i_2, sm_cb_rlc);
//  assert(h_2.success == true);
//  sleep(2);
//
//  inter_xapp_e i_3 = ms_1;
//  // returns a handle
//  sm_ans_xapp_t h_3 = report_sm_xapp_api(&nodes.n[0].id, SM_GTP_ID, i_3, sm_cb_gtp);
//  assert(h_3.success == true);
//  sleep(2);
//
//  // Control ADD slice
//  sm_ag_if_wr_t ctrl_msg_add = create_add_slice();
//  control_sm_xapp_api(&nodes.n[0].id, SM_SLICE_ID, &ctrl_msg_add);
//  free(ctrl_msg_add.slice_req_ctrl.msg.u.add_mod_slice.dl.slices);
//  free(ctrl_msg_add.slice_req_ctrl.msg.u.add_mod_slice.dl.sched_name);
//
//  sleep(1);
//
//  // Control ASSOC slice
//  sm_ag_if_wr_t ctrl_msg_assoc = create_assoc_slice();
//  control_sm_xapp_api(&nodes.n[0].id, SM_SLICE_ID, &ctrl_msg_assoc);
//  free(ctrl_msg_assoc.slice_req_ctrl.msg.u.ue_slice.ues);
//
//  sleep(1);
//
//  // Remove the handle previously returned
//  rm_report_sm_xapp_api(h.u.handle);

  // Remove the handle previously returned
  rm_report_sm_xapp_api(h_1.u.handle);
//
//  // Remove the handle previously returned
//  rm_report_sm_xapp_api(h_2.u.handle);
//
//  // Remove the handle previously returned
//  rm_report_sm_xapp_api(h_3.u.handle);

  sleep(1);

  // Stop the Agent
  stop_agent_api();

  // Stop the RIC
  stop_near_ric_api();

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);     

  printf("Test communicating E2-Agent, Near-RIC and xApp run SUCCESSFULLY\n");
}

