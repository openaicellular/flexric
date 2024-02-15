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
#include "../../../../src/sm/rc_sm/ie/ir/ran_param_struct.h"
#include "../../../../src/sm/rc_sm/ie/ir/ran_param_list.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

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
double sojourn_time = 0; 

static
int64_t last_us = 0;

static
ue_id_e2sm_t ue_id;

static
pthread_mutex_t mtx;

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  kpm_ind_data_t const* kpm = &rd->ind.kpm.ind;

  //int64_t now = time_now_us();
  //printf("KPM ind_msg latency = %ld μs\n", now - kpm->hdr.kpm_ric_ind_hdr_format_1.collectStartTime);

  {
  lock_guard(&mtx);
  ue_id = cp_ue_id_e2sm(&kpm->msg.frm_3.meas_report_per_ue[0].ue_meas_report_lst);
  }


  sojourn_time = kpm->msg.frm_3.meas_report_per_ue[0].ind_msg_format_1.meas_data_lst[0].meas_record_lst[0].real_val; 
  static int counter = 0;
  printf("%7d UE RNTI %x default DRB sojourn time %9.3lf\r", counter, (uint32_t)ue_id.gnb.amf_ue_ngap_id, sojourn_time);
  fflush(stdout);
  counter++;

  //printf("UE ID %ld \n ", ue_id.gnb.amf_ue_ngap_id);
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
kpm_act_def_format_1_t gen_act_def_frmt_1(const char* action)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = 100;

  // [1, 65535]
  dst.meas_info_lst_len = 1;
  dst.meas_info_lst = calloc(1, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  *dst.meas_info_lst = gen_meas_info_format_1_lst(action);
 
  return dst;
}

static
test_info_lst_t filter_predicate(test_cond_type_e type, test_cond_e cond, int value)
{
  test_info_lst_t dst = {0};

  dst.test_cond_type = type;
  // It can only be TRUE_TEST_COND_TYPE so it does not matter the type
  dst.S_NSSAI = TRUE_TEST_COND_TYPE;

  dst.test_cond = calloc(1, sizeof(test_cond_e));
  assert(dst.test_cond != NULL && "Memory exhausted");
  *dst.test_cond = cond;

  dst.test_cond_value = calloc(1, sizeof(test_cond_value_t));
  assert(dst.test_cond_value != NULL && "Memory exhausted");
  dst.test_cond_value->type = INTEGER_TEST_COND_VALUE;

  dst.test_cond_value->int_value = calloc(1, sizeof(int64_t));
  assert(dst.test_cond_value->int_value != NULL && "Memory exhausted");
  *dst.test_cond_value->int_value = value;

  return dst;
} 

static
kpm_act_def_format_4_t gen_act_def_frmt_4(const char* action)
{
  kpm_act_def_format_4_t dst = {0};

  // [1, 32768]
  dst.matching_cond_lst_len = 1;

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");

  // Filter connected UEs by S-NSSAI criteria
  test_cond_type_e const type = S_NSSAI_TEST_COND_TYPE;
  test_cond_e const condition = EQUAL_TEST_COND;
  int const value = 1;
  dst.matching_cond_lst[0].test_info_lst = filter_predicate(type, condition, value);

  printf("[xApp]: Filter UEs by S-NSSAI criteria where SST = %lu\n", *dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value);

  // Action definition Format 1 
  dst.action_def_format_1 = gen_act_def_frmt_1(action);  // 8.2.1.2.1

  return dst;
}

static
kpm_act_def_t gen_act_def(const char* act)
{
  kpm_act_def_t dst = {0}; 

  dst.type = FORMAT_4_ACTION_DEFINITION; 
  dst.frm_4 = gen_act_def_frmt_4(act);
  return dst;
}

typedef enum{
  DRB_QoS_Configuration_7_6_2_1 = 1,
  QoS_flow_mapping_configuration_7_6_2_1 = 2,
  Logical_channel_configuration_7_6_2_1 = 3,
  Radio_admission_control_7_6_2_1 = 4,
  DRB_termination_control_7_6_2_1 = 5,
  DRB_split_ratio_control_7_6_2_1 = 6,
  PDCP_Duplication_control_7_6_2_1 = 7,
} rc_ctrl_service_style_1_e;

static
e2sm_rc_ctrl_hdr_frmt_1_t gen_rc_ctrl_hdr_frmt_1(void)
{
  e2sm_rc_ctrl_hdr_frmt_1_t dst = {0};

  // 6.2.2.6
  {
  lock_guard(&mtx);
  dst.ue_id = cp_ue_id_e2sm(&ue_id);
  }
  // CONTROL Service Style 1: Radio Bearer Control
  dst.ric_style_type = 1;

  // QoS flow mapping conf 
  dst.ctrl_act_id = QoS_flow_mapping_configuration_7_6_2_1 ;

  return dst;
}

static
e2sm_rc_ctrl_hdr_t gen_rc_ctrl_hdr(void)
{
  e2sm_rc_ctrl_hdr_t dst = {0};
  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_HDR;
  dst.frmt_1 = gen_rc_ctrl_hdr_frmt_1();
  return dst;
}

typedef enum {
  DRB_ID_8_4_2_2 = 1,
  LIST_OF_QOS_FLOWS_MOD_IN_DRB_8_4_2_2 = 2,
  QOS_FLOW_ITEM_8_4_2_2 = 3,
  QOS_FLOW_ID_8_4_2_2 = 4,
  QOS_FLOW_MAPPING_IND_8_4_2_2 = 5,
} qos_flow_mapping_conf_e;

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1_qos_flow_map(int drb)
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0}; 

  // 8.4.2.2 QoS flow mapping configuration
  dst.sz_ran_param = 2;
  dst.ran_param = calloc(2, sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "Memory exhausted");

  dst.ran_param[0].ran_param_id = DRB_ID_8_4_2_2;
  dst.ran_param[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t)) ;
  assert(dst.ran_param[0].ran_param_val.flag_true != NULL && "Memory exhausted");

  // Let's suppose that it is the DRB 4 or 5 
  assert(drb == 4 || drb == 5);
  dst.ran_param[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  dst.ran_param[0].ran_param_val.flag_true->int_ran = drb; 

  // List of QoS Flows to be modified in DRB
  dst.ran_param[1].ran_param_id = LIST_OF_QOS_FLOWS_MOD_IN_DRB_8_4_2_2;
  dst.ran_param[1].ran_param_val.type = LIST_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[1].ran_param_val.lst = calloc(1, sizeof(ran_param_list_t));
  assert(dst.ran_param[1].ran_param_val.lst != NULL && "Memory exhausted");
  ran_param_list_t* rpl = dst.ran_param[1].ran_param_val.lst;

  rpl->sz_lst_ran_param = 1; 
  rpl->lst_ran_param = calloc(1, sizeof(lst_ran_param_t));
  assert(rpl->lst_ran_param != NULL && "Memory exhausted");

  // QoS Flow Item
  rpl->lst_ran_param[0].ran_param_struct.sz_ran_param_struct = 2;
  rpl->lst_ran_param[0].ran_param_struct.ran_param_struct = calloc(2, sizeof(seq_ran_param_t));
  assert(rpl->lst_ran_param[0].ran_param_struct.ran_param_struct != NULL && "Memory exhausted");
  seq_ran_param_t* rps = rpl->lst_ran_param[0].ran_param_struct.ran_param_struct ;

  // QoS Flow Identifier
  rps[0].ran_param_id = QOS_FLOW_ID_8_4_2_2;
  rps[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  rps[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps[0].ran_param_val.flag_true != NULL && "Memory exhausted");
  rps[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  // Let's suppose that we have QFI 10
  rps[0].ran_param_val.flag_true->int_ran = 10; 

  // QoS Flow Mapping Indication
  rps[1].ran_param_id = QOS_FLOW_MAPPING_IND_8_4_2_2;
  rps[1].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE; 
  rps[1].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps[1].ran_param_val.flag_false != NULL && "Memory exhausted"); 

  // ENUMERATED (ul, dl, ...) 
  rps[1].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps[1].ran_param_val.flag_false->int_ran = 1; 

  return dst;
}

static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg(void)
{
  e2sm_rc_ctrl_msg_t dst = {0}; 

  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_MSG;
  const int drb = 5;
  dst.frmt_1 = gen_rc_ctrl_msg_frmt_1_qos_flow_map(drb);

  return dst;
}

static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg_release_bearer(void)
{
  e2sm_rc_ctrl_msg_t dst = {0}; 

  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_MSG;
  const int drb = 4;
  dst.frmt_1 = gen_rc_ctrl_msg_frmt_1_qos_flow_map(drb);

  return dst;
}

static
sm_ans_xapp_t hndlr_kpm;

static
void stop_and_exit()
{
  // Remove the handle previously returned
  if(hndlr_kpm.u.handle != 0)
    rm_report_sm_xapp_api(hndlr_kpm.u.handle);

  sleep(1);
  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  int rc = pthread_mutex_destroy(&mtx);
  assert(rc == 0);

  exit(EXIT_SUCCESS);
}


static
void sig_handler(int sig_num)
{
  printf("\nEnding the xApp with signal number = %d\n. Please, wait.", sig_num);
  // For the impatient, do not break my code
  static pthread_once_t once = PTHREAD_ONCE_INIT;
  pthread_once(&once, stop_and_exit);
}

int main(int argc, char *argv[])
{
  // Signal handler
  signal(SIGINT, sig_handler);

  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_xapp_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr_xapp(&nodes); });
  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

//  sm_ans_xapp_t* h = calloc(nodes.len, sizeof(sm_ans_xapp_t)); 
//  assert(h != NULL && "Memory exhausted");

  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);

  //////////// 
  // START KPM 
  //////////// 
  kpm_sub_data_t kpm_sub = {0};
  defer({ free_kpm_sub_data(&kpm_sub); });

  // KPM Event Trigger
  uint64_t period_ms = 100;
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

  // KPM Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
  assert(kpm_sub.ad != NULL && "Memory exhausted");
  const char act[] = "DRB.RlcSduDelayDl";
  *kpm_sub.ad = gen_act_def(act); 

  const int KPM_ran_function = 2;

  for(size_t i =0; i < nodes.len; ++i){ 
    if(nodes.n[i].id.type == ngran_gNB_DU){
      printf("Subscribing to KPM RLC in DU \n");
      hndlr_kpm = report_sm_xapp_api(&nodes.n[i].id, KPM_ran_function, &kpm_sub, sm_cb_kpm);
      assert(hndlr_kpm.success == true);
    } else {
      printf(" nodes.n[i].id.type %d \n ", nodes.n[i].id.type );
    }
  } 

  //////////// 
  // END KPM 
  //////////// 

  sleep(1);

  int const RC_ran_function = 3;
  int const one_ms = 1000;
  int const ten_ms = 10000;
  int const three_sec = 3000000;

  int num_drbs = 1;
  while(true){
    if(sojourn_time > ten_ms && num_drbs == 1){
      // RC Control 
      rc_ctrl_req_data_t rc_ctrl = {0};
      defer({ free_rc_ctrl_req_data(&rc_ctrl); });

      rc_ctrl.hdr = gen_rc_ctrl_hdr();
      rc_ctrl.msg = gen_rc_ctrl_msg();

      for(size_t i =0; i < nodes.len; ++i){ 
        if(nodes.n[i].id.type == ngran_gNB_CU ){
          control_sm_xapp_api(&nodes.n[i].id, RC_ran_function, &rc_ctrl);
        }
      }
      num_drbs++;
      last_us = time_now_us(); 
      printf("Creating second bearer\n"); 
      sleep(1);
    } else if(sojourn_time < one_ms && num_drbs == 2 && ((time_now_us() - last_us) > three_sec) ){
      // RC Control 
      rc_ctrl_req_data_t rc_ctrl = {0};
      defer({ free_rc_ctrl_req_data(&rc_ctrl); });

      rc_ctrl.hdr = gen_rc_ctrl_hdr();
      rc_ctrl.msg = gen_rc_ctrl_msg_release_bearer();

      for(size_t i =0; i < nodes.len; ++i){ 
        if(nodes.n[i].id.type == ngran_gNB_CU ){
          control_sm_xapp_api(&nodes.n[i].id, RC_ran_function, &rc_ctrl);
        }
      }
      num_drbs--;
      printf("Releasing second bearer\n"); 
      sleep(1);
    }
  }
}
