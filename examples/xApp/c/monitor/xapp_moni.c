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
#include "../../../../src/sm/kpm_sm/kpm_sm_id_wrapper.h"
#include "../../../../src/util/e2ap_ngran_types.h"

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

//uint64_t count_max = 100;
//uint64_t count_mac = 0;
//uint64_t aggr_tstamp_mac = 0;
//uint64_t count_rlc = 0;
//uint64_t aggr_tstamp_rlc = 0;
//uint64_t count_pdcp = 0;
//uint64_t aggr_tstamp_pdcp = 0;
//uint64_t count_kpm = 0;
//uint64_t aggr_tstamp_kpm = 0;
static
void sm_cb_all(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);

  int64_t now = time_now_us();
  if (rd->ind.type == MAC_STATS_V0) {
//    count_mac += 1;
//    aggr_tstamp_mac += now - rd->ind.mac.msg.tstamp;
//    if (count_mac == count_max) {
//      printf("MAC ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
//             aggr_tstamp_mac/count_max, e2_node->type, e2_node->nb_id);
//      count_mac = 0;
//      aggr_tstamp_mac = 0;
//    }
    printf("MAC ind_msg latency = %lu from E2-node type %d ID %d\n",
           now - rd->ind.mac.msg.tstamp, e2_node->type, e2_node->nb_id);
  } else if (rd->ind.type == RLC_STATS_V0) {
//    count_rlc += 1;
//    aggr_tstamp_rlc += now - rd->ind.rlc.msg.tstamp;
//    if (count_rlc == count_max) {
//      printf("RLC ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
//             aggr_tstamp_rlc/count_max, e2_node->type, e2_node->nb_id);
//      count_rlc = 0;
//      aggr_tstamp_rlc = 0;
//    }
    printf("RLC ind_msg latency = %lu from E2-node type %d ID %d\n",
           now - rd->ind.rlc.msg.tstamp, e2_node->type, e2_node->nb_id);
  } else if (rd->ind.type == PDCP_STATS_V0) {
//    count_pdcp += 1;
//    aggr_tstamp_pdcp += now - rd->ind.pdcp.msg.tstamp;
//    if (count_pdcp == count_max) {
//      printf("PDCP ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
//             aggr_tstamp_pdcp/count_max, e2_node->type, e2_node->nb_id);
//      count_pdcp = 0;
//      aggr_tstamp_pdcp = 0;
//    }
    printf("PDCP ind_msg latency = %lu from E2-node type %d ID %d\n",
           now - rd->ind.pdcp.msg.tstamp, e2_node->type, e2_node->nb_id);
//  } else if (rd->ind.type == GTP_STATS_V0) {
//    printf("GTP ind_msg latency = %ld from E2-node type %d ID %d\n",
//           now - rd->ind.gtp.msg.tstamp, e2_node->type, e2_node->nb_id);
  } else if (rd->ind.type == KPM_STATS_V3_0) {
    if (rd->ind.kpm.ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime) {
//      count_kpm += 1;
//      aggr_tstamp_kpm += now - rd->ind.kpm.ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime;
//      if (count_kpm == count_max) {
//        printf("KPM ind_msg latency (averaged) = %lu from E2-node type %d ID %d\n",
//               aggr_tstamp_kpm/count_max, e2_node->type, e2_node->nb_id);
//        count_kpm = 0;
//        aggr_tstamp_kpm = 0;
//      }
      printf("KPM ind_msg latency = %lu from E2-node type %d ID %d\n",
             now - rd->ind.kpm.ind.hdr.kpm_ric_ind_hdr_format_1.collectStartTime, e2_node->type, e2_node->nb_id);
//      kpm_ind_data_t const* kpm = &rd->ind.kpm.ind;
//      if (kpm->msg.type == FORMAT_1_INDICATION_MESSAGE) {
//        for (size_t i = 0; i < kpm->msg.frm_1.meas_data_lst_len; i++) {
//          for (size_t j = 0; j < kpm->msg.frm_1.meas_data_lst[i].meas_record_len; j++) {
//            if (kpm->msg.frm_1.meas_data_lst[i].meas_record_lst[j].value == INTEGER_MEAS_VALUE)
//              printf("meas record INTEGER_MEAS_VALUE value %d\n", kpm->msg.frm_1.meas_data_lst[i].meas_record_lst[j].int_val);
//            else if (kpm->msg.frm_1.meas_data_lst[i].meas_record_lst[j].value == REAL_MEAS_VALUE)
//              printf("meas record REAL_MEAS_VALUE value %f\n", kpm->msg.frm_1.meas_data_lst[i].meas_record_lst[j].real_val);
//            else
//              printf("meas record NO_VALUE_MEAS_VALUE value\n");
//          }
//        }
//      } else if (kpm->msg.type == FORMAT_3_INDICATION_MESSAGE) {
//        for (size_t i = 0; i < kpm->msg.frm_3.ue_meas_report_lst_len; i++) {
//          if (kpm->msg.frm_3.meas_report_per_ue[i].ue_meas_report_lst.type == GNB_UE_ID_E2SM) {
//            printf("UE ID type %d, amf_ue_ngap_id %ld\n",
//                   kpm->msg.frm_3.meas_report_per_ue[i].ue_meas_report_lst.type,
//                   kpm->msg.frm_3.meas_report_per_ue[i].ue_meas_report_lst.gnb.amf_ue_ngap_id
//            );
//          }
//          if (kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst_len > 0) {
//            // TODO: need to get the all meas data
//            size_t meas_data_idx = 0;
//            if (kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst[meas_data_idx].incomplete_flag) {
//              if (*kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst[meas_data_idx].incomplete_flag == TRUE_ENUM_VALUE)
//                printf("No UE connected to E2-Node, meas_data incomplete_flag == TRUE_ENUM_VALUE\n");
//            } else if (kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_info_lst_len == kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst[0].meas_record_len) {
//              size_t rec_data_len = kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst[meas_data_idx].meas_record_len;
//              for (size_t j = 0; j < rec_data_len; j++) {
//                if (kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_info_lst[j].meas_type.type == NAME_MEAS_TYPE &&
//                    kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst[meas_data_idx].meas_record_lst[j].value == REAL_MEAS_VALUE) {
//                  printf("MeasName %s, MeasData %lf\n",
//                         kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_info_lst[j].meas_type.name.buf,
//                         kpm->msg.frm_3.meas_report_per_ue[i].ind_msg_format_1.meas_data_lst[meas_data_idx].meas_record_lst[j].real_val
//                  );
//                }
//              }
//            }
//          }
//        }
//        //printf("Sojourn time %lf \n",kpm->msg.frm_3.meas_report_per_ue[0].ind_msg_format_1.meas_data_lst[0].meas_record_lst[0].real_val);
//      } else {
//        printf("unknown kpm ind format\n");
//      }
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


static
byte_array_t copy_str_to_ba(const char* str)
{
  assert(str != NULL);

  size_t const sz = strlen(str);
  byte_array_t dst = {.len = sz };
  dst.buf = calloc(sz ,sizeof(uint8_t) );
  assert(dst.buf != NULL);

  memcpy(dst.buf, str, sz);

  return dst;
}

static
kpm_event_trigger_def_t gen_ev_trig(uint64_t period)
{
  kpm_event_trigger_def_t dst = {0};

  dst.type = FORMAT_1_RIC_EVENT_TRIGGER;
  dst.kpm_ric_event_trigger_format_1.report_period_ms = period;

  return dst;
}

static
meas_info_format_1_lst_t gen_meas_info_format_1_lst(const char* action)
{
  meas_info_format_1_lst_t dst = {0};

  dst.meas_type.type = NAME_MEAS_TYPE;
  // ETSI TS 128 552
  dst.meas_type.name = copy_str_to_ba(  action );

  dst.label_info_lst_len = 1;
  dst.label_info_lst = calloc(1, sizeof(label_info_lst_t));
  assert(dst.label_info_lst != NULL && "Memory exhausted");
  dst.label_info_lst[0].noLabel = calloc(1, sizeof(enum_value_e));
  assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
  *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE;

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frmt_1(const char** action)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = 100;

  // [1, 65535]
  size_t count = 0;
  while (action[count] != NULL) {
    count++;
  }
  dst.meas_info_lst_len = count;
  dst.meas_info_lst = calloc(count, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");
  printf("count %ld\n", count);
  for(size_t i = 0; i < dst.meas_info_lst_len; i++) {
    dst.meas_info_lst[i] = gen_meas_info_format_1_lst(action[i]);
  }

  return dst;
}

static
kpm_act_def_format_4_t gen_act_def_frmt_4(const char** action)
{
  kpm_act_def_format_4_t dst = {0};

  // [1, 32768]
  dst.matching_cond_lst_len = 1;

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");

  // Hack. Subscribe to all UEs with CQI greater than 0 to get a list of all available UEs in the RAN
  dst.matching_cond_lst[0].test_info_lst.test_cond_type = CQI_TEST_COND_TYPE;
  dst.matching_cond_lst[0].test_info_lst.CQI = TRUE_TEST_COND_TYPE;

  dst.matching_cond_lst[0].test_info_lst.test_cond = calloc(1, sizeof(test_cond_e));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond = GREATERTHAN_TEST_COND;

  dst.matching_cond_lst[0].test_info_lst.test_cond_value = calloc(1, sizeof(test_cond_value_e));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond_value =  INTEGER_TEST_COND_VALUE;
  dst.matching_cond_lst[0].test_info_lst.int_value = malloc(sizeof(int64_t));
  assert(dst.matching_cond_lst[0].test_info_lst.int_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.int_value = 0;

  // Action definition Format 1
  dst.action_def_format_1 = gen_act_def_frmt_1(action);  // 8.2.1.2.1

  return dst;
}

static
kpm_act_def_t gen_act_def(const char** act, format_action_def_e act_frm)
{
  kpm_act_def_t dst = {0};

  if (act_frm == FORMAT_1_ACTION_DEFINITION) {
    dst.type = FORMAT_1_ACTION_DEFINITION;
    dst.frm_1 = gen_act_def_frmt_1(act);
  } else if (act_frm == FORMAT_4_ACTION_DEFINITION) {
    dst.type = FORMAT_4_ACTION_DEFINITION;
    dst.frm_4 = gen_act_def_frmt_4(act);
  } else {
    assert(0!=0 && "not support action definition type");
  }

  return dst;
}

size_t max_handle = 256;
size_t c_handle = 0;
static
void send_report_sm(global_e2_node_id_t* id,
                    size_t n_idx,
                    sm_ans_xapp_t* handle,
                    uint16_t num_sm,
                    uint16_t* sm_id_arr)
{
  char* tti = "10_ms";
  size_t sm_idx = 0;
  size_t end_c_handle = c_handle + num_sm;
  for(size_t j = c_handle; j < end_c_handle; j++){
    uint16_t ran_func_id = sm_id_arr[sm_idx];
    printf("xApp subscribes RAN Func ID %d in E2 node idx %ld\n", ran_func_id, n_idx);
    if (ran_func_id == SM_KPM_ID){
      kpm_sub_data_t kpm_sub = {0};
      defer({ free_kpm_sub_data(&kpm_sub); });

      // KPM Event Trigger
      uint64_t period_ms = 10;
      kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

      // KPM Action Definition
      kpm_sub.sz_ad = 1;
      kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
      assert(kpm_sub.ad != NULL && "Memory exhausted");
      const char *act[] = {"DRB.RlcSduDelayDl", "DRB.IPThpDl.QCI", "DRB.IPThpUl.QCI", NULL}; // TS 34.425 clause 4.4.6
      format_action_def_e act_type = FORMAT_4_ACTION_DEFINITION;
      *kpm_sub.ad = gen_act_def(act, act_type);


      handle[j] = report_sm_xapp_api(id, ran_func_id, &kpm_sub, sm_cb_all);
    } else {
      handle[j] = report_sm_xapp_api(id, ran_func_id, tti, sm_cb_all);
    }
    assert(handle[j].success == true);
    c_handle+=1;
    sm_idx+=1;
  }
}

static
void send_subscription_req(e2_node_connected_t* n, size_t n_idx, sm_ans_xapp_t* handle) {
  // send subscription request to each e2 nodes
  for (size_t j = 0; j < n->len_rf; j++)
    printf("Registered E2 node idx %ld, supported RAN Func ID = %d\n ", n_idx, n->ack_rf[j].id);

  if (n->id.type == e2ap_ngran_gNB) {
    uint16_t num_sm = 4;
    uint16_t sm_id_arr[4] = {SM_MAC_ID, SM_RLC_ID, SM_PDCP_ID, SM_KPM_ID};
    send_report_sm(&n->id, n_idx, handle, num_sm, sm_id_arr);
  } else if (n->id.type == e2ap_ngran_gNB_CU) {
    uint16_t num_sm = 1;
    uint16_t sm_id_arr[1] = {SM_PDCP_ID};
    send_report_sm(&n->id, n_idx, handle, num_sm, sm_id_arr);
  } else if (n->id.type == e2ap_ngran_gNB_DU) {
    uint16_t num_sm = 3;
    uint16_t sm_id_arr[3] = {SM_MAC_ID, SM_RLC_ID, SM_KPM_ID};
    send_report_sm(&n->id, n_idx, handle, num_sm, sm_id_arr);
  } else {
    printf("xApp doesn't support RAN type %d, do not send the subscription request\n", n->id.type);
    return;
  }
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
    send_subscription_req(&nodes.n[i], i, handle);
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
          //       cur_nodes.n[i].id.nb_id, get_e2ap_ngran_name(cur_nodes.n[i].id.type));
          e2ap_ngran_node_t cur_type = cur_nodes.n[i].id.type;
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
            printf("/////////////// send sub req to new E2 node, nb_id %d, type %s //////////////\n", cur_nodes.n[i].id.nb_id, get_e2ap_ngran_name(cur_nodes.n[i].id.type));
            send_subscription_req(&cur_nodes.n[i], i, handle);
          }
        }
      }
      nodes_len = cur_nodes_len;
      nodes = e2_nodes_xapp_api();

      if (nodes_len == 0)
        c_handle = 0;
    }
    sleep(1);
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



