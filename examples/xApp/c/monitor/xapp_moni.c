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

#include "../../../../src/xApp/e42_xapp_api.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/sm/mac_sm/mac_sm_id.h"
#include "../../../../src/sm/rlc_sm/rlc_sm_id.h"
#include "../../../../src/sm/pdcp_sm/pdcp_sm_id.h"
#include "../../../../src/sm/gtp_sm/gtp_sm_id.h"
#include "../../../../src/sm/kpm_sm_v2.02/kpm_sm_id.h"
#include "../../../../src/util/ngran_types.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

static volatile int keepRunning = 1;

void intHandler() {
  keepRunning = 0;
}

uint64_t count_max = 100;
uint64_t count_mac = 0;
uint64_t aggr_tstamp_mac = 0;
uint64_t count_rlc = 0;
uint64_t aggr_tstamp_rlc = 0;
uint64_t count_pdcp = 0;
uint64_t aggr_tstamp_pdcp = 0;
static
void sm_cb_all(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);

  int64_t now = time_now_us();
  if (rd->type == MAC_STATS_V0) {
    count_mac += 1;
    aggr_tstamp_mac += now - rd->mac_stats.msg.tstamp;
    if (count_mac == count_max) {
      printf("MAC avg ind_msg latency = %ld\n", aggr_tstamp_mac/count_max);
      count_mac = 0;
      aggr_tstamp_mac = 0;
    }
  } else if (rd->type == RLC_STATS_V0) {
    count_rlc += 1;
    aggr_tstamp_rlc += now - rd->rlc_stats.msg.tstamp;
    if (count_rlc == count_max) {
      printf("RLC avg ind_msg latency = %ld\n", aggr_tstamp_rlc/count_max);
      count_rlc = 0;
      aggr_tstamp_rlc = 0;
    }
  } else if (rd->type == PDCP_STATS_V0) {
    count_pdcp += 1;
    aggr_tstamp_pdcp += now - rd->pdcp_stats.msg.tstamp;
    if (count_pdcp == count_max) {
      printf("PDCP avg ind_msg latency = %ld\n", aggr_tstamp_pdcp/count_max);
      count_pdcp = 0;
      aggr_tstamp_pdcp = 0;
    }
  }
  else if (rd->type == GTP_STATS_V0)
    printf("GTP ind_msg latency = %ld\n", now - rd->gtp_stats.msg.tstamp);
  else if (rd->type == KPM_STATS_V0) {
    int64_t diff = now/1000000 - (int64_t)rd->kpm_stats.hdr.collectStartTime;
    if (diff > 1)
      printf("KPM ind_msg latency = %lu seconds\n", diff);
    else
      printf("KPM ind_msg latency < 1 seconds\n");
  }
  else
    assert(0!=0&&"Unknown SM\n");
}

//static
//bool check_sm_id(uint16_t* sm_id_arr, uint16_t num_sm, uint16_t id){
//  for(int i = 0; i < num_sm; i++)
//    if (sm_id_arr[i] == id) return false;
//  return true;
//}

//static
//uint16_t* get_rand_sm_id(uint16_t num_sm)
//{
//  srand(time(0));
//
//  uint16_t* sm_id_arr = NULL;
//  if(num_sm > 0){
//    sm_id_arr = calloc(num_sm, sizeof(uint16_t));
//    assert(sm_id_arr != NULL);
//  }
//
//  int i_sm = 0;
//  while(i_sm < num_sm){
//    uint16_t sm_id = 0;
//    switch(abs(rand()%5)){
//      case 0:
//        sm_id = SM_MAC_ID;
//        break;
//      case 1:
//        sm_id = SM_RLC_ID;
//        break;
//      case 2:
//        sm_id = SM_PDCP_ID;
//        break;
//      case 3:
//        sm_id = SM_GTP_ID;
//        break;
//      case 4:
//        sm_id = SM_KPM_ID;
//        break;
//      default:
//        assert(0!=0&&"Unknown SM ID\n");
//        break;
//    }
//    if(!check_sm_id(sm_id_arr, num_sm, sm_id)) continue;
//    sm_id_arr[i_sm] = sm_id;
//    i_sm++;
//  }
//
//  return sm_id_arr;
//}

size_t max_handle = 256;
size_t c_handle = 0;
static
void send_subscription_req(e2_node_connected_t* n, int n_idx, sm_ans_xapp_t* handle, int num_sm)
{
  // send subscription request to each e2 nodes
  for(size_t j = 0; j < n->len_rf; j++)
    printf("Registered E2 node idx %d, supported RAN Func ID = %d\n ", n_idx, n->ack_rf[j].id);

  inter_xapp_e tti = ms_10;
  if (n->id.type == ngran_gNB) {
    num_sm = 3;
    uint16_t sm_id_arr[3] = {SM_MAC_ID, SM_RLC_ID, SM_PDCP_ID};
    for(size_t j = c_handle; j < c_handle+num_sm; j++){
      uint16_t ran_func_id = sm_id_arr[num_sm-=1];
      printf("xApp subscribes RAN Func ID %d in E2 node idx %d, (ngran_gNB)\n", ran_func_id, n_idx);
      handle[j] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_all);
      assert(handle[j].success == true);
      c_handle+=1;
    }
  } else if (n->id.type == ngran_gNB_CU) {
    num_sm = 1;
    uint16_t sm_id_arr[1] = {SM_PDCP_ID};
    for(size_t j = c_handle; j < c_handle+num_sm; j++){
      uint16_t ran_func_id = sm_id_arr[num_sm-=1];
      printf("xApp subscribes RAN Func ID %d in E2 node idx %d, (ngran_gNB_CU)\n", ran_func_id, n_idx);
      handle[j] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_all);
      assert(handle[j].success == true);
      c_handle+=1;
    }
  } else if (n->id.type == ngran_gNB_DU) {
    num_sm = 2;
    uint16_t sm_id_arr[2] = {SM_MAC_ID, SM_RLC_ID};
    for(size_t j = c_handle; j < c_handle+num_sm; j++){
      uint16_t ran_func_id = sm_id_arr[num_sm-=1];
      printf("xApp subscribes RAN Func ID %d in E2 node idx %d, (ngran_gNB_DU)\n", ran_func_id, n_idx);
      handle[j] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_all);
      assert(handle[j].success == true);
      c_handle+=1;
    }
  } else {
    printf("xApp doesn't support RAN type %d, do not send the subscription request\n", n->id.type);
    return;
  }
//  assert(num_sm == 2);
//  // give the requested RAN func id
//  uint16_t sm_id_arr[2] = {SM_MAC_ID, SM_RLC_ID};
//  // TODO: select the SM from n->ack_rf
//  // uint16_t* sm_id_arr = get_rand_sm_id(num_sm);
//  // TODO: select the transmission interval time
//  inter_xapp_e tti = ms_10;
//
//  for(size_t j = c_handle; j < c_handle+num_sm; j++){
//    uint16_t ran_func_id = sm_id_arr[num_sm-=1];
//    printf("xApp subscribes RAN Func ID %d in E2 node %d\n", ran_func_id, n_idx);
//    handle[j] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_all);
//    assert(handle[j].success == true);
//    c_handle+=1;
//  }
}


int main(int argc, char *argv[])
{
  srand(time(0));
  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);


  // init num of sm and handle
  // TODO: give the num of sm randomly  //abs(rand()%5)+1;
  size_t num_sm = 2;
  sm_ans_xapp_t *handle = NULL;
  if (max_handle > 0) {
    handle = calloc(max_handle, sizeof(sm_ans_xapp_t *));
    assert(handle != NULL);
  }

  signal(SIGINT, intHandler);
  signal(SIGTERM, intHandler);

  size_t nodes_len = e2_nodes_len_xapp_api();
  // start the xApp subscription procedure until detect connected E2 nodes
  while (nodes_len <= 0) {
    // get the original connected e2 nodes info
    e2_node_arr_t nodes = e2_nodes_xapp_api();
    defer({ free_e2_node_arr(&nodes); });
    if (nodes.len > nodes_len) {
      printf("Update connected E2 nodes = %d\n", nodes.len);
      nodes_len = nodes.len;
    } else {
      printf("No E2 node connects\n");
      sleep(1);
    }
  }

  // case1: send subscription req to the original connected e2 node
  // get current e2 nodes info
  e2_node_arr_t cur_nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&cur_nodes); });
  // TODO: send subscription request to new e2 node
  for (size_t i = 0; i < nodes_len; i++) {
    send_subscription_req(&cur_nodes.n[i], i, handle, num_sm);
  }

  // case2: send subscription req to the new connected e2 node
  while(keepRunning) {
    size_t cur_nodes_len = e2_nodes_len_xapp_api();
    if (cur_nodes_len - nodes_len > 0) {
      printf("//////////////// detect E2 nodes len update ////////////////\n");
      sleep(1);
      // get the new e2 nodes info
      e2_node_arr_t cur_nodes = e2_nodes_xapp_api();
      defer({ free_e2_node_arr(&cur_nodes); });

      for (size_t i = 0; i < cur_nodes_len; i++)
        printf("/////////////// E2 node list, idx %ld, nb_id %d, type %s //////////////\n", i, cur_nodes.n[i].id.nb_id, get_ngran_name(cur_nodes.n[i].id.type));

      // TODO: send subscription request to new e2 node
      for (size_t i = nodes_len; i < cur_nodes_len; i++) {
          printf("/////////////// send sub req to new E2 node type %s //////////////\n", get_ngran_name(cur_nodes.n[i].id.type));
          send_subscription_req(&cur_nodes.n[i], i, handle, num_sm);
      }
      nodes_len = cur_nodes_len;
    }
  }

  printf("CTRL+C detect\n");
  // TODO: send subscription request delete
  for(size_t i = 0; i < c_handle; ++i)
    rm_report_sm_xapp_api(handle[i].u.handle);

  // free sm handel
  // TODO: free handle
  free(handle);

  // stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);


  printf("Test xApp run SUCCESSFULLY\n");
}



