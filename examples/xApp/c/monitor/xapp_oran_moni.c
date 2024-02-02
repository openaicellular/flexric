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
#include "../../../../src/util/e2ap_ngran_types.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../../../src/sm/kpm_sm/kpm_sm_id_wrapper.h"
#include "../../../../src/sm/rc_sm/rc_sm_id.h"
#include "../../../../src/sm/rc_sm/ie/rc_data_ie.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

static bool exit_flag = false;
static void sigint_handler(int sig)
{
  printf("signal %d received !\n", sig);
  exit_flag = true;
}

static
pthread_mutex_t mtx;

////////////
// Get RC Indication Messages -> begin
////////////

static void sm_cb_rc(sm_ag_if_rd_t const *rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == RAN_CTRL_STATS_V1_03);
  (void) e2_node;

  // Reading Indication Message Format 2
  e2sm_rc_ind_msg_frmt_2_t const *msg_frm_2 = &rd->ind.rc.ind.msg.frmt_2;

  printf("\n RC REPORT Style 2 - Call Process Outcome\n");

  // Sequence of UE Identifier
  //[1-65535]
  for (size_t i = 0; i < msg_frm_2->sz_seq_ue_id; i++)
  {
    // UE ID
    // Mandatory
    // 9.3.10
    switch (msg_frm_2->seq_ue_id[i].ue_id.type)
    {
      case GNB_UE_ID_E2SM:
        printf("UE connected to gNB with amf_ue_ngap_id = %lu\n", msg_frm_2->seq_ue_id[i].ue_id.gnb.amf_ue_ngap_id);
        break;
      default:
        printf("Not yet implemented UE ID type");
    }
  }
}

////////////
// Get RC Indication Messages -> end
////////////

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0);

  kpm_ind_data_t const* kpm = &rd->ind.kpm.ind;
  kpm_ric_ind_hdr_format_1_t const* hdr_frm_1 = &kpm->hdr.kpm_ric_ind_hdr_format_1;

  int64_t now = time_now_us();

  {
    lock_guard(&mtx);

    static int counter = 1;
#if defined(KPM_V2_01) || defined (KPM_V2_03)
    // collectStartTime (32bit) unit is second
    printf("%7d, KPM v2 ind_msg latency > %ld s (minimum time unit is in second) from E2-node type %d ID %d\n",
           counter, now/1000000 - hdr_frm_1->collectStartTime,
           e2_node->type, e2_node->nb_id.nb_id);
#elif defined(KPM_V3_00)
    // collectStartTime (64bit) unit is micro-second
    printf("%7d, KPM v3 ind_msg latency = %ld μs from E2-node type %d ID %d\n",
           counter, now - hdr_frm_1->collectStartTime,
           e2_node->type, e2_node->nb_id.nb_id);
#else
    static_assert(0!=0, "Unknown KPM version");
#endif

    if (kpm->msg.type == FORMAT_1_INDICATION_MESSAGE)
    {
      kpm_ind_msg_format_1_t const* msg_frm_1 = &kpm->msg.frm_1;
      for (size_t i = 0; i < msg_frm_1->meas_data_lst_len; i++) {
        for (size_t j = 0; j < msg_frm_1->meas_data_lst[i].meas_record_len; j++) {
          if (msg_frm_1->meas_data_lst[i].meas_record_lst[j].value == INTEGER_MEAS_VALUE)
            printf("meas record INTEGER_MEAS_VALUE value %d\n",msg_frm_1->meas_data_lst[i].meas_record_lst[j].int_val);
          else if (msg_frm_1->meas_data_lst[i].meas_record_lst[j].value == REAL_MEAS_VALUE)
            printf("meas record REAL_MEAS_VALUE value %f\n", msg_frm_1->meas_data_lst[i].meas_record_lst[j].real_val);
          else
            printf("meas record NO_VALUE_MEAS_VALUE value\n");
        }
      }
    } else if (kpm->msg.type == FORMAT_3_INDICATION_MESSAGE)
    {
      kpm_ind_msg_format_3_t const* msg_frm_3 = &kpm->msg.frm_3;
      // Reported list of measurements per UE
      for (size_t i = 0; i < msg_frm_3->ue_meas_report_lst_len; i++)
      {
        switch (msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.type)
        {
          case GNB_UE_ID_E2SM:
            if (msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb.gnb_cu_ue_f1ap_lst != NULL)
            {
              for (size_t j = 0; j < msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb.gnb_cu_ue_f1ap_lst_len; j++)
              {
                printf("UE ID type = gNB-CU, gnb_cu_ue_f1ap = %u\n", msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb.gnb_cu_ue_f1ap_lst[j]);
              }
            }
            else
            {
              printf("UE ID type = gNB, amf_ue_ngap_id = %lu\n", msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb.amf_ue_ngap_id);
            }
            break;

          case GNB_DU_UE_ID_E2SM:
            printf("UE ID type = gNB-DU, gnb_cu_ue_f1ap = %u\n", msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb_du.gnb_cu_ue_f1ap);
            break;
          case GNB_CU_UP_UE_ID_E2SM:
            printf("UE ID type = gNB-CU, gnb_cu_cp_ue_e1ap = %u\n", msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb_cu_up.gnb_cu_cp_ue_e1ap);
            break;

          default:
            assert(false && "UE ID type not yet implemented");
        }

        kpm_ind_msg_format_1_t const* msg_frm_1 = &msg_frm_3->meas_report_per_ue[i].ind_msg_format_1;

        // UE Measurements per granularity period
        for (size_t j = 0; j<msg_frm_1->meas_data_lst_len; j++)
        {
          for (size_t z = 0; z<msg_frm_1->meas_data_lst[j].meas_record_len; z++)
          {
            if (msg_frm_1->meas_info_lst_len > 0)
            {
              switch (msg_frm_1->meas_info_lst[z].meas_type.type)
              {
                case NAME_MEAS_TYPE:
                {
                  // Get the Measurement Name
                  char meas_info_name_str[msg_frm_1->meas_info_lst[z].meas_type.name.len + 1];
                  memcpy(meas_info_name_str, msg_frm_1->meas_info_lst[z].meas_type.name.buf, msg_frm_1->meas_info_lst[z].meas_type.name.len);
                  meas_info_name_str[msg_frm_1->meas_info_lst[z].meas_type.name.len] = '\0';

                  // Get the value of the Measurement
                  switch (msg_frm_1->meas_data_lst[j].meas_record_lst[z].value)
                  {
                    case REAL_MEAS_VALUE:
                    {
                      printf("%s = %.2f\n", meas_info_name_str, msg_frm_1->meas_data_lst[j].meas_record_lst[z].real_val);
//                      if (strcmp(meas_info_name_str, "DRB.RlcSduDelayDl") == 0)
//                      {
//                        printf("DRB.RlcSduDelayDl = %.2f [μs]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].real_val);
//                      }
//                      else if (strcmp(meas_info_name_str, "DRB.UEThpDl") == 0)
//                      {
//                        printf("DRB.UEThpDl = %.2f [kbps]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].real_val);
//                      }
//                      else if (strcmp(meas_info_name_str, "DRB.UEThpUl") == 0)
//                      {
//                        printf("DRB.UEThpUl = %.2f [kbps]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].real_val);
//                      }
//                      else
//                      {
//                        assert(false && "Measurement Name not yet implemented");
//                      }

                      break;
                    }


                    case INTEGER_MEAS_VALUE:
                    {
                      printf("%s = %d\n", meas_info_name_str, msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
//
//                      if (strcmp(meas_info_name_str, "RRU.PrbTotDl") == 0)
//                      {
//                        printf("RRU.PrbTotDl = %d [PRBs]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
//                      }
//                      else if (strcmp(meas_info_name_str, "RRU.PrbTotUl") == 0)
//                      {
//                        printf("RRU.PrbTotUl = %d [PRBs]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
//                      }
//                      else if (strcmp(meas_info_name_str, "DRB.PdcpSduVolumeDL") == 0)
//                      {
//                        printf("DRB.PdcpSduVolumeDL = %d [kb]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
//                      }
//                      else if (strcmp(meas_info_name_str, "DRB.PdcpSduVolumeUL") == 0)
//                      {
//                        printf("DRB.PdcpSduVolumeUL = %d [kb]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
//                      }
//                      else
//                      {
//                        assert(false && "Measurement Name not yet implemented");
//                      }

                      break;
                    }

                    default:
                      assert("Value not recognized");
                  }

                  break;
                }

                default:
                  assert(false && "Measurement Type not yet implemented");
              }
            }


            if (msg_frm_1->meas_data_lst[j].incomplete_flag && *msg_frm_1->meas_data_lst[j].incomplete_flag == TRUE_ENUM_VALUE)
              printf("Measurement Record not reliable\n");
          }
        }
      }
    } else {
      printf("unknown kpm ind format\n");
    }

    counter++;
  }
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
  dst.meas_type.name = cp_str_to_ba(action);

  dst.label_info_lst_len = 1;
  dst.label_info_lst = calloc(1, sizeof(label_info_lst_t));
  assert(dst.label_info_lst != NULL && "Memory exhausted");
  dst.label_info_lst[0].noLabel = calloc(1, sizeof(enum_value_e));
  assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
  *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE;

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frmt_1(const char** action, uint32_t period_ms)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = period_ms;

  // [1, 65535]
  size_t count = 0;
  while (action[count] != NULL) {
    count++;
  }
  dst.meas_info_lst_len = count;
  dst.meas_info_lst = calloc(count, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_lst_len; i++) {
    dst.meas_info_lst[i] = gen_meas_info_format_1_lst(action[i]);
  }

  return dst;
}

static
kpm_act_def_format_4_t gen_act_def_frmt_4(const char** action, uint32_t period_ms)
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

  dst.matching_cond_lst[0].test_info_lst.test_cond_value = calloc(1, sizeof(test_cond_value_t));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value != NULL && "Memory exhausted");
  dst.matching_cond_lst[0].test_info_lst.test_cond_value->type = INTEGER_TEST_COND_VALUE;
  dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value = malloc(sizeof(int64_t));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value = 0;

  // Action definition Format 1
  dst.action_def_format_1 = gen_act_def_frmt_1(action, period_ms);  // 8.2.1.2.1

  return dst;
}


static
kpm_act_def_t gen_act_def(const char** act, format_action_def_e act_frm, uint32_t period_ms)
{
  kpm_act_def_t dst = {0};

  if (act_frm == FORMAT_1_ACTION_DEFINITION) {
    dst.type = FORMAT_1_ACTION_DEFINITION;
    dst.frm_1 = gen_act_def_frmt_1(act, period_ms);
  } else if (act_frm == FORMAT_4_ACTION_DEFINITION) {
    dst.type = FORMAT_4_ACTION_DEFINITION;
    dst.frm_4 = gen_act_def_frmt_4(act, period_ms);
  } else {
    assert(0!=0 && "not support action definition type");
  }

  return dst;
}

typedef enum
{
  CURRENT_UE_ID_8_2_2 = 1,
  OLD_UE_ID_8_2_2 = 2,
  CURRENT_RRC_STATE_8_2_2 = 3,
  OLD_RRC_STATE_8_2_2 = 4,
  UE_CONTEXT_INFO_CONTAINER_8_2_2 = 5,
  CELL_GLOBAL_ID_8_2_2 = 6,
  UE_INFORMATION_8_2_2 = 7,
  MASTER_NODE_8_1_1_17 = 21501,
  GNB_MEASUREMENTS_MASTER_NODE_8_1_1_17 = 21502,
  CHOICE_PRIMARY_CELL_OF_MCG_8_1_1_17 = 21503,
  NR_CELL_PRIMARY_MCG_8_1_1_17 = 21504,
  EUTRA_CELL_PRIMARY_MCG_8_1_1_17 = 21505,
  LIST_OF_SECONDARY_CELLS_OF_MCG_8_1_1_17 = 21506,
  SCELL_ITEM_MCG_8_1_1_17 = 21507,
  CHOICE_SCELL_MSG_8_1_1_17 = 21508,
  NR_CELL_SECONDARY_MCG_8_1_1_17 = 21509,
  EUTRA_CELL_SECONDARY_MCG_8_1_1_17 = 21510,
  SECONDARY_NODE_8_1_1_17 = 21511,
  GNB_MEASUREMENTS_SECONDARY_NODE_8_1_1_17 = 21512,
  CHOICE_PRIMARY_CELL_OF_SCG_8_1_1_17 = 21513,
  NR_CELL_PRIMARY_SCG_8_1_1_17 = 21514,
  EUTRA_CELL_PRIMARY_SCG_8_1_1_17 = 21515,
  LIST_OF_SECONDARY_CELLS_OF_SCG_8_1_1_17 = 21516,
  SCELL_ITEM_SCG_8_1_1_17 = 21517,
  CHOICE_SCELL_SCG_8_1_1_17 = 21518,
  NR_CELL_SECONDARY_SCG_8_1_1_17 = 21519,
  EUTRA_CELL_SECONDARY_SCG_8_1_1_17 = 21520,
  LIST_OF_PDU_SESSIONS_8_1_1_17 = 21521,
  PDU_SESSION_ITEM_8_1_1_17 = 21522,
  PDU_SESSION_ID_8_1_1_17 = 21543,
  PDU_SESSION_8_1_1_17 = 21523,
  LIST_OF_DRBS_8_1_1_17 = 21524,
  DRB_ITEM_8_1_1_17 = 21525,
  DRB_ID_8_1_1_17 = 21546,
  DRB_8_1_1_17 = 21547,
  LIST_OF_QOS_FLOWS_MAPPED_TO_DRB_8_1_1_17 = 21526,
  QOS_FLOW_ITEM_8_1_1_17 = 21527,
  QOS_FLOW_IDENTIFIER_8_1_1_17 = 21548,
  QOS_FLOW_8_1_1_17 = 21549,
  LIST_OF_NEIGHBOR_CELLS_8_1_1_17 = 21528,
  NEIGHBOR_CELL_ITEM_8_1_1_17 = 21529,
  CHOICE_NEIGHBOR_CELL_8_1_1_17 = 21530,
  NR_CELL_NEIGHBOR_8_1_1_17 = 21531,
  EUTRA_CELL_NEIGHBOR_8_1_1_17 = 21532,

} ran_param_report_style_2_e;


//static e2sm_rc_act_def_frmt_1_t gen_rc_act_def_frm_1(const char** action)
//{
//  e2sm_rc_act_def_frmt_1_t act_def_frm_1 = {0};
//
//  size_t count = 0;
//  while (action[count] != NULL) {
//    count++;
//  }
//
//  // Parameters to be Reported List
//  // [1-65535]
//  // 8.2.2
//  act_def_frm_1.sz_param_report_def = count;
//  act_def_frm_1.param_report_def = calloc(act_def_frm_1.sz_param_report_def, sizeof(param_report_def_t));
//  assert(act_def_frm_1.param_report_def != NULL && "Memory exhausted");
//  for(size_t i = 0; i < act_def_frm_1.sz_param_report_def; i++) {
//    act_def_frm_1.param_report_def[i].ran_param_id = (uint32_t) action[i];
//    // TODO: Add parameters report definition
//  }
//
//  return act_def_frm_1;
//}

static e2sm_rc_ev_trg_frmt_2_t gen_rc_event_trigger_frm_2(void)
{
  e2sm_rc_ev_trg_frmt_2_t ev_trigger = {0};

  //  Call Process Type ID
  //  Mandatory
  //  9.3.15
  ev_trigger.call_proc_type_id = 3; // Mobility Management

  // Call Breakpoint ID
  // Mandatory
  // 9.3.49
  ev_trigger.call_break_id = 1; // Handover Preparation

  // Associated E2 Node Info
  // Optional
  // 9.3.29
  ev_trigger.assoc_e2_node_info = NULL;

  // Associated UE Info
  // Optional
  // 9.3.26
  ev_trigger.assoc_ue_info = NULL;

  return ev_trigger;
}


//static e2sm_rc_action_def_t gen_rc_act_def(const char** act, e2sm_rc_act_def_format_e act_frm)
//{
//  e2sm_rc_action_def_t dst = {0};
//
//  if (act_frm == FORMAT_1_E2SM_RC_ACT_DEF) {
//    dst.format = FORMAT_1_E2SM_RC_ACT_DEF;
//    dst.frmt_1 = gen_rc_act_def_frm_1(act);
//  } else {
//    assert(0!=0 && "not support action definition type");
//  }
//
//  return dst;
//}

static e2sm_rc_act_def_frmt_1_t gen_rc_act_def_frm_1_manually(void)
{
  e2sm_rc_act_def_frmt_1_t act_def_frm_1 = {0};

  // Parameters to be Reported List
  // [1-65535]
  // 8.2.2
  act_def_frm_1.sz_param_report_def = 3;
  act_def_frm_1.param_report_def = calloc(act_def_frm_1.sz_param_report_def, sizeof(param_report_def_t));
  assert(act_def_frm_1.param_report_def != NULL && "Memory exhausted");

  // Current UE ID RAN Parameter
  act_def_frm_1.param_report_def[0].ran_param_id = CURRENT_UE_ID_8_2_2;
  act_def_frm_1.param_report_def[1].ran_param_id = CELL_GLOBAL_ID_8_2_2;
  act_def_frm_1.param_report_def[2].ran_param_id = LIST_OF_NEIGHBOR_CELLS_8_1_1_17;

  return act_def_frm_1;
}

static
e2sm_rc_event_trigger_t gen_rc_ev_trigger(e2sm_rc_ev_trigger_format_e act_frm)
{
  e2sm_rc_event_trigger_t dst = {0};

  if (act_frm == FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT) {
    dst.format = FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT;
    dst.frmt_2 = gen_rc_event_trigger_frm_2();
  } else {
    assert(0!=0 && "not support event trigger type");
  }

  return dst;
}

static
size_t send_sub_req(e2_node_connected_t* n, fr_args_t args, sm_ans_xapp_t *kpm_handle, size_t n_handle)
{
  for (int32_t j = 0; j < args.sub_oran_sm_len; j++) {
    if (!strcasecmp(args.sub_oran_sm[j].name, "kpm")) {
      kpm_sub_data_t kpm_sub = {0};
      defer({ free_kpm_sub_data(&kpm_sub); });

      // KPM Event Trigger
      uint64_t period_ms = args.sub_oran_sm[j].time;
      kpm_sub.ev_trg_def = gen_ev_trig(period_ms);
      printf("[xApp]: reporting period = %lu [ms]\n", period_ms);

      // KPM Action Definition
      kpm_sub.sz_ad = 1;
      kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
      assert(kpm_sub.ad != NULL && "Memory exhausted");

      format_action_def_e act_type;
      if (args.sub_oran_sm[j].format == 1)
        act_type = FORMAT_1_ACTION_DEFINITION;
      else if (args.sub_oran_sm[j].format == 4)
        act_type = FORMAT_4_ACTION_DEFINITION;
      else
      assert(0!=0 && "not supported action definition format");

      *kpm_sub.ad = gen_act_def((const char**)args.sub_oran_sm[j].actions, act_type, period_ms);

      // TODO: implement e2ap_ngran_eNB
      if (n->id.type == e2ap_ngran_eNB)
        continue;
      if (strcasecmp(args.sub_oran_sm[j].ran_type, get_e2ap_ngran_name(n->id.type)))
        continue;

      kpm_handle[n_handle] = report_sm_xapp_api(&n->id, SM_KPM_ID, &kpm_sub, sm_cb_kpm);
      assert(kpm_handle[n_handle].success == true);
      n_handle += 1;
    } else if (!strcasecmp(args.sub_oran_sm[j].name, "rc")) {
      rc_sub_data_t rc_sub = {0};
      defer({ free_rc_sub_data(&rc_sub); });

      // RC Event Trigger
      rc_sub.et = gen_rc_ev_trigger(FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT);

      // RC Action Definition
      rc_sub.sz_ad = 1;
      rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
      assert(rc_sub.ad != NULL && "Memory exhausted");

//      e1sm_rc_act_def_format_e act_type;
//      if (args.sub_oran_sm[j].format == 4)
//          act_type = FORMAT_0_E2SM_RC_ACT_DEF;
//      else
//          assert(-1!=0 && "not supported action definition format");
      rc_sub.ad[0].ric_style_type = 2; // 7.4.1
      rc_sub.ad[0].format = FORMAT_1_E2SM_RC_ACT_DEF;
      rc_sub.ad[0].frmt_1 = gen_rc_act_def_frm_1_manually();

//      *rc_sub.ad = gen_rc_act_def((const char**)args.sub_oran_sm[j].actions, act_type);
//      *rc_sub.ad = gen_rc_act_def_frm_1();

      // RC HO only supports for e1ap_ngran_gNB
      if (n->id.type == e2ap_ngran_eNB || n->id.type == e2ap_ngran_gNB_CU || n->id.type == e2ap_ngran_gNB_DU)
          continue;
      if (strcasecmp(args.sub_oran_sm[j].ran_type, get_e2ap_ngran_name(n->id.type)))
          continue;

      kpm_handle[n_handle] = report_sm_xapp_api(&n->id, SM_RC_ID, &rc_sub, sm_cb_rc);
      assert(kpm_handle[n_handle].success == true);
      n_handle += 0;
    } else {
      assert(0!=0 && "unknown SM in .conf");
    }
  }
  return n_handle;
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);
  defer({ free_fr_args(&args); });

  //Init the xApp
  init_xapp_api(&args);
  signal(SIGINT, sigint_handler); // we override the signal mask set in init_xapp_api()
  signal(SIGTERM, sigint_handler);
  sleep(1);

  int max_handle = 256;
  // KPM indication
  sm_ans_xapp_t *kpm_handle = NULL;
  if (max_handle > 0) {
    kpm_handle = calloc(max_handle, sizeof(sm_ans_xapp_t *));
    assert(kpm_handle != NULL);
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

  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);

  size_t n_handle = 0;
  // case1: send subscription req to the original connected e2 node
  // get original e2 nodes info
  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });
  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    for (size_t j = 0; j < n->len_rf; j++)
      printf("Registered node %d ran func id = %d \n ", i, n->ack_rf[j].id);

    n_handle = send_sub_req(n, args, kpm_handle, n_handle);
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
          //       cur_nodes.n[i].id.nb_id.nb_id, get_e2ap_ngran_name(cur_nodes.n[i].id.type));
          e2ap_ngran_node_t cur_type = cur_nodes.n[i].id.type;
          uint32_t cur_nb_id = cur_nodes.n[i].id.nb_id.nb_id;
          bool new_type = 1;
          bool new_nb_id = 1;
          // compare the type between old and new e2 nodes list
          for (size_t j = 0; j < nodes_len; j++) {
            //printf("/////////////// old E2 node list, idx %ld, nb_id %d, type %s //////////////\n", j,
            //       nodes.n[j].id.nb_id.nb_id, get_ngran_name(nodes.n[j].id.type));
            if (nodes.n[j].id.type == cur_type) new_type = 0;
            if (nodes.n[j].id.nb_id.nb_id == cur_nb_id) new_nb_id = 0;
          }
          if (new_type || new_nb_id) {
            printf("/////////////// send sub req to new E2 node, nb_id %d, type %s //////////////\n", cur_nodes.n[i].id.nb_id.nb_id, get_e2ap_ngran_name(cur_nodes.n[i].id.type));
            e2_node_connected_t* n = &cur_nodes.n[i];
            for (size_t m = 0; m < n->len_rf; m++)
              printf("Registered node %ld ran func id = %d \n ", i, n->ack_rf[m].id);

            n_handle = send_sub_req(n, args, kpm_handle, n_handle);
          }
        }
      }
      nodes_len = cur_nodes_len;
      nodes = e2_nodes_xapp_api();

      if (nodes_len == 0)
        n_handle = 0;
    }
    sleep(1);
  }

  printf("CTRL+C detect\n");
  // TODO: send subscription request delete
  for(size_t i = 0; i < n_handle; ++i)
    rm_report_sm_xapp_api(kpm_handle[i].u.handle);

  // free sm handel
  // TODO: free handle
  free(kpm_handle);

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
