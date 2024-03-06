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
 * distributed under the License is distributed on an "AS IS" BAS
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
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

typedef enum {
  RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID = 202,   // 8.2.4  RAN Parameters for Report Service Style 4

  END_E2SM_RC_RAN_PARAM_ID
} ran_param_id_e;

static pthread_mutex_t mtx;

// Print integer value
static void print_int_ran_param_value(int64_t value)
{
  if(value == RRC_CONNECTED_RRC_STATE_E2SM_RC) {
    printf("RAN Parameter Value = RRC_Connected\n");
  } else if(value == RRC_INACTIVE_RRC_STATE_E2SM_RC) {
    printf("RAN Parameter Value = RRC_Inactive\n");
  } else if(value == RRC_IDLE_RRC_STATE_E2SM_RC) {
    printf("RAN Parameter Value = RRC_Idle\n");
  }
}

static void print_element_ran_param_value(ran_parameter_value_t* param_value)
{
  assert(param_value != NULL);

  switch (param_value->type)
  {
  case INTEGER_RAN_PARAMETER_VALUE:
    print_int_ran_param_value(param_value->int_ran);
    break;
  
  default:
    printf("Add corresponding print function for the RAN Parameter Value (other than integer)\n");
  } 
}

static void print_ran_param_name(uint32_t id)
{
  switch (id)
  {
  case RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID:
    printf("RAN Parameter Name = RRC State Changed To\n");
    break;
  
  default:
    printf("Add corresponding RAN Parameter ID\n");
  }
}

static void print_gnb_id_e2sm(gnb_e2sm_t* gnb)
{
  assert(gnb != NULL);

  if (gnb->gnb_cu_ue_f1ap_lst != NULL) {
    for (size_t j = 0; j < gnb->gnb_cu_ue_f1ap_lst_len; j++) {
      printf("UE ID type = gNB-CU, gnb_cu_ue_f1ap = %u\n", gnb->gnb_cu_ue_f1ap_lst[j]);
    }
  } else if (gnb->gnb_cu_cp_ue_e1ap_lst != NULL) {
    for (size_t j = 0; j < gnb->gnb_cu_cp_ue_e1ap_lst_len; j++) {
      printf("UE ID type = gNB-CU-CP, gnb_cu_cp_ue_e1ap = %u\n", gnb->gnb_cu_cp_ue_e1ap_lst[j]);
    }
  } else {
    printf("UE ID type = gNB, amf_ue_ngap_id = %lu\n", gnb->amf_ue_ngap_id);
  }
}

static void sm_cb_rc(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  
  // Reading Indication Message Format 2
  e2sm_rc_ind_msg_frmt_2_t const* ind_msg_frmt_2 = &rd->ind.rc.ind.msg.frmt_2;

  static int counter = 1;
  {
    lock_guard(&mtx);

    printf("\n%7d RC Indication Message\n", counter);

    for(size_t i = 0; i < ind_msg_frmt_2->sz_seq_ue_id; i++) {
      seq_ue_id_t* const ue_id_item = &ind_msg_frmt_2->seq_ue_id[i];

      switch (ue_id_item->ue_id.type)
      {
      case GNB_UE_ID_E2SM:
        print_gnb_id_e2sm(&ue_id_item->ue_id.gnb);
        break;
      
      default:
        assert(false && "E2 Node Type not yet added\n");
      }

      // List parameters
      for(size_t j = 0; j < ue_id_item->sz_seq_ran_param; j++) {
        seq_ran_param_t* const ran_param_item = &ue_id_item->seq_ran_param[j];

        print_ran_param_name(ran_param_item->ran_param_id);

        switch (ran_param_item->ran_param_val.type)
        {
        case ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE:
          print_element_ran_param_value(ran_param_item->ran_param_val.flag_false);
          break;

        case ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE:
          print_element_ran_param_value(ran_param_item->ran_param_val.flag_true);
          break;
        
        default:
          printf("Add corresponding function for the RAN Parameter Value Type (other than element)\n");
        }
      }
    }

    counter++;
  }
}

static rrc_state_lst_t fill_rrc_state_change(void)
{
  rrc_state_lst_t rrc_state_lst = {0};

  rrc_state_lst.sz_rrc_state = 1;
  rrc_state_lst.state_chng_to = calloc(rrc_state_lst.sz_rrc_state, sizeof(rrc_state_t));
  assert(rrc_state_lst.state_chng_to != NULL && "Memory exhausted");

  // 9.3.37
  rrc_state_lst.state_chng_to[0].state_chngd_to = ANY_RRC_STATE_E2SM_RC;

  // 9.3.25
  // Logical OR
  rrc_state_lst.state_chng_to[0].log_or = NULL;

  return rrc_state_lst;
}

static e2sm_rc_ev_trg_frmt_4_t fill_rc_event_trigger_frmt_4(void)
{
  e2sm_rc_ev_trg_frmt_4_t et_frmt_4 = {0};

  et_frmt_4.sz_ue_info_chng = 1;
  et_frmt_4.ue_info_chng = calloc(et_frmt_4.sz_ue_info_chng, sizeof(ue_info_chng_t));
  assert(et_frmt_4.ue_info_chng != NULL && "Memory exhausted");

  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  et_frmt_4.ue_info_chng[0].ev_trig_cond_id = 1;  // this parameter contains rnd value, but must be matched in ind hdr
                                                    /* For each information change configured, Event Trigger Condition ID is assigned 
                                                    so that E2 Node can reply to Near-RT RIC in the RIC INDICATION message to inform
                                                    which event(s) are the cause for triggering. */

  // CHOICE Trigger Type
  et_frmt_4.ue_info_chng[0].type = RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE;
  // RRC State
  // 9.3.37
  et_frmt_4.ue_info_chng[0].rrc_state = fill_rrc_state_change();

  // Associated UE Info 
  // Optional
  // 9.3.26
  et_frmt_4.ue_info_chng[0].assoc_ue_info = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  et_frmt_4.ue_info_chng[0].log_or = NULL;

  return et_frmt_4;
}

static e2sm_rc_act_def_frmt_1_t fill_rc_action_def_frmt_1(void)
{
  e2sm_rc_act_def_frmt_1_t ad_frmt_1 = {0};

  // Parameters to be Reported List
  // [1-65535]
  ad_frmt_1.sz_param_report_def = 1;
  ad_frmt_1.param_report_def = calloc(ad_frmt_1.sz_param_report_def, sizeof(param_report_def_t));
  assert(ad_frmt_1.param_report_def != NULL && "Memory exhausted");

  // RRC State Changed To
  ad_frmt_1.param_report_def[0].ran_param_id = RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID;
  ad_frmt_1.param_report_def[0].ran_param_def = NULL;  // RRC State Changed To parameter is element; only for structure and list

  return ad_frmt_1;
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_xapp_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr_xapp(&nodes); });

  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);

  // RAN Control REPORT handle
  sm_ans_xapp_t* rc_handle = NULL;
  if(nodes.len > 0){
    rc_handle = calloc(nodes.len, sizeof(sm_ans_xapp_t) ); 
    assert(rc_handle  != NULL);
  }

  const int RC_ran_function = 3;

  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_xapp_t* n = &nodes.n[i];
    if(n->id.type == ngran_gNB_DU || n->id.type == ngran_gNB_CUUP) continue;

    for (size_t j = 0; j < n->len_rf; j++){
      printf("[xApp]: registered node %lu ran func id = %d \n ", j, n->rf[j].id);
    }

    ////////////
    // START RC
    ////////////
    rc_sub_data_t rc_sub = {0};
    defer({ free_rc_sub_data(&rc_sub); });

    // Generate Event Trigger
    rc_sub.et.format = FORMAT_4_E2SM_RC_EV_TRIGGER_FORMAT;
    rc_sub.et.frmt_4 = fill_rc_event_trigger_frmt_4();

    // Generate Action Definition
    rc_sub.sz_ad = 1;
    rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
    assert(rc_sub.ad != NULL && "Memory exhausted");

    rc_sub.ad[0].ric_style_type = 4; // REPORT Service Style 4: UE Information
    rc_sub.ad[0].format = FORMAT_1_E2SM_RC_ACT_DEF;
    rc_sub.ad[0].frmt_1 = fill_rc_action_def_frmt_1();

    rc_handle[i] = report_sm_xapp_api(&nodes.n[i].id, RC_ran_function, &rc_sub, sm_cb_rc);
    assert(rc_handle[i].success == true);
  }

  sleep(20);

  for(int i = 0; i < nodes.len; ++i){
    // Remove the handle previously returned
    if (rc_handle[i].success == true)
      rm_report_sm_xapp_api(rc_handle[i].u.handle);
  }

  if(nodes.len > 0){
    free(rc_handle);
  }

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
