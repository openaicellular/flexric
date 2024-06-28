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
  RRC_MESSAGE_E2SM_RC_RAN_PARAM_ID_REPORT_1 = 3,  // 8.2.1 RAN Parameters for Report Service Style 1
  UE_ID_E2SM_RC_RAN_PARAM_ID_REPORT_1 = 4,  // 8.2.1 RAN Parameters for Report Service Style 1
  RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID = 202, // 8.2.4  RAN Parameters for Report Service Style 4

  END_E2SM_RC_RAN_PARAM_ID
} ran_param_id_e;

static
pthread_mutex_t mtx;

// Print integer value
static
void log_int_ran_param_value(int64_t value)
{
  if (value == RRC_CONNECTED_RRC_STATE_E2SM_RC) {
    printf("RAN Parameter Value = RRC_Connected\n");
  } else if (value == RRC_INACTIVE_RRC_STATE_E2SM_RC) {
    printf("RAN Parameter Value = RRC_Inactive\n");
  } else if (value == RRC_IDLE_RRC_STATE_E2SM_RC) {
    printf("RAN Parameter Value = RRC_Idle\n");
  }
}

static
void log_element_ran_param_value(ran_parameter_value_t* param_value)
{
  assert(param_value != NULL);

  switch (param_value->type) {
    case INTEGER_RAN_PARAMETER_VALUE:
      log_int_ran_param_value(param_value->int_ran);
      break;

    default:
      printf("Add corresponding print function for the RAN Parameter Value (other than integer)\n");
  }
}

static
void log_ran_param_name(uint32_t id)
{
  switch (id) {
    case RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID:
      printf("RAN Parameter Name = RRC State Changed To\n");
      break;

    default:
      printf("Add corresponding RAN Parameter ID\n");
  }
}

static
void log_gnb_id_e2sm(gnb_e2sm_t* gnb)
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

static
void log_ind_msg_frmt_1(const e2sm_rc_ind_msg_t* msg)
{
  const e2sm_rc_ind_msg_frmt_1_t* ind_msg_frmt_1 = &msg->frmt_1;

  static int counter = 1;
  {
    lock_guard(&mtx);

    printf("\n%7d RC Indication Message\n", counter);

    for (size_t i = 0; i < ind_msg_frmt_1->sz_seq_ue_id; i++) {
      seq_ue_id_t* const ue_id_item = &ind_msg_frmt_1->seq_ue_id[i];

      switch (ue_id_item->ue_id.type) {
        case GNB_UE_ID_E2SM:
          log_gnb_id_e2sm(&ue_id_item->ue_id.gnb);
          break;

        default:
          assert(false && "E2 Node Type not yet added\n");
      }

      // List parameters
      for (size_t j = 0; j < ue_id_item->sz_seq_ran_param; j++) {
        seq_ran_param_t* const ran_param_item = &ue_id_item->seq_ran_param[j];

        log_ran_param_name(ran_param_item->ran_param_id);

        switch (ran_param_item->ran_param_val.type) {
          case ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE:
            log_element_ran_param_value(ran_param_item->ran_param_val.flag_false);
            break;

          case ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE:
            log_element_ran_param_value(ran_param_item->ran_param_val.flag_true);
            break;

          default:
            printf("Add corresponding function for the RAN Parameter Value Type (other than element)\n");
        }
      }
    }

    counter++;
  }
}

static
void log_ind_msg_frmt_2(const e2sm_rc_ind_msg_t* msg)
{
  const e2sm_rc_ind_msg_frmt_2_t* ind_msg_frmt_2 = &msg->frmt_2;

  static int counter = 1;
  {
    lock_guard(&mtx);

    printf("\n%7d RC Indication Message\n", counter);

    for (size_t i = 0; i < ind_msg_frmt_2->sz_seq_ue_id; i++) {
      seq_ue_id_t* const ue_id_item = &ind_msg_frmt_2->seq_ue_id[i];

      switch (ue_id_item->ue_id.type) {
        case GNB_UE_ID_E2SM:
          log_gnb_id_e2sm(&ue_id_item->ue_id.gnb);
          break;

        default:
          assert(false && "E2 Node Type not yet added\n");
      }

      // List parameters
      for (size_t j = 0; j < ue_id_item->sz_seq_ran_param; j++) {
        seq_ran_param_t* const ran_param_item = &ue_id_item->seq_ran_param[j];

        log_ran_param_name(ran_param_item->ran_param_id);

        switch (ran_param_item->ran_param_val.type) {
          case ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE:
            log_element_ran_param_value(ran_param_item->ran_param_val.flag_false);
            break;

          case ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE:
            log_element_ran_param_value(ran_param_item->ran_param_val.flag_true);
            break;

          default:
            printf("Add corresponding function for the RAN Parameter Value Type (other than element)\n");
        }
      }
    }

    counter++;
  }
}

typedef void (*log_ind_msg_data)(const e2sm_rc_ind_msg_t* msg);

static
log_ind_msg_data rc_msg[END_E2SM_RC_IND_MSG] = {
  log_ind_msg_frmt_1,
  log_ind_msg_frmt_2,
  NULL,
  NULL,
  NULL,
};

static
void sm_cb_rc(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);

  // log properly INDICATION formats
  const e2sm_rc_ind_msg_format_e type = rd->ind.rc.ind.msg.format;
  rc_msg[type];
}

static
rrc_state_lst_t fill_rrc_state_change(void)
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

static
ue_info_chng_t fill_ue_info_chng(ue_info_chng_trigger_type_e const trigger_type)
{
  ue_info_chng_t ue_info_chng = {0};

  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  ue_info_chng.ev_trig_cond_id = 1; // this parameter contains rnd value, but must be matched in ind hdr
  /* For each information change configured, Event Trigger Condition ID is assigned
  so that E2 Node can reply to Near-RT RIC in the RIC INDICATION message to inform
  which event(s) are the cause for triggering. */

  // CHOICE Trigger Type
  ue_info_chng.type = trigger_type;

  switch (trigger_type) {
    case RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE: {
      // RRC State
      // 9.3.37
      ue_info_chng.rrc_state = fill_rrc_state_change();
      break;
    }

    default:
      assert(false && "Add requested Trigger Type. At the moment, only RRC State supported");
  }

  // Associated UE Info
  // Optional
  // 9.3.26
  ue_info_chng.assoc_ue_info = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  ue_info_chng.log_or = NULL;

  return ue_info_chng;
}

static
param_report_def_t fill_param_report(uint32_t const ran_param_id, ran_param_def_t const* ran_param_def)
{
  param_report_def_t param_report = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  param_report.ran_param_id = ran_param_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  if (ran_param_def != NULL) {
    param_report.ran_param_def = calloc(1, sizeof(ran_param_def_t));
    assert(param_report.ran_param_def != NULL && "Memory exhausted");
    *param_report.ran_param_def = cp_ran_param_def(ran_param_def);
  }

  return param_report;
}

static
rc_sub_data_t gen_rc_sub_msg(ran_func_def_report_t const* ran_func)
{
  assert(ran_func != NULL);

  rc_sub_data_t rc_sub = {0};

  for (size_t i = 0; i < ran_func->sz_seq_report_sty; i++) {

    if (cmp_str_ba("UE Information", ran_func->seq_report_sty[i].name) == 0) {  // as defined in section 7.4.5, formats used for SUBSCRIPTION msg are known
      size_t const sz = ran_func->seq_report_sty[i].sz_seq_ran_param;

      // Generate Event Trigger
      rc_sub.et.format = ran_func->seq_report_sty[i].ev_trig_type;
      assert(rc_sub.et.format == FORMAT_4_E2SM_RC_EV_TRIGGER_FORMAT && "Event Trigger Format received not valid");
      rc_sub.et.frmt_4.sz_ue_info_chng = sz;
      rc_sub.et.frmt_4.ue_info_chng = calloc(sz, sizeof(ue_info_chng_t));
      assert(rc_sub.et.frmt_4.ue_info_chng != NULL && "Memory exhausted");

      // Generate Action Definition
      rc_sub.sz_ad = 1;
      rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
      assert(rc_sub.ad != NULL && "Memory exhausted");
      rc_sub.ad[0].ric_style_type = 4; // REPORT Service Style 4: UE Information
      rc_sub.ad[0].format = ran_func->seq_report_sty[i].act_frmt_type;
      assert(rc_sub.ad[0].format == FORMAT_1_E2SM_RC_ACT_DEF && "Action Definition Format received not valid");
      rc_sub.ad[0].frmt_1.sz_param_report_def = sz;
      rc_sub.ad[0].frmt_1.param_report_def = calloc(sz, sizeof(param_report_def_t));
      assert(rc_sub.ad[0].frmt_1.param_report_def != NULL && "Memory exhausted");

      // Fill RAN Parameter Info
      for (size_t j = 0; j < sz; j++) {
        assert(cmp_str_ba("RRC State Changed To", ran_func->seq_report_sty[i].ran_param[j].name) == 0 && "Add requested RAN Parameter. At the moment, only RRC State supported");

        ue_info_chng_trigger_type_e const trigger_type = RRC_STATE_UE_INFO_CHNG_TRIGGER_TYPE;
        uint32_t const ran_param_id = ran_func->seq_report_sty[i].ran_param[j].id;
        ran_param_def_t const* ran_param_def = ran_func->seq_report_sty[i].ran_param[j].def;
        // Fill Event Trigger
        rc_sub.et.frmt_4.ue_info_chng[j] = fill_ue_info_chng(trigger_type);
        // Fill Action Definition
        rc_sub.ad[0].frmt_1.param_report_def[j] = fill_param_report(ran_param_id, ran_param_def);
      }
    } else if (cmp_str_ba("Message Copy", ran_func->seq_report_sty[i].name) == 0) {  // as defined in section 7.4.2, formats used for SUBSCRIPTION msg are known
      /* fill properly Event Trigger Format 1 and Action Definition Format 1 with "RRC Message" and "UE ID" RAN Parameter IDs */
    } else {
      assert(false && "Add requested REPORT Style. At the moment, only UE Information supported");
    }
  }

  return rc_sub;
}

static
bool eq_sm(sm_ran_function_t const* elem, int const id)
{
  if (elem->id == id)
    return true;

  return false;
}

static
size_t find_sm_idx(sm_ran_function_t* rf, size_t sz, bool (*f)(sm_ran_function_t const*, int const), int const id)
{
  for (size_t i = 0; i < sz; i++) {
    if (f(&rf[i], id))
      return i;
  }

  assert(0 != 0 && "SM ID could not be found in the RAN Function List");
}

int main(int argc, char* argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  // Init the xApp
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
  sm_ans_xapp_t* hndl = calloc(nodes.len, sizeof(sm_ans_xapp_t));
  assert(hndl != NULL);

  ////////////
  // START RC
  ////////////
  int const RC_ran_function = 3;

  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_xapp_t* n = &nodes.n[i];

    size_t const idx = find_sm_idx(n->rf, n->len_rf, eq_sm, RC_ran_function);
    assert(n->rf[idx].defn.type == RC_RAN_FUNC_DEF_E && "RC is not the received RAN Function");
    // if REPORT Service is supported by E2 node, send SUBSCRIPTION message
    if (n->rf[idx].defn.rc.report != NULL) {
      // Generate RC SUBSCRIPTION message
      rc_sub_data_t rc_sub = gen_rc_sub_msg(n->rf[idx].defn.rc.report);

      hndl[i] = report_sm_xapp_api(&n->id, RC_ran_function, &rc_sub, sm_cb_rc);
      assert(hndl[i].success == true);

      free_rc_sub_data(&rc_sub);
    }
  }
  ////////////
  // END RC
  ////////////

  sleep(10);

  for (int i = 0; i < nodes.len; ++i) {
    // Remove the handle previously returned
    if (hndl[i].success == true)
      rm_report_sm_xapp_api(hndl[i].u.handle);
  }
  free(hndl);

  // Stop the xApp
  while (try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
