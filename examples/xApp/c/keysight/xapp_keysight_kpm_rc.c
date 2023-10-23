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
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

static
uint64_t period_ms = 300;

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
pthread_mutex_t mtx;

static
void display_kpm_ind_msg_frm_1_meas(kpm_ind_msg_format_1_t const *msg_frm_1)
{
  assert(msg_frm_1 != NULL);


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
            case INTEGER_MEAS_VALUE:
            {
              if (strcmp(meas_info_name_str, "RRU.PrbTotDl") == 0)
              {
                printf("Style 1\n");
                printf("RRU.PrbTotDl = %d [%%]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
              }
              else if (strcmp(meas_info_name_str, "RRU.PrbTotUl") == 0)
              {
                printf("Style 1\n");
                printf("RRU.PrbTotUl = %d [%%]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
              }
              else
              {
                printf("Measurement Name not yet implemented\n");
                //assert(false && "Measurement Name not yet implemented");
              }

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

static
void display_kpm_ind_msg_frm_2_meas(kpm_ind_msg_format_2_t const *msg_frm_2)
{
  assert(msg_frm_2 != NULL);

  for (size_t j = 0; j<msg_frm_2->meas_data_lst_len; j++)
    {
      printf("Style 2\n");

      for (size_t z = 0; z<msg_frm_2->meas_data_lst[j].meas_record_len; z++)
      {
        if (msg_frm_2->meas_info_cond_ue_lst_len > 0)
        {
          // Matching Condition
          for(size_t i = 0; i < msg_frm_2->meas_info_cond_ue_lst[z].matching_cond_lst_len; i++)
          {
            switch (msg_frm_2->meas_info_cond_ue_lst[z].matching_cond_lst[i].cond_type)
            {
            case TEST_INFO:
            {
              assert(msg_frm_2->meas_info_cond_ue_lst[z].matching_cond_lst[i].test_info_lst.test_cond_type == GBR_TEST_COND_TYPE);
              assert(*msg_frm_2->meas_info_cond_ue_lst[z].matching_cond_lst[i].test_info_lst.test_cond == LESSTHAN_TEST_COND);
              assert(msg_frm_2->meas_info_cond_ue_lst[z].matching_cond_lst[i].test_info_lst.test_cond_value->type == INTEGER_TEST_COND_VALUE);
              printf("Test Condition: GBR rate < %lu\n", *msg_frm_2->meas_info_cond_ue_lst[z].matching_cond_lst[i].test_info_lst.test_cond_value->int_value);
              
              break;
            }

            default:
              assert(false && "Not yet implemented");
            }
          }

          // List of Matched UEs
          if (msg_frm_2->meas_info_cond_ue_lst[z].ue_id_matched_lst != NULL)
          {
            for(size_t i = 0; i < msg_frm_2->meas_info_cond_ue_lst[z].ue_id_matched_lst_len; i++)
            {
              switch(msg_frm_2->meas_info_cond_ue_lst[z].ue_id_matched_lst[i].type)
              {
              case GNB_UE_ID_E2SM:
                printf("#%ld UE connected to gNB with amf_ue_ngap_id = %lu\n", i, msg_frm_2->meas_info_cond_ue_lst[z].ue_id_matched_lst[i].gnb.amf_ue_ngap_id);
                break;

              case GNB_DU_UE_ID_E2SM:
                printf("#%ld UE connected to gNB-DU with gnb_cu_ue_f1ap = %u\n", i, msg_frm_2->meas_info_cond_ue_lst[z].ue_id_matched_lst[i].gnb_du.gnb_cu_ue_f1ap);
                break;
              
              default:
                assert(false && "Not yet implemented UE ID type");
              }
            }
          }
          else
          {
            printf("No matched UEs with this test condition\n");
          }

          switch (msg_frm_2->meas_info_cond_ue_lst[z].meas_type.type)
          {
          case NAME_MEAS_TYPE:
          {
            // Get the Measurement Name
            char meas_info_name_str[msg_frm_2->meas_info_cond_ue_lst[z].meas_type.name.len + 1];
            memcpy(meas_info_name_str, msg_frm_2->meas_info_cond_ue_lst[z].meas_type.name.buf, msg_frm_2->meas_info_cond_ue_lst[z].meas_type.name.len);
            meas_info_name_str[msg_frm_2->meas_info_cond_ue_lst[z].meas_type.name.len] = '\0';

            // Get the value of the Measurement
            switch (msg_frm_2->meas_data_lst[j].meas_record_lst[z].value)
            {
            case INTEGER_MEAS_VALUE:
            {
              if (strcmp(meas_info_name_str, "DRB.UEThpDl") == 0)
              {
                printf("DRB.UEThpDl = %d [kb/s]\n", msg_frm_2->meas_data_lst[j].meas_record_lst[z].int_val);
              }
              else if (strcmp(meas_info_name_str, "DRB.UEThpUl") == 0)
              {
                printf("DRB.UEThpUl = %d [kb/s]\n", msg_frm_2->meas_data_lst[j].meas_record_lst[z].int_val);
              }
              else
              {
                printf("Measurement Name not yet implemented\n");
                //assert(false && "Measurement Name not yet implemented");
              }

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
      if (msg_frm_2->meas_data_lst[j].incomplete_flag && *msg_frm_2->meas_data_lst[j].incomplete_flag == TRUE_ENUM_VALUE)
        printf("Measurement Record not reliable\n");        
      }
    }
}

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  // Reading Indication Message Format 1
  kpm_ind_data_t const* ind = &rd->ind.kpm.ind;
  kpm_ric_ind_hdr_format_1_t const* hdr_frm_1 = &ind->hdr.kpm_ric_ind_hdr_format_1;

  static int counter = 1;

  int64_t now = time_now_us();
  printf("\n%7d KPM ind_msg latency = %lu [μs]\n", counter, now - hdr_frm_1->collectStartTime*1000000);  // xApp <-> E2 Node

  // Check the Indication Message Type
  switch (ind->msg.type)
  {
  case FORMAT_1_INDICATION_MESSAGE:
    display_kpm_ind_msg_frm_1_meas(&ind->msg.frm_1);
    break;

  case FORMAT_2_INDICATION_MESSAGE:
    display_kpm_ind_msg_frm_2_meas(&ind->msg.frm_2);
    break;

  
  default:
    assert(false && "Wrong Indication Message Received");
  }

  counter++;
  // {
  //   lock_guard(&mtx);

    
  // }
}


static
meas_info_format_1_lst_t gen_meas_info_frm_1_cell_lst(const char* act)
{
  meas_info_format_1_lst_t dst = {0};

  dst.meas_type.type = NAME_MEAS_TYPE;
  dst.meas_type.name = copy_str_to_ba(act);

  dst.label_info_lst_len = 1;
  dst.label_info_lst = calloc(1, sizeof(label_info_lst_t));
  assert(dst.label_info_lst != NULL && "Memory exhausted");
  dst.label_info_lst[0].noLabel = calloc(1, sizeof(enum_value_e));
  assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
  *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE;

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frm_1_cell(const char** act)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = period_ms;

  // [1, 65535]
  size_t count = 0;
  while (act[count] != NULL) {
    count++;
  }
  dst.meas_info_lst_len = count;
  dst.meas_info_lst = calloc(count, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_lst_len; i++){
    dst.meas_info_lst[i] = gen_meas_info_frm_1_cell_lst(act[i]);
  }

  return dst;
}



/* UE LEVEL */
static
meas_info_format_1_lst_t gen_meas_info_frm_1_ue_lst(const char* act)
{
  meas_info_format_1_lst_t dst = {0};

  dst.meas_type.type = NAME_MEAS_TYPE;
  dst.meas_type.name = copy_str_to_ba(act);

  dst.label_info_lst_len = 1;
  dst.label_info_lst = calloc(1, sizeof(label_info_lst_t));
  assert(dst.label_info_lst != NULL && "Memory exhausted");

  if (strcmp(act, "DRB.UEThpDl") == 0 || strcmp(act, "DRB.UEThpUl") == 0)
  {
    dst.label_info_lst[0].noLabel = calloc(1, sizeof(enum_value_e));
    assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
    *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE; 
  }
  else if (strcmp(act, "DRB.UEThpDl.QOS") == 0 || strcmp(act, "DRB.UEThpUl.QOS") == 0)
  {
    dst.label_info_lst[0].fiveQI = calloc(1, sizeof(uint8_t));
    assert(dst.label_info_lst[0].fiveQI != NULL && "Memory exhausted");
    *dst.label_info_lst[0].fiveQI = 6;
  }
  else if (strcmp(act, "DRB.UEThpDl.SNSSAI") == 0 || strcmp(act, "DRB.UEThpUl.SNSSAI") == 0)
  {
    dst.label_info_lst[0].sliceID = calloc(1, sizeof(s_nssai_e2sm_t));
    dst.label_info_lst[0].sliceID->sST = 2;
    dst.label_info_lst[0].sliceID->sD = calloc(1, sizeof(uint32_t));
    *dst.label_info_lst[0].sliceID->sD = 1000;
  }
  else
  {
    assert(false && "Not yet implemented");
  }

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frm_1_ue(const char** act)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = period_ms;

  // [1, 65535]
  size_t count = 0;
  while (act[count] != NULL) {
    count++;
  }
  dst.meas_info_lst_len = count;
  dst.meas_info_lst = calloc(count, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_lst_len; i++){
    dst.meas_info_lst[i] = gen_meas_info_frm_1_ue_lst(act[i]);
  }

  return dst;
}

static
guami_t gen_guami(void)
{
  guami_t guami = {0};

  // Mandatory
  // PLMN Identity 6.2.3.1
  guami.plmn_id = (e2sm_plmn_t) {.mcc = 226, .mnc = 04, .mnc_digit_len = 2};

  // Mandatory
  // AMF Region ID BIT STRING (SIZE(8))
  guami.amf_region_id = 1;

  // Mandatory
  //  AMF Set ID BIT STRING (SIZE(10))
  guami.amf_set_id = 1;

  // Mandatory
  // AMF Pointer BIT STRING (SIZE(6))
  guami.amf_ptr = 1;

  return guami;
}

static
gnb_e2sm_t gen_gnb_ue_id(void)
{
  gnb_e2sm_t gnb = {0};

  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  gnb.amf_ue_ngap_id = 101;

  // Mandatory
  // GUAMI 6.2.3.17 
  gnb.guami = gen_guami();

  return gnb;
}

static
ue_id_e2sm_t gen_ue_id(void)
{
  ue_id_e2sm_t ue_id = {0};

  ue_id.type = GNB_UE_ID_E2SM;
  ue_id.gnb = gen_gnb_ue_id();


  return ue_id;
}

static
kpm_act_def_format_2_t gen_act_def_frm_2_ue(const char** act)
{
  kpm_act_def_format_2_t dst = {0};

  dst.ue_id = gen_ue_id();

  dst.action_def_format_1 = gen_act_def_frm_1_ue(act);
  
  return dst;
}

static
matching_condition_format_3_lst_t gen_matching_cond_frm_3_lst(void)
{
  matching_condition_format_3_lst_t dst = {0};

  // Test information list as matching condition
  dst.cond_type = TEST_INFO;
  dst.test_info_lst.test_cond_type = GBR_TEST_COND_TYPE;
  dst.test_info_lst.GBR = TRUE_TEST_COND_TYPE;

  
  dst.test_info_lst.test_cond = calloc(1, sizeof(test_cond_e));
  assert(dst.test_info_lst.test_cond != NULL && "Memory exhausted");
  *dst.test_info_lst.test_cond = LESSTHAN_TEST_COND;

  dst.test_info_lst.test_cond_value = calloc(1, sizeof(test_cond_value_t));
  assert(dst.test_info_lst.test_cond_value != NULL && "Memory exhausted");
  dst.test_info_lst.test_cond_value->type = INTEGER_TEST_COND_VALUE;
  dst.test_info_lst.test_cond_value->int_value = calloc(1, sizeof(int64_t));
  assert(dst.test_info_lst.test_cond_value->int_value != NULL && "Memory exhausted");
  *dst.test_info_lst.test_cond_value->int_value = 10000000; // unit?


  return dst;
}

static
meas_info_format_3_lst_t gen_meas_info_format_3_lst(const char* act)
{
  meas_info_format_3_lst_t dst = {0};

  // Measurement Type
  dst.meas_type.type = NAME_MEAS_TYPE;
  dst.meas_type.name = copy_str_to_ba(act);

  // Matching Condition List [1, 32768]
  dst.matching_cond_lst_len = 1; // ADD ONE FOR LABEL AND THE OTHER ONE FOR TEST LIST
  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_3_lst_t));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.matching_cond_lst_len; i++){
    dst.matching_cond_lst[i] = gen_matching_cond_frm_3_lst();
  }

  // Bin Range Definition
  // Optional
  dst.bin_range_def = NULL;

  return dst;
}

static
kpm_act_def_format_3_t gen_act_def_frm_3_ue(const char** act)
{
  kpm_act_def_format_3_t dst = {0};

  // Measurement Information List [1, 65535]
  size_t count = 0;
  while (act[count] != NULL) {
    count++;
  }
  dst.meas_info_lst_len = count;
  dst.meas_info_lst = calloc(dst.meas_info_lst_len, sizeof(meas_info_format_3_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_lst_len; i++) {
    dst.meas_info_lst[i] = gen_meas_info_format_3_lst(act[i]);
  }

  // Granularity Period
  // Mandatory
  dst.gran_period_ms = period_ms;

  // Cell Global ID
  // Optional
  dst.cell_global_id = NULL;


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
kpm_sub_data_t gen_kpm_sub_style_1(void)
{
  kpm_sub_data_t kpm_sub = {0};

  // Generate Event Trigger
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

  // Generate Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
  assert(kpm_sub.ad != NULL && "Memory exhausted");

  const char *act_cell[] = {"RRU.PrbTotDl", "RRU.PrbTotUl", NULL};  // 3GPP TS 28.552
  kpm_sub.ad[0].type = FORMAT_1_ACTION_DEFINITION;
  kpm_sub.ad[0].frm_1 = gen_act_def_frm_1_cell(act_cell);

  return kpm_sub;
}

static
kpm_sub_data_t gen_kpm_sub_style_2(void)
{
  kpm_sub_data_t kpm_sub = {0};

  // Generate Event Trigger
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

  // Generate Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
  assert(kpm_sub.ad != NULL && "Memory exhausted");

  const char *act_ue[] = {"DRB.UEThpDl", "DRB.UEThpDl.QOS", "DRB.UEThpDl.SNSSAI", "DRB.UEThpUl", "DRB.UEThpUl.QOS", "DRB.UEThpUl.SNSSAI", NULL};  // 3GPP TS 28.552
  kpm_sub.ad[0].type = FORMAT_2_ACTION_DEFINITION;
  kpm_sub.ad[0].frm_2 = gen_act_def_frm_2_ue(act_ue);

  return kpm_sub;
}

static
kpm_sub_data_t gen_kpm_sub_style_3(void)
{
  kpm_sub_data_t kpm_sub = {0};

  // Generate Event Trigger
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

  // Generate Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
  assert(kpm_sub.ad != NULL && "Memory exhausted");

  const char *act_ue[] = {"DRB.UEThpDl", "DRB.UEThpUl", NULL};  // 3GPP TS 28.552
  kpm_sub.ad[0].type = FORMAT_3_ACTION_DEFINITION;
  kpm_sub.ad[0].frm_3 = gen_act_def_frm_3_ue(act_ue);

  return kpm_sub;
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr(&nodes); });
  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);

  // KPM handle
  sm_ans_xapp_t* kpm_handle = NULL;
  if(nodes.len > 0){
    kpm_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) ); 
    assert(kpm_handle != NULL && "Memory exhausted");
  }

  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    for (size_t j = 0; j < n->len_rf; j++)
      printf("Registered node ID %d ran func id = %d \n ", n->id.nb_id.nb_id, n->ack_rf[j].id);


  //////////// 
  // START KPM 
  //////////// 
  printf("[xApp]: reporting period = %lu [ms]\n", period_ms);


  // KPM SUBSCRIPTION FOR CELL LEVEL MEASUREMENTS Style 1
  kpm_sub_data_t kpm_sub_1 = gen_kpm_sub_style_1();
  defer({ free_kpm_sub_data(&kpm_sub_1); });


  // KPM SUBSCRIPTION FOR UE LEVEL MEASUREMENTS Style 2
  kpm_sub_data_t kpm_sub_2 = gen_kpm_sub_style_2();
  defer({ free_kpm_sub_data(&kpm_sub_2); });


  // KPM SUBSCRIPTION FOR UE LEVEL MEASUREMENTS Style 3
  kpm_sub_data_t kpm_sub_3 = gen_kpm_sub_style_3();
  defer({ free_kpm_sub_data(&kpm_sub_3); });


  const int KPM_ran_function = 1;

  kpm_handle[i] = report_sm_xapp_api(&nodes.n[i].id, KPM_ran_function, &kpm_sub_1, sm_cb_kpm);
  kpm_handle[i] = report_sm_xapp_api(&nodes.n[i].id, KPM_ran_function, &kpm_sub_2, sm_cb_kpm);
  kpm_handle[i] = report_sm_xapp_api(&nodes.n[i].id, KPM_ran_function, &kpm_sub_3, sm_cb_kpm);


  assert(kpm_handle[i].success == true);
  } 
  
  //////////// 
  // END KPM 
  //////////// 

  sleep(10);

  for(int i = 0; i < nodes.len; ++i){
  // Remove the handle previously returned
  rm_report_sm_xapp_api(kpm_handle[i].u.handle);
  }

  if(nodes.len > 0){
    free(kpm_handle);
  }

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
