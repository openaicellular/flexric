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

uint64_t count_max_e2nodetype2 = 100;
uint64_t count_kpm_e2nodetype2 = 0;
uint64_t aggr_tstamp_kpm_e2nodetype2 = 0;
uint64_t count_max_e2nodetype7 = 100;
uint64_t count_kpm_e2nodetype7 = 0;
uint64_t aggr_tstamp_kpm_e2nodetype7 = 0;
static
void sm_cb_kpm(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == KPM_STATS_V0);
  int64_t now = time_now_us();
  int64_t ts =  0;
  if (e2_node->type == 2) {
    if (rd->kpm_stats.msg.MeasData_len > 0) {
      if (rd->kpm_stats.msg.MeasData[0].measRecord_len > 0) {
        count_kpm_e2nodetype2 += 1;
        ts = rd->kpm_stats.msg.MeasData[0].measRecord[0].real_val;
        aggr_tstamp_kpm_e2nodetype2 += now - ts;
        if (count_kpm_e2nodetype2 == count_max_e2nodetype2) {
          printf("[%ld] KPM ind_msg latency (averaged) = %lu us, rnti = %.0f, dl_thr = %.2f Mbps, dl_mcs = %.0f, dl_cqi = %.0f from E2-node type %d ID %d\n",
                 now, aggr_tstamp_kpm_e2nodetype2/count_max_e2nodetype2,
                 rd->kpm_stats.msg.MeasData[0].measRecord[9].real_val,
                 rd->kpm_stats.msg.MeasData[0].measRecord[1].real_val,
                 rd->kpm_stats.msg.MeasData[0].measRecord[7].real_val,
                 rd->kpm_stats.msg.MeasData[0].measRecord[8].real_val,
                 e2_node->type, e2_node->nb_id);
          count_kpm_e2nodetype2 = 0;
          aggr_tstamp_kpm_e2nodetype2 = 0;
        }
      }
    }
  } else if (e2_node->type == 7) {
    if (rd->kpm_stats.msg.MeasData_len > 0) {
      if (rd->kpm_stats.msg.MeasData[0].measRecord_len > 0) {
        count_kpm_e2nodetype7 += 1;
        ts = rd->kpm_stats.msg.MeasData[0].measRecord[0].real_val;
        aggr_tstamp_kpm_e2nodetype7 += now - ts;
        if (count_kpm_e2nodetype7 == count_max_e2nodetype7) {
          printf("[%ld] KPM ind_msg latency (averaged) = %lu us, rnti = %.0f, dl_thr = %.2f Mbps, dl_mcs = %.0f, dl_cqi = %.0f from E2-node type %d ID %d\n",
                 now, aggr_tstamp_kpm_e2nodetype7/count_max_e2nodetype7,
                 rd->kpm_stats.msg.MeasData[0].measRecord[9].real_val,
                 rd->kpm_stats.msg.MeasData[0].measRecord[1].real_val,
                 rd->kpm_stats.msg.MeasData[0].measRecord[7].real_val,
                 rd->kpm_stats.msg.MeasData[0].measRecord[8].real_val,
                 e2_node->type, e2_node->nb_id);
          count_kpm_e2nodetype7 = 0;
          aggr_tstamp_kpm_e2nodetype7 = 0;
        }
      }
    }
  } else
    printf("unknown e2 node type");
}

size_t max_handle = 256;
size_t c_handle = 0;
static
void send_subscription_req(e2_node_connected_t* n, int n_idx, sm_ans_xapp_t* handle)
{
  // send subscription request to each e2 nodes
  // for(size_t j = 0; j < n->len_rf; j++)
  //   printf("Registered E2 node idx %d, supported RAN Func ID = %d\n ", n_idx, n->ack_rf[j].id);
  inter_xapp_e tti = ms_10;
  uint16_t ran_func_id = SM_KPM_ID;
  printf("xApp subscribes RAN Func ID %d in E2 node idx %d\n", ran_func_id, n_idx);
  handle[c_handle] = report_sm_xapp_api(&n->id, ran_func_id, tti, sm_cb_kpm);
  assert(handle[c_handle].success == true);
  c_handle+=1;

}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  signal(SIGINT, sigint_handler); // we override the signal mask set in init_xapp_api()
  signal(SIGTERM, sigint_handler);
  sleep(1);

  // init num of sm and handle
  // TODO: give the num of sm randomly  //abs(rand()%5)+1;
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
    if (nodes.n[i].id.type == 2 || nodes.n[i].id.type == 7) {
      send_subscription_req(&nodes.n[i], i, handle);
    }
  }

  // case2: send subscription req to the new connected e2 node
  while(!exit_flag) {
    size_t cur_nodes_len = e2_nodes_len_xapp_api();
    (void)usleep(10000); // we choose 10ms as kpm reporting has the same value
    if (cur_nodes_len != nodes_len) {
      printf("/////// detect E2 nodes len update, new len = %ld, old len = %ld ///////\n", cur_nodes_len, nodes_len);
      printf("Updating E2 nodes list ...\n");
      sleep(1);
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
            if (cur_type == 2 || cur_type == 7) {
              printf("/////////////// send sub req to new E2 node, nb_id %d, type %s //////////////\n", cur_nodes.n[i].id.nb_id, get_ngran_name(cur_nodes.n[i].id.type));
              send_subscription_req(&cur_nodes.n[i], i, handle);
            }
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
