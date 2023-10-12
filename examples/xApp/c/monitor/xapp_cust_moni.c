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
#include "../../../../src/sm/slice_sm/slice_sm_id.h"


#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static
void sm_cb_mac(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type ==INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == MAC_STATS_V0);

  int64_t now = time_now_us();
  printf("MAC ind_msg latency = %ld from E2-node type %d ID %d\n",
         now - rd->ind.mac.msg.tstamp, e2_node->type, e2_node->nb_id.nb_id);
}

static
void sm_cb_rlc(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type ==INDICATION_MSG_AGENT_IF_ANS_V0);

  assert(rd->ind.type == RLC_STATS_V0);

  int64_t now = time_now_us();
  printf("RLC ind_msg latency = %ld from E2-node type %d ID %d\n",
         now - rd->ind.rlc.msg.tstamp, e2_node->type, e2_node->nb_id.nb_id);
}

static
void sm_cb_pdcp(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);

  assert(rd->ind.type == PDCP_STATS_V0);

  int64_t now = time_now_us();
  printf("PDCP ind_msg latency = %ld from E2-node type %d ID %d\n",
         now - rd->ind.pdcp.msg.tstamp, e2_node->type, e2_node->nb_id.nb_id);
}

static
void sm_cb_gtp(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type ==INDICATION_MSG_AGENT_IF_ANS_V0);

  assert(rd->ind.type == GTP_STATS_V0);

  int64_t now = time_now_us();
  printf("GTP ind_msg latency = %ld from E2-node type %d ID %d\n",
         now - rd->ind.gtp.msg.tstamp, e2_node->type, e2_node->nb_id.nb_id);
}

static
void sm_cb_slice(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type ==INDICATION_MSG_AGENT_IF_ANS_V0);

  assert(rd->ind.type == SLICE_STATS_V0);

  int64_t now = time_now_us();
  printf("SLICE ind_msg latency = %ld from E2-node type %d ID %d\n",
         now - rd->ind.slice.msg.tstamp, e2_node->type, e2_node->nb_id.nb_id);
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);
  defer({ free_fr_args(&args); });

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });

  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

  //Init SM handler
  sm_ans_xapp_t* mac_handle = NULL;
  sm_ans_xapp_t* rlc_handle = NULL;
  sm_ans_xapp_t* pdcp_handle = NULL;
  sm_ans_xapp_t* gtp_handle = NULL;
  sm_ans_xapp_t* slice_handle = NULL;

  if(nodes.len > 0){
    mac_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) ); 
    assert(mac_handle  != NULL);
    rlc_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) ); 
    assert(rlc_handle  != NULL);
    pdcp_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) ); 
    assert(pdcp_handle  != NULL);
    gtp_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) );
    assert(gtp_handle  != NULL);
    slice_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) );
    assert(slice_handle  != NULL);
  }

  //Subscribe SMs for all the E2-nodes
  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    for (size_t j = 0; j < n->len_rf; j++)
      printf("Registered node %d ran func id = %d \n ", i, n->ack_rf[j].id);

    for (int32_t j = 0; j < args.sub_cust_sm_len; j++) {
      if (!strcasecmp(args.sub_cust_sm[j].name, "mac")) {
        mac_handle[i] = report_sm_xapp_api(&nodes.n[i].id, SM_MAC_ID, (void*)args.sub_cust_sm[j].time, sm_cb_mac);
        assert(mac_handle[i].success == true);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "rlc")) {
        rlc_handle[i] = report_sm_xapp_api(&nodes.n[i].id, SM_RLC_ID, (void*)args.sub_cust_sm[j].time, sm_cb_rlc);
        assert(rlc_handle[i].success == true);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "pdcp")) {
        pdcp_handle[i] = report_sm_xapp_api(&nodes.n[i].id, SM_PDCP_ID, (void*)args.sub_cust_sm[j].time, sm_cb_pdcp);
        assert(pdcp_handle[i].success == true);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "gtp")) {
        gtp_handle[i] = report_sm_xapp_api(&nodes.n[i].id, SM_GTP_ID, (void*)args.sub_cust_sm[j].time, sm_cb_gtp);
        assert(gtp_handle[i].success == true);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "slice")) {
        slice_handle[i] = report_sm_xapp_api(&nodes.n[i].id, SM_SLICE_ID, (void*)args.sub_cust_sm[j].time, sm_cb_slice);
        assert(slice_handle[i].success == true);

      } else {
        assert(0!=0 && "unknown SM in .conf");
      }
    }

    sleep(1);
  }

  sleep(10);


  for(int i = 0; i < nodes.len; ++i) {
    // Remove the handle previously returned
    for (int32_t j = 0; j < args.sub_cust_sm_len; j++) {
      if (!strcasecmp(args.sub_cust_sm[j].name, "mac")) {
        rm_report_sm_xapp_api(mac_handle[i].u.handle);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "rlc")) {
        rm_report_sm_xapp_api(rlc_handle[i].u.handle);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "pdcp")) {
        rm_report_sm_xapp_api(pdcp_handle[i].u.handle);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "gtp")) {
        rm_report_sm_xapp_api(gtp_handle[i].u.handle);

      } else if (!strcasecmp(args.sub_cust_sm[j].name, "slice")) {
        rm_report_sm_xapp_api(slice_handle[i].u.handle);

      } else {
        assert(0 != 0 && "unknown SM");
      }
    }

    sleep(1);
  }

  if(nodes.len > 0){
    free(mac_handle);
    free(rlc_handle);
    free(pdcp_handle);
    free(gtp_handle);
    free(slice_handle);
  }

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}



