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
void sm_cb_kpm(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0); 

  // Reading Indication Message Format 1
  kpm_ind_data_t const* ind = &rd->ind.kpm.ind;
  kpm_ric_ind_hdr_format_1_t const* hdr_frm_1 = &ind->hdr.kpm_ric_ind_hdr_format_1;
  kpm_ind_msg_format_1_t const* msg_frm_1 = &ind->msg.frm_1;


  {
    lock_guard(&mtx);

    static int counter = 1;

    int64_t now = time_now_us();
    printf("KPM ind_msg latency = %ld\n", now - hdr_frm_1->collectStartTime*1000000);  // xApp <-> E2 Node

    // Reported list of cell level measurements per granularity period
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
                printf("RRU.PrbTotDl = %d [PRBs]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
              }
              else if (strcmp(meas_info_name_str, "RRU.PrbTotUl") == 0)
              {
                printf("RRU.PrbTotUl = %d [PRBs]\n", msg_frm_1->meas_data_lst[j].meas_record_lst[z].int_val);
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
        printf("Measurement Record not reliable");        
      }
    }
    counter++;
  }
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

  dst.gran_period_ms = 1;

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

static
kpm_act_def_t gen_act_def_cell(const char** act)
{
  kpm_act_def_t dst = {0}; 

  dst.type = FORMAT_1_ACTION_DEFINITION; 
  dst.frm_1 = gen_act_def_frm_1_cell(act);
  
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

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  //Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_t nodes = e2_nodes_xapp_api();
  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

  sm_ans_xapp_t* h = calloc(nodes.len, sizeof(sm_ans_xapp_t)); 
  assert(h != NULL && "Memory exhausted");


  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);


  //////////// 
  // START KPM 
  //////////// 
  kpm_sub_data_t kpm_sub = {0};

  // KPM Event Trigger
  uint64_t period_ms = 1;
  kpm_sub.ev_trg_def = gen_ev_trig(period_ms);

  // KPM Action Definition
  kpm_sub.sz_ad = 1;
  kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
  assert(kpm_sub.ad != NULL && "Memory exhausted");

  // Generate Action for CELL LEVEL measurements
  const char *act_cell[] = {"RRU.PrbTotDl", "RRU.PrbTotUl", NULL};  // 3GPP TS 28.552
  *kpm_sub.ad = gen_act_def_cell(act_cell); 

  const int KPM_ran_function = 1;

  for(size_t i =0; i < nodes.len; ++i){ 
    h[i] = report_sm_xapp_api(&nodes.n[i].id, KPM_ran_function, &kpm_sub, sm_cb_kpm);
    assert(h[i].success == true);
  } 
  free_kpm_sub_data(&kpm_sub); 

  //////////// 
  // END KPM 
  //////////// 

  sleep(10);

  for(int i = 0; i < nodes.len; ++i){
  // Remove the handle previously returned
  rm_report_sm_xapp_api(h[i].u.handle);
  }

  if(nodes.len > 0){
    free(h);
  }

  //Stop the xApp
  while(try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY\n");
}
