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
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "xApp/e42_xapp_api.h"
#include "util/alg_ds/alg/defer.h"
#include "util/time_now_us.h"
#include "sm/mac_sm/mac_sm_id.h"

// static
// void sm_cb_mac(sm_ag_if_rd_t const* rd)
// {
//   assert(rd != NULL);
//   assert(rd->type == MAC_STATS_V0);

//   int64_t now = time_now_us();
//   printf("MAC ind_msg latency = %ld \n", now - rd->mac_stats.msg.tstamp);
// }


static sm_ag_if_wr_t fill_dummy_mac_sm_ctrl_req (void)
{
  sm_ag_if_wr_t wr = {0};
  wr.type = CONTROL_SM_AG_IF_WR;
  wr.ctrl.type = MAC_CTRL_REQ_V0;
  wr.ctrl.mac_ctrl.hdr.dummy = 0;
  wr.ctrl.mac_ctrl.msg.ran_conf_len = 1;
  // call 
  wr.ctrl.mac_ctrl.msg.ran_conf = calloc(wr.ctrl.mac_ctrl.msg.ran_conf_len, sizeof(wr.ctrl.mac_ctrl.msg.ran_conf));
  assert(wr.ctrl.mac_ctrl.msg.ran_conf != NULL && "mem exhausted\n");
  
  wr.ctrl.mac_ctrl.msg.ran_conf[0].isset_pusch_mcs = true;
  wr.ctrl.mac_ctrl.msg.ran_conf[0].pusch_mcs = 1;
  wr.ctrl.mac_ctrl.msg.ran_conf[0].rnti = 1; // XXX: collect the RNTI from connected UE and use that value here instead of a fix one.
  
  return wr;
}


// static sm_ag_if_wr_t fill_mac_sm_ctrl_req(void) 
// {
//   assert (0!=0 && "not implemented yet\n");
//   sm_ag_if_wr_t wr = {0};
//   return wr;
// }

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);
  defer({ free_fr_args(&args); });

  // Init the xApp
  init_xapp_api(&args);
  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });
  assert(nodes.len > 0);
  printf("Connected E2 nodes = %d\n", nodes.len);


  for(size_t i = 0; i < nodes.len; ++i) {
    e2_node_connected_t *n = &nodes.n[i];
    for (size_t j = 0; j < n->len_rf; ++j)
      printf("Registered ran func id = %d \n ", n->ack_rf[j].id);
    // do CTRL operations on the RAN
    sm_ag_if_wr_t ctrl_msg_add = fill_dummy_mac_sm_ctrl_req();
    control_sm_xapp_api(&nodes.n[i].id, SM_MAC_ID, &ctrl_msg_add);
    
    //XXX-TODO: free_mac_ctrl_msg(&ctrl_msg_add.mac_req_ctrl.msg); it is still unimplemented
  }
  sleep(10);

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}