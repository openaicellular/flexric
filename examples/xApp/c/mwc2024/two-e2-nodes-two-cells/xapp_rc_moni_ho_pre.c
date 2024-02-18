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

#include "../../../../../src/xApp/e42_xapp_api.h"
#include "../../../../../src/sm/rc_sm/ie/ir/ran_param_struct.h"
#include "../../../../../src/sm/rc_sm/ie/ir/ran_param_list.h"
#include "../../../../../src/util/time_now_us.h"
#include "../../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../../../../src/sm/rc_sm/rc_sm_id.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

uint64_t LST_NR_CELL_ID[2] = {0};

///////////
// Get RC Indication Messages -> begin
////////////

static void sm_cb_rc(sm_ag_if_rd_t const *rd, global_e2_node_id_t const* e2_node)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == RAN_CTRL_STATS_V1_03);
  (void) e2_node;

  // Reading Indication Message Format 2
  if (rd->ind.rc.ind.msg.format == FORMAT_2_E2SM_RC_IND_MSG) {
    e2sm_rc_ind_msg_frmt_2_t const *msg_frm_2 = &rd->ind.rc.ind.msg.frmt_2;

    printf("\n RC REPORT Style 2 - Call Process Outcome\n");

    // Sequence of UE Identifier
    //[1-65535]
    for (size_t i = 0; i < msg_frm_2->sz_seq_ue_id; i++)
    {
      seq_ue_id_t* cur_ue_id = &msg_frm_2->seq_ue_id[i];
      // UE ID
      // Mandatory
      // 9.3.10
      if (cur_ue_id->ue_id.type == GNB_UE_ID_E2SM) {
        if (cur_ue_id->ue_id.gnb.ran_ue_id != NULL)
          printf("[RC_SM] UE %zu - ran_ue_id = %p\n", i, (void*)cur_ue_id->ue_id.gnb.ran_ue_id);
        else
          printf("[RC_SM] UE %zu - ran_ue_id is NULL\n", i);
      } else {
        printf("[RC_SM] UE %zu Not yet implemented UE ID type \n", i);
        continue;
      }

      // Sequence of
      // RAN Parameter
      // [1- 65535]
      // 8.2.2
      for (size_t t = 0; t < cur_ue_id->sz_seq_ran_param; t++){
        seq_ran_param_t* cur_ran_param = &cur_ue_id->seq_ran_param[t];
        if(cur_ran_param->ran_param_id == 6){
          // Cell Global ID
          // 9.3.36
          // O-RAN.WG3.E2SM-R003
          // 6.2.2.5
          char* cell_global_id = copy_ba_to_str(&cur_ran_param->ran_param_val.flag_false->octet_str_ran);
          printf("[RC_SM] UE %zu - cell global id = %s \n", i, cell_global_id);
          sscanf(cell_global_id, "%ld", &LST_NR_CELL_ID[0]); // index 0 is global_cell_id
        }

      }
    }
  } else {
    printf("Not support RC format %d\n", rd->ind.rc.ind.msg.format + 1);
  }

}

////////////
// Get RC Indication Messages -> end
////////////


// RC SUB -> start

static e2sm_rc_act_def_frmt_1_t gen_rc_act_def_frm_1(const sub_oran_sm_t action)
{
  e2sm_rc_act_def_frmt_1_t act_def_frm_1 = {0};

  // Parameters to be Reported List
  // [1-65535]
  // 8.2.2
  act_def_frm_1.sz_param_report_def = action.act_len;
  act_def_frm_1.param_report_def = calloc(act_def_frm_1.sz_param_report_def, sizeof(param_report_def_t));
  assert(act_def_frm_1.param_report_def != NULL && "Memory exhausted");
  for(size_t i = 0; i < act_def_frm_1.sz_param_report_def; i++) {
    act_def_frm_1.param_report_def[i].ran_param_id = action.actions[i].id;
    // TODO: Add parameters report definition
  }

  return act_def_frm_1;
}

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

// RC SUB -> End

static e2sm_rc_action_def_t gen_rc_act_def(const sub_oran_sm_t act, e2sm_rc_act_def_format_e act_frm)
{
  e2sm_rc_action_def_t dst = {0};

  if (act_frm == FORMAT_1_E2SM_RC_ACT_DEF) {
    dst.format = FORMAT_1_E2SM_RC_ACT_DEF;
    dst.frmt_1 = gen_rc_act_def_frm_1(act);
  } else {
    assert(0!=0 && "not support action definition type");
  }

  return dst;
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
size_t send_sub_req_rc(e2_node_connected_t* n, fr_args_t args, sm_ans_xapp_t *rc_handle, size_t n_handle)
{
  for (int32_t j = 0; j < args.sub_oran_sm_len; j++) {
    if (!strcasecmp(args.sub_oran_sm[j].name, "rc")) {
      rc_sub_data_t rc_sub = {0};
      defer({ free_rc_sub_data(&rc_sub); });

      // RC Event Trigger
      rc_sub.et = gen_rc_ev_trigger(FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT);

      // RC Action Definition
      rc_sub.sz_ad = 1;
      rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
      assert(rc_sub.ad != NULL && "Memory exhausted");

      e2sm_rc_act_def_format_e act_type;
      if (args.sub_oran_sm[j].format == 1)
        act_type = FORMAT_1_E2SM_RC_ACT_DEF;
      else
        assert(-1!=0 && "not supported action definition format");
//      rc_sub.ad[0].ric_style_type = 2; // 7.4.1
//      rc_sub.ad[0].format = FORMAT_1_E2SM_RC_ACT_DEF;
//      rc_sub.ad[0].frmt_1 = gen_rc_act_def_frm_1_manually();

      *rc_sub.ad = gen_rc_act_def((const sub_oran_sm_t)args.sub_oran_sm[j], act_type);

      // RC HO only supports for e1ap_ngran_gNB
      if (n->id.type == e2ap_ngran_eNB || n->id.type == e2ap_ngran_gNB_CU || n->id.type == e2ap_ngran_gNB_DU)
        continue;
      if (strcasecmp(args.sub_oran_sm[j].ran_type, get_e2ap_ngran_name(n->id.type)))
        continue;

      rc_handle[n_handle] = report_sm_xapp_api(&n->id, SM_RC_ID, &rc_sub, sm_cb_rc);
      assert(rc_handle[n_handle].success == true);
      n_handle += 1;
    }
  }
  return n_handle;
}


void start_rc_sub(fr_args_t args, e2_node_arr_t nodes){
  int max_handle = 256;
  // RC indication
  sm_ans_xapp_t *rc_handle = NULL;
  rc_handle = calloc(max_handle, sizeof(sm_ans_xapp_t *));
  assert(rc_handle != NULL);

  size_t n_handle = 0;
  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    n_handle = send_sub_req_rc(n, args, rc_handle, n_handle);
  }

  sleep(2);

  for(size_t i = 0; i < nodes.len; ++i){
    rm_report_sm_xapp_api(rc_handle[i].u.handle);
  }

  free(rc_handle);
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

    start_rc_sub(args, nodes);
    sleep(3);

    //Stop the xApp
    while(try_stop_xapp_api() == false)
        usleep(1000);

    printf("Test xApp run SUCCESSFULLY\n");

}

