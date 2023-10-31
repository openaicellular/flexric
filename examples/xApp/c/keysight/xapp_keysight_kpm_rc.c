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
void sm_cb_rc(sm_ag_if_rd_t const* rd)
{

}

static
void sm_cb_kpm_1(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  // Reading Indication Message Format 1
  kpm_ind_msg_format_1_t const* msg_frm_1 = &rd->ind.kpm.ind.msg.frm_1;

  static int counter_1 = 1;
  printf("\n%7d KPM Style 1 - Cell-level measurements\nperiod = %lu [ms]\n", counter_1, period_ms);


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
              printf("RRU.PrbTotDl = %d [%%]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
            }
            else if (strcmp(meas_info_name_str, "RRU.PrbTotUl") == 0)
            {
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
  counter_1++;
}

static
void sm_cb_kpm_2(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  // Reading Indication Message Format 1
  kpm_ind_msg_format_1_t const* msg_frm_1 = &rd->ind.kpm.ind.msg.frm_1;

  static int counter_2 = 1;
  printf("\n%7d KPM Style 2 - Single UE-level measurements\nperiod = %lu [ms]\n", counter_2, period_ms);


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
            if (strcmp(meas_info_name_str, "DRB.UEThpDl") == 0)
            {
              printf("DRB.UEThpDl = %d [kb/s]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
            }
            else if (strcmp(meas_info_name_str, "DRB.UEThpDl.QOS") == 0)
            {
              printf("DRB.UEThpDl.QOS = %d [kb/s], with 5QI = %hhu\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val, *msg_frm_1->meas_info_lst[z].label_info_lst[0].fiveQI);
            }
            else if (strcmp(meas_info_name_str, "DRB.UEThpDl.SNSSAI") == 0)
            {
              printf("DRB.UEThpDl.SNSSAI = %d [kb/s], with sST = %hhu and sD = %u\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val, msg_frm_1->meas_info_lst[z].label_info_lst[0].sliceID->sST, *msg_frm_1->meas_info_lst[z].label_info_lst[0].sliceID->sD);
            }
            else if (strcmp(meas_info_name_str, "DRB.UEThpUl") == 0)
            {
              printf("DRB.UEThpUl = %d [kb/s]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
            }
            else if (strcmp(meas_info_name_str, "DRB.UEThpUl.QOS") == 0)
            {
              printf("DRB.UEThpUl.QOS = %d [kb/s], with 5QI = %hhu\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val, *msg_frm_1->meas_info_lst[z].label_info_lst[0].fiveQI);
            }
            else if (strcmp(meas_info_name_str, "DRB.UEThpUl.SNSSAI") == 0)
            {
              printf("DRB.UEThpUl.SNSSAI = %d [kb/s], with sST = %hhu and sD = %u\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val, msg_frm_1->meas_info_lst[z].label_info_lst[0].sliceID->sST, *msg_frm_1->meas_info_lst[z].label_info_lst[0].sliceID->sD);
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
  counter_2++;
}

static
void sm_cb_kpm_3(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  // Reading Indication Message Format 2
  kpm_ind_msg_format_2_t const* msg_frm_2 = &rd->ind.kpm.ind.msg.frm_2;

  static int counter_3 = 1;
  printf("\n%7d KPM Style 3 - Condition based, UE-level measurements\nperiod = %lu [ms]\n", counter_3, period_ms);

  assert(msg_frm_2->meas_data_lst_len == 1 && "Check the granularity period, should be equal to reporting period");


  for (size_t i = 0; i < msg_frm_2->meas_info_cond_ue_lst_len; i++)  // equal to 2, UL/DL thr
  {
    // Matching Condition
    for(size_t j = 0; j < msg_frm_2->meas_info_cond_ue_lst[i].matching_cond_lst_len; j++)
    {
      switch (msg_frm_2->meas_info_cond_ue_lst[i].matching_cond_lst[j].cond_type)
      {
      case TEST_INFO:
      {
        assert(msg_frm_2->meas_info_cond_ue_lst[i].matching_cond_lst[j].test_info_lst.test_cond_type == GBR_TEST_COND_TYPE);
        assert(*msg_frm_2->meas_info_cond_ue_lst[i].matching_cond_lst[j].test_info_lst.test_cond == LESSTHAN_TEST_COND);
        assert(msg_frm_2->meas_info_cond_ue_lst[i].matching_cond_lst[j].test_info_lst.test_cond_value->type == INTEGER_TEST_COND_VALUE);
        printf("Test Condition: GBR rate < %lu\n", *msg_frm_2->meas_info_cond_ue_lst[i].matching_cond_lst[j].test_info_lst.test_cond_value->int_value);
            
        break;
      }

      default:
        assert(false && "Not yet implemented");
      }
    }

    // List of Matched UEs
    if (msg_frm_2->meas_info_cond_ue_lst[i].ue_id_matched_lst != NULL)
    {
      for(size_t j = 0; j < msg_frm_2->meas_info_cond_ue_lst[i].ue_id_matched_lst_len; j++)
      {
        switch(msg_frm_2->meas_info_cond_ue_lst[i].ue_id_matched_lst[j].type)
        {
        case GNB_UE_ID_E2SM:
          printf("#%ld UE connected to gNB with amf_ue_ngap_id = %lu\n", j+1, msg_frm_2->meas_info_cond_ue_lst[i].ue_id_matched_lst[j].gnb.amf_ue_ngap_id);
          break;

        case GNB_DU_UE_ID_E2SM:
          printf("#%ld UE connected to gNB-DU with gnb_cu_ue_f1ap = %u\n", j+1, msg_frm_2->meas_info_cond_ue_lst[i].ue_id_matched_lst[j].gnb_du.gnb_cu_ue_f1ap);
          break;
              
        default:
          assert(false && "Not yet implemented UE ID type");
        }

        switch (msg_frm_2->meas_info_cond_ue_lst[i].meas_type.type)
        {
        case NAME_MEAS_TYPE:
        {
          // Get the Measurement Name
          char meas_info_name_str[msg_frm_2->meas_info_cond_ue_lst[i].meas_type.name.len + 1];
          memcpy(meas_info_name_str, msg_frm_2->meas_info_cond_ue_lst[i].meas_type.name.buf, msg_frm_2->meas_info_cond_ue_lst[i].meas_type.name.len);
          meas_info_name_str[msg_frm_2->meas_info_cond_ue_lst[i].meas_type.name.len] = '\0';


          // Get the value of the Measurement
          switch (msg_frm_2->meas_data_lst[0].meas_record_lst[j].value)
          {
          case INTEGER_MEAS_VALUE:
          {
            if (strcmp(meas_info_name_str, "DRB.UEThpDl") == 0)
            {
              printf("DRB.UEThpDl = %d [kb/s]\n", msg_frm_2->meas_data_lst[0].meas_record_lst[j].int_val);
            }
            else if (strcmp(meas_info_name_str, "DRB.UEThpUl") == 0)
            {
              printf("DRB.UEThpUl = %d [kb/s]\n", msg_frm_2->meas_data_lst[0].meas_record_lst[j+msg_frm_2->meas_info_cond_ue_lst[i].ue_id_matched_lst_len].int_val);
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
    }
    else
    {
      printf("No matched UEs with this test condition\n");
    }

  }

  if (msg_frm_2->meas_data_lst[0].incomplete_flag && *msg_frm_2->meas_data_lst[0].incomplete_flag == TRUE_ENUM_VALUE)
    printf("Measurement Record not reliable\n");


  counter_3++;
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
    *dst.label_info_lst[0].fiveQI = 4;
  }
  else if (strcmp(act, "DRB.UEThpDl.SNSSAI") == 0 || strcmp(act, "DRB.UEThpUl.SNSSAI") == 0)
  {
    dst.label_info_lst[0].sliceID = calloc(1, sizeof(s_nssai_e2sm_t));
    dst.label_info_lst[0].sliceID->sST = 1;
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
gnb_du_e2sm_t gen_gnb_du_ue_id(void)
{
  gnb_du_e2sm_t gnb_du = {0};

  // gNB-CU UE F1AP ID
  // Mandatory
  // 6.2.3.21
  gnb_du.gnb_cu_ue_f1ap = 201;

  // RAN UE ID
  // Optional
  // 6.2.3.25
  gnb_du.ran_ue_id = NULL;
  

  return gnb_du;
}

static
ue_id_e2sm_t gen_ue_id(ngran_node_t const* ngran_type)
{
  ue_id_e2sm_t ue_id = {0};

  switch (*ngran_type)
  {
  case ngran_gNB:
    ue_id.type = GNB_UE_ID_E2SM;
    ue_id.gnb = gen_gnb_ue_id();
    break;
  
  case ngran_gNB_DU:
    ue_id.type = GNB_DU_UE_ID_E2SM;
    ue_id.gnb_du = gen_gnb_du_ue_id();
    break;

  default:
    assert(false && "Unknown NG-RAN Type");
  }
  
  return ue_id;
}

static
kpm_act_def_format_2_t gen_act_def_frm_2_ue(const char** act, ngran_node_t const* ngran_type)
{
  kpm_act_def_format_2_t dst = {0};

  dst.ue_id = gen_ue_id(ngran_type);

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
  *dst.test_info_lst.test_cond_value->int_value = 10000; // [kb/s]


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
kpm_sub_data_t gen_kpm_sub_style_2(ngran_node_t const* ngran_type)
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
  kpm_sub.ad[0].frm_2 = gen_act_def_frm_2_ue(act_ue, ngran_type);

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

//////////// 
// Reserved for RC SM -> begin 
////////////

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
e2sm_rc_ctrl_hdr_frmt_1_t gen_rc_ctrl_hdr_frmt_1(ngran_node_t const* ngran_type)
{
  e2sm_rc_ctrl_hdr_frmt_1_t dst = {0};

  // UE ID
  // Mandatory
  // 9.3.10
  dst.ue_id = gen_ue_id(ngran_type);

  // CONTROL Service Style 1: Radio Bearer Control
  dst.ric_style_type = 1;

  // Control Action ID: DRB QoS Configuration
  // Mandatory
  // 9.3.6
  dst.ctrl_act_id = DRB_QoS_Configuration_7_6_2_1;

  // RIC Control decision
  // Optional - included only if this action is sent as response to Insert Indication
  dst.ric_ctrl_decision = NULL;

  return dst;
}

static
e2sm_rc_ctrl_hdr_t gen_rc_ctrl_hdr(ngran_node_t const* ngran_type)
{
  e2sm_rc_ctrl_hdr_t dst = {0};
  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_HDR;
  dst.frmt_1 = gen_rc_ctrl_hdr_frmt_1(ngran_type);
  return dst;
}

typedef enum {
  DRB_ID_8_4_2_1 = 1,
  FIVEQI_8_4_2_1 = 2,
  PACKET_DELAY_BUDGET_8_4_2_1 = 3,
  PACKET_ERROR_RATE_8_4_2_1 = 4,
  SCALAR_FACTOR_8_4_2_1 = 5,
  EXPONENT_FACTOR_8_4_2_1 = 6,
  NG_RAN_DRB_ALLOC_AND_RETENTION_PRIORITY_8_4_2_1 = 7,
  PRIORITY_LEVEL_8_4_2_1 = 8,
  PRE_EMPTION_CAPABILITY_8_4_2_1 = 9,
  PRE_EMPTION_VULNERABILITY_8_4_2_1 = 10,
  PRIORITY_LEVEL_OF_MAPPED_QOS_FLOWS_8_4_2_1 = 11,
  QOS_PARAMETERS_FOR_GBR_FLOWS_IN_NG_RAN_BEARER_8_4_2_1 = 12,
  MAXIMUM_FLOW_BIT_RATE_DOWNLINK_8_4_2_1 = 13,
  GUARANTEED_FLOW_BIT_RATE_DOWNLINK_8_4_2_1 = 14,
  MAXIMUM_PACKET_LOSS_RATE_DOWNLINK_8_4_2_1 = 15,
  MAXIMUM_FLOW_BIT_RATE_UPLINK_8_4_2_1 = 16,
  GUARANTEED_FLOW_BIT_RATE_UPLINK_8_4_2_1 = 17,
  MAXIMUM_PACKET_LOSS_RATE_UPLINK_8_4_2_1 = 18,
  QOS_MONITORING_ENABLE_REQUEST_8_4_2_1 = 19,
  QOS_MONITORING_REPORTING_FREQUENCY_8_4_2_1 = 20,
  QOS_MONITORING_DISABLED_8_4_2_1 = 21,
  REFLECTIVE_QOS_MAPPING_8_4_2_1 = 22,
} drb_qos_conf_e;

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1(void)
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0};

  // 8.4.2.1 DRB QoS Configuration
  dst.sz_ran_param = 3;
  dst.ran_param = calloc(dst.sz_ran_param, sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "Memory exhausted");

  // DRB ID RAN Parameter
  dst.ran_param[0].ran_param_id = DRB_ID_8_4_2_1;
  dst.ran_param[0].ran_param_val.type = ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[0].ran_param_val.flag_true = calloc(1, sizeof(ran_parameter_value_t)) ;
  assert(dst.ran_param[0].ran_param_val.flag_true != NULL && "Memory exhausted");
  // Let's suppose that it is the DRB 1
  dst.ran_param[0].ran_param_val.flag_true->type = INTEGER_RAN_PARAMETER_VALUE; 
  dst.ran_param[0].ran_param_val.flag_true->int_ran = 1;


  // Priority level of mapped QoS flows RAN Parameter
  dst.ran_param[1].ran_param_id = PRIORITY_LEVEL_OF_MAPPED_QOS_FLOWS_8_4_2_1;
  dst.ran_param[1].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[1].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(dst.ran_param[1].ran_param_val.flag_false != NULL && "Memory exhausted");
  // Let's suppose rnd value
  dst.ran_param[1].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  dst.ran_param[1].ran_param_val.flag_false->int_ran = 10;

  // QoS parameters for GBR flows in NG-RAN Bearer RAN Parameter
  dst.ran_param[2].ran_param_id = QOS_PARAMETERS_FOR_GBR_FLOWS_IN_NG_RAN_BEARER_8_4_2_1;
  dst.ran_param[2].ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  dst.ran_param[2].ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(dst.ran_param[2].ran_param_val.strct != NULL && "Memory exhausted");
  ran_param_struct_t* rps = dst.ran_param[2].ran_param_val.strct;

  rps->sz_ran_param_struct = 4;
  rps->ran_param_struct = calloc(rps->sz_ran_param_struct, sizeof(seq_ran_param_t));
  assert(rps->ran_param_struct != NULL && "Memory exhausted");

  // Maximum Flow Bit Rate DL RAN Parameter
  rps->ran_param_struct[0].ran_param_id = MAXIMUM_FLOW_BIT_RATE_DOWNLINK_8_4_2_1;
  rps->ran_param_struct[0].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  rps->ran_param_struct[0].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps->ran_param_struct[0].ran_param_val.flag_false != NULL && "Memory exhausted");
  // Let's suppose rnd value
  rps->ran_param_struct[0].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps->ran_param_struct[0].ran_param_val.flag_false->int_ran = 5000;

  // Maximum Flow Bit Rate UL RAN Parameter
  rps->ran_param_struct[1].ran_param_id = MAXIMUM_FLOW_BIT_RATE_UPLINK_8_4_2_1;
  rps->ran_param_struct[1].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  rps->ran_param_struct[1].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps->ran_param_struct[1].ran_param_val.flag_false != NULL && "Memory exhausted");
  // Let's suppose rnd value
  rps->ran_param_struct[1].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps->ran_param_struct[1].ran_param_val.flag_false->int_ran = 5000;

  // Guaranteed Flow Bit Rate DL RAN Parameter
  rps->ran_param_struct[2].ran_param_id = GUARANTEED_FLOW_BIT_RATE_DOWNLINK_8_4_2_1;
  rps->ran_param_struct[2].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  rps->ran_param_struct[2].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps->ran_param_struct[2].ran_param_val.flag_false != NULL && "Memory exhausted");
  // Let's suppose rnd value
  rps->ran_param_struct[2].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps->ran_param_struct[2].ran_param_val.flag_false->int_ran = 5000;

  // Guaranteed Flow Bit Rate UL RAN Parameter
  rps->ran_param_struct[3].ran_param_id = GUARANTEED_FLOW_BIT_RATE_UPLINK_8_4_2_1;
  rps->ran_param_struct[3].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  rps->ran_param_struct[3].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(rps->ran_param_struct[3].ran_param_val.flag_false != NULL && "Memory exhausted");
  // Let's suppose rnd value
  rps->ran_param_struct[3].ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  rps->ran_param_struct[3].ran_param_val.flag_false->int_ran = 5000;

  return dst;
}

static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg(void)
{
  e2sm_rc_ctrl_msg_t dst = {0}; 

  // Radio Bearer Control
  dst.format = FORMAT_1_E2SM_RC_CTRL_MSG;
  dst.frmt_1 = gen_rc_ctrl_msg_frmt_1();

  return dst;
}

static
e2sm_rc_ev_trg_frmt_2_t gen_rc_event_trigger_frm_2(void)
{
  e2sm_rc_ev_trg_frmt_2_t ev_trigger = {0};

  //  Call Process Type ID
  //  Mandatory
  //  9.3.15
  ev_trigger.call_proc_type_id = 2;  // Bearer Context Management

  // Call Breakpoint ID
  // Mandatory
  // 9.3.49
  ev_trigger.call_break_id = 1;  // Bearer Context Setup

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

typedef enum {
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

static
e2sm_rc_act_def_frmt_1_t gen_rc_act_def_frm_1(void)
{
  e2sm_rc_act_def_frmt_1_t act_def_frm_1 = {0};

  // Parameters to be Reported List
  // [1-65535]
  // 8.2.2
  act_def_frm_1.sz_param_report_def = 2;
  act_def_frm_1.param_report_def = calloc(act_def_frm_1.sz_param_report_def, sizeof(param_report_def_t));
  assert(act_def_frm_1.param_report_def != NULL && "Memory exhausted");

  // Current UE ID RAN Parameter
  act_def_frm_1.param_report_def[0].ran_param_id = CURRENT_UE_ID_8_2_2;
  act_def_frm_1.param_report_def[0].ran_param_def = NULL;  // current ue id is element, optional

  // UE information RAN Parameter
  act_def_frm_1.param_report_def[1].ran_param_id = UE_INFORMATION_8_2_2;
  act_def_frm_1.param_report_def[1].ran_param_def = calloc(1, sizeof(ran_param_def_t));
  assert(act_def_frm_1.param_report_def[1].ran_param_def != NULL && "Memory exhausted");
  // fill UE information
  act_def_frm_1.param_report_def[1].ran_param_def->type = LIST_RAN_PARAMETER_DEF_TYPE;
  act_def_frm_1.param_report_def[1].ran_param_def->lst = calloc(1, sizeof(ran_param_type_t));
  assert(act_def_frm_1.param_report_def[1].ran_param_def->lst != NULL && "Memory exhausted");

  act_def_frm_1.param_report_def[1].ran_param_def->lst->sz_ran_param = 1;
  act_def_frm_1.param_report_def[1].ran_param_def->lst->ran_param = calloc(act_def_frm_1.param_report_def[1].ran_param_def->lst->sz_ran_param, sizeof(ran_param_lst_struct_t));
  assert(act_def_frm_1.param_report_def[1].ran_param_def->lst->ran_param != NULL && "Memory exhausted");

  act_def_frm_1.param_report_def[1].ran_param_def->lst->ran_param[0].ran_param_id = LIST_OF_PDU_SESSIONS_8_1_1_17;
  act_def_frm_1.param_report_def[1].ran_param_def->lst->ran_param[0].ran_param_name.len = strlen("List of PDU Sessions");
  act_def_frm_1.param_report_def[1].ran_param_def->lst->ran_param[0].ran_param_name.buf = calloc(strlen("List of PDU Sessions") + 1, sizeof(char));
  memcpy(act_def_frm_1.param_report_def[1].ran_param_def->lst->ran_param[0].ran_param_name.buf, "List of PDU Sessions", strlen("List of PDU Sessions"));



  return act_def_frm_1;
}

static
rc_sub_data_t gen_rc_sub_style_2(void)
{
  rc_sub_data_t rc_sub = {0};

  // Generate Event Trigger
  // 9.2.1.1.2
  rc_sub.et.format = FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT;
  rc_sub.et.frmt_2 = gen_rc_event_trigger_frm_2();

  // Generate Action Definition
  // 9.2.1.2.1
  rc_sub.sz_ad = 1;
  rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
  assert(rc_sub.ad != NULL && "Memory exhausted");

  rc_sub.ad[0].ric_style_type = 2;  // 7.4.1

  rc_sub.ad[0].format = FORMAT_1_E2SM_RC_ACT_DEF;
  rc_sub.ad[0].frmt_1 = gen_rc_act_def_frm_1();

  return rc_sub;
}


//////////// 
// Reserved for RC SM -> end 
////////////

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

  // // KPM handle
  // sm_ans_xapp_t* kpm_handle = NULL;
  // if(nodes.len > 0){
  //   kpm_handle = calloc( nodes.len, sizeof(sm_ans_xapp_t) ); 
  //   assert(kpm_handle != NULL && "Memory exhausted");
  // }

  // for (int i = 0; i < nodes.len; i++) {
  //   e2_node_connected_t* n = &nodes.n[i];
  //   for (size_t j = 0; j < n->len_rf; j++)
  //     printf("Registered node ID %d ran func id = %d \n ", n->id.nb_id.nb_id, n->ack_rf[j].id);


  // //////////// 
  // // START KPM 
  // //////////// 
  // printf("[xApp]: reporting period = %lu [ms]\n", period_ms);

  // // if (n->id.cu_du_id != NULL)
  // // {
  // //   printf("cu_du_id is %lu\n", *n->id.cu_du_id);
  // // }

  // if ((n->id.nb_id.nb_id == 1 && n->id.type == ngran_gNB) || n->id.type == ngran_gNB_DU) {
  // // // KPM SUBSCRIPTION FOR CELL LEVEL MEASUREMENTS Style 1
  // // kpm_sub_data_t kpm_sub_1 = gen_kpm_sub_style_1();
  // // defer({ free_kpm_sub_data(&kpm_sub_1); });


  // // // KPM SUBSCRIPTION FOR UE LEVEL MEASUREMENTS Style 2
  // // kpm_sub_data_t kpm_sub_2 = gen_kpm_sub_style_2(&n->id.type);
  // // defer({ free_kpm_sub_data(&kpm_sub_2); });


  // // KPM SUBSCRIPTION FOR UE LEVEL MEASUREMENTS Style 3
  // kpm_sub_data_t kpm_sub_3 = gen_kpm_sub_style_3();
  // defer({ free_kpm_sub_data(&kpm_sub_3); });


  // const int KPM_ran_function = 1;

  // // kpm_handle[i] = report_sm_xapp_api(&n->id, KPM_ran_function, &kpm_sub_1, sm_cb_kpm_1);
  // // kpm_handle[i] = report_sm_xapp_api(&n->id, KPM_ran_function, &kpm_sub_2, sm_cb_kpm_2);
  // kpm_handle[i] = report_sm_xapp_api(&n->id, KPM_ran_function, &kpm_sub_3, sm_cb_kpm_3);
  

  // assert(kpm_handle[i].success == true);
  // } 
  // }
  // //////////// 
  // // END KPM 
  // //////////// 

  // sleep(10);

  // for(int i = 0; i < nodes.len; ++i){
  // // Remove the handle previously returned
  // rm_report_sm_xapp_api(kpm_handle[i].u.handle);
  // }

  // if(nodes.len > 0){
  //   free(kpm_handle);
  // }

  //////////// 
  // START RC 
  //////////// 

  const int RC_ran_function = 3;

  for(size_t i =0; i < nodes.len; ++i){
    if(nodes.n[i].id.type == ngran_gNB) {
  // RC REPORT Service Style 2: Call Process Outcome
  rc_sub_data_t rc_sub = gen_rc_sub_style_2();
  defer({free_rc_sub_data(&rc_sub);});

  report_sm_xapp_api(&nodes.n[i].id, RC_ran_function, &rc_sub, sm_cb_rc);
  sleep(10);
    }
  }
  sleep(10);
  // // RC CONTROL Service Style 1
  // rc_ctrl_req_data_t rc_ctrl = {0};
  // defer({ free_rc_ctrl_req_data(&rc_ctrl); });

  



  // for(size_t i =0; i < nodes.len; ++i){
  //   if (nodes.n[i].id.type == ngran_gNB) {
  //     rc_ctrl.hdr = gen_rc_ctrl_hdr(&nodes.n[i].id.type);
  //     rc_ctrl.msg = gen_rc_ctrl_msg();
  //   control_sm_xapp_api(&nodes.n[i].id, RC_ran_function, &rc_ctrl);
  //   printf("RC Control message Style 1 \"DRB QoS Configuration\" sent\n");
  // }
  // }

  //////////// 
  // END RC 
  ////////////


  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
