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

static bool exit_flag = false;
static void sigint_handler(int sig)
{
  printf("signal %d received !\n", sig);
  exit_flag = true;
}

uint64_t count_max = 100;
uint64_t count_mac = 0;
uint64_t aggr_tstamp_mac = 0;
uint64_t count_rlc = 0;
uint64_t aggr_tstamp_rlc = 0;
uint64_t count_pdcp = 0;
uint64_t aggr_tstamp_pdcp = 0;
uint64_t count_kpm = 0;
uint64_t aggr_tstamp_kpm = 0;
static
void sm_cb_all(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);

  int64_t now = time_now_us();
  if (rd->type == MAC_STATS_V0) {
    count_mac += 1;
    aggr_tstamp_mac += now - rd->mac_stats.msg.tstamp;
    if (count_mac == count_max) {
      printf("MAC ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
             aggr_tstamp_mac/count_max, e2_node->type, e2_node->nb_id);
      count_mac = 0;
      aggr_tstamp_mac = 0;
    }
  } else if (rd->type == RLC_STATS_V0) {
    count_rlc += 1;
    aggr_tstamp_rlc += now - rd->rlc_stats.msg.tstamp;
    if (count_rlc == count_max) {
      printf("RLC ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
             aggr_tstamp_rlc/count_max, e2_node->type, e2_node->nb_id);
      count_rlc = 0;
      aggr_tstamp_rlc = 0;
    }
  } else if (rd->type == PDCP_STATS_V0) {
    count_pdcp += 1;
    aggr_tstamp_pdcp += now - rd->pdcp_stats.msg.tstamp;
    if (count_pdcp == count_max) {
      printf("PDCP ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
             aggr_tstamp_pdcp/count_max, e2_node->type, e2_node->nb_id);
      count_pdcp = 0;
      aggr_tstamp_pdcp = 0;
    }
  } else if (rd->type == GTP_STATS_V0) {
    printf("GTP ind_msg latency = %ld from E2-node type %d ID %d\n",
           now - rd->gtp_stats.msg.tstamp, e2_node->type, e2_node->nb_id);
  } else if (rd->type == KPM_STATS_V0) {
    // int64_t diff = now/1000000 - (int64_t)rd->kpm_stats.hdr.collectStartTime;
    if (rd->kpm_stats.msg.MeasData_len > 0) {
      if (rd->kpm_stats.msg.MeasData[0].measRecord_len > 0) {
        count_kpm += 1;
        aggr_tstamp_kpm += now - rd->kpm_stats.msg.MeasData[0].measRecord[0].real_val;
        if (count_kpm == count_max) {
          printf("KPM ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
                 aggr_tstamp_kpm/count_max, e2_node->type, e2_node->nb_id);
          count_kpm = 0;
          aggr_tstamp_kpm = 0;
        }
      }
    }
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
    num_sm = 4;
    uint16_t sm_id_arr[4] = {SM_MAC_ID, SM_RLC_ID, SM_PDCP_ID, SM_KPM_ID};
    for(size_t j = c_handle; j < c_handle+num_sm; j++){
      uint16_t ran_func_id = sm_id_arr[num_sm-=1];
      printf("xApp subscribes RAN Func ID %d in E2 node idx %d, ngran_gNB\n", ran_func_id, n_idx);
      handle[j] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_all);
      assert(handle[j].success == true);
      c_handle+=1;
    }
  } else if (n->id.type == ngran_gNB_CU) {
    num_sm = 1;
    uint16_t sm_id_arr[1] = {SM_PDCP_ID};
    for(size_t j = c_handle; j < c_handle+num_sm; j++){
      uint16_t ran_func_id = sm_id_arr[num_sm-=1];
      printf("xApp subscribes RAN Func ID %d in E2 node idx %d, ngran_gNB_CU\n", ran_func_id, n_idx);
      handle[j] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_all);
      assert(handle[j].success == true);
      c_handle+=1;
    }
  } else if (n->id.type == ngran_gNB_DU) {
    num_sm = 2;
    uint16_t sm_id_arr[2] = {SM_MAC_ID, SM_RLC_ID};
    for(size_t j = c_handle; j < c_handle+num_sm; j++){
      uint16_t ran_func_id = sm_id_arr[num_sm-=1];
      printf("xApp subscribes RAN Func ID %d in E2 node idx %d, ngran_gNB_DU\n", ran_func_id, n_idx);
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
  signal(SIGINT, sigint_handler); // we override the signal mask set in init_xapp_api()
  signal(SIGTERM, sigint_handler);
  sleep(1);


  // init num of sm and handle
  // TODO: give the num of sm randomly  //abs(rand()%5)+1;
  size_t num_sm = 2;
  sm_ans_xapp_t *handle = NULL;
  if (max_handle > 0) {
    handle = calloc(max_handle, sizeof(sm_ans_xapp_t *));
    assert(handle != NULL);
  }

  size_t nodes_len = e2_nodes_len_xapp_api();
  // start the xApp subscription procedure until detect connected E2 nodes
  while (nodes_len <= 0) {
    // get the original connected e2 nodes info
    size_t tmp_len = e2_nodes_len_xapp_api();
    if (tmp_len > nodes_len) {
      printf("Update connected E2 nodes len = %ld\n", tmp_len);
      nodes_len = tmp_len;
    } else {
      printf("No E2 node connects\n");
      sleep(1);
    }
  }

  // case1: send subscription req to the original connected e2 node
  // get original e2 nodes info
  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });
  for (size_t i = 0; i < nodes.len; i++) {
    send_subscription_req(&nodes.n[i], i, handle, num_sm);
  }

  // case2: send subscription req to the new connected e2 node
  while(!exit_flag) {
    size_t cur_nodes_len = e2_nodes_len_xapp_api();

    if (cur_nodes_len != nodes_len) {
      printf("/////// detect E2 nodes len update, new len = %ld, old len = %ld ///////\n", cur_nodes_len, nodes_len);

      if (cur_nodes_len != 0) {
        // get the new e2 nodes info
        e2_node_arr_t cur_nodes = e2_nodes_xapp_api();
        defer({ free_e2_node_arr(&cur_nodes); });

        // TODO: send subscription request to new e2 node
        for (size_t i = 0; i < cur_nodes_len; i++) {
          //printf("/////////////// new E2 node list, idx %ld, nb_id %d, type %s //////////////\n", i,
          //       cur_nodes.n[i].id.nb_id, get_ngran_name(cur_nodes.n[i].id.type));
          ngran_node_t cur_type = cur_nodes.n[i].id.type;
          uint32_t cur_nb_id = cur_nodes.n[i].id.nb_id;
          bool new_type = 1;
          bool new_nb_id = 1;
          // compare the type between old and new e2 nodes list
          for (size_t j = 0; j < nodes_len; j++) {
            //printf("/////////////// old E2 node list, idx %ld, nb_id %d, type %s //////////////\n", j,
            //       nodes.n[j].id.nb_id, get_ngran_name(nodes.n[j].id.type));
            if (nodes.n[j].id.type == cur_type) new_type = 0;
            if (nodes.n[j].id.nb_id == cur_nb_id) new_nb_id = 0;
          }
          if (new_type || new_nb_id) {
            printf("/////////////// send sub req to new E2 node, nb_id %d, type %s //////////////\n", cur_nodes.n[i].id.nb_id, get_ngran_name(cur_nodes.n[i].id.type));
            send_subscription_req(&cur_nodes.n[i], i, handle, num_sm);
          }
        }
      }
      nodes_len = cur_nodes_len;
      nodes = e2_nodes_xapp_api();

      if (nodes_len == 0)
        c_handle = 0;
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



