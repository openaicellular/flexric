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
#include "../../../../src/sm/rc_sm/rc_sm_id.h"
#include "../../../../src/sm/kpm_sm/kpm_sm_id_wrapper.h"
//#include "../../../../src/sm/kpm_sm/kpm_sm_v03.00/ie/kpm_data_ie.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

static e2_node_arr_t nodes;
//size_t MAX_LST_NR_CELL = 25;
uint64_t LST_NR_CELL_ID[25] = {500, 501};

typedef struct lst_kpm_handle_t {
  sm_ans_xapp_t* kpm_handle;
  size_t n_handle;
} lst_kpm_handle_t;

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
        }

        // UE information
        // List of Neighbor cells
        // 8.1.1.17
        if(cur_ran_param->ran_param_id == 21528) {
          ran_param_list_t* ne_cell_lst = cur_ran_param->ran_param_val.lst;
          for (size_t cell_idx = 0; cell_idx < ne_cell_lst->sz_lst_ran_param; cell_idx++){
            lst_ran_param_t cur_cell = ne_cell_lst->lst_ran_param[cell_idx];
            // CHOICE Neighbor cell
            // 21530
            ran_param_struct_t* CHOICE_Neighbor_cell = cur_cell.ran_param_struct.ran_param_struct[0].ran_param_val.strct;
            // NR Cell
            // 8.1.1.1
            // Support only 5G cell
            assert(CHOICE_Neighbor_cell->sz_ran_param_struct == 1 && "Support only 5G cell");
            ran_param_struct_t* nr_cell = CHOICE_Neighbor_cell->ran_param_struct[0].ran_param_val.strct;
            // NR PCI
            // TS 38.473
            // 9.3.1.29
            int64_t nr_pci = nr_cell->ran_param_struct[0].ran_param_val.flag_false->int_ran;
            printf("[RC_SM] UE %zu - Neighbor cell id %zu with PCI %ld \n", i, cell_idx, nr_pci);
            LST_NR_CELL_ID[cell_idx] = nr_pci;
          }
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

////////////
// Get KPM Indication Messages -> start
////////////

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node){
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == KPM_STATS_V3_0);

  kpm_ind_data_t const* kpm = &rd->ind.kpm.ind;
  kpm_ric_ind_hdr_format_1_t const* hdr_frm_1 = &kpm->hdr.kpm_ric_ind_hdr_format_1;

  int64_t now = time_now_us();
  {
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
    if (kpm->msg.type == FORMAT_3_INDICATION_MESSAGE){
      kpm_ind_msg_format_3_t const* msg_frm_3 = &kpm->msg.frm_3;
      for (size_t i = 0; i < msg_frm_3->ue_meas_report_lst_len; i++){

        uint64_t* ran_ue_id = msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb.ran_ue_id;
        if (ran_ue_id != NULL){
          printf("[KPM-SM] UE %zu in cell(PCI) %lu\n", *ran_ue_id, LST_NR_CELL_ID[rand()%2]);
        }else{
          printf("[KPM-SM] No UE in cell(PCI) %lu\n", LST_NR_CELL_ID[rand()%2]);
        }
      }
    }
    counter++;
  }
}

////////////
// Get KPM Indication Messages -> end
////////////

// RC CTRL -> Start
static
e2sm_rc_ctrl_hdr_frmt_1_t gen_rc_ctrl_hdr_frmt_1(ue_id_e2sm_t ue_id, uint32_t ric_style_type, uint16_t ctrl_act_id)
{
    e2sm_rc_ctrl_hdr_frmt_1_t dst = {0};

    // 6.2.2.6
    dst.ue_id = cp_ue_id_e2sm(&ue_id);

    dst.ric_style_type = ric_style_type;
    dst.ctrl_act_id = ctrl_act_id;

    return dst;
}

static
e2sm_rc_ctrl_hdr_t gen_rc_ctrl_hdr(e2sm_rc_ctrl_hdr_e hdr_frmt, ue_id_e2sm_t ue_id, uint32_t ric_style_type, uint16_t ctrl_act_id)
{
    e2sm_rc_ctrl_hdr_t dst = {0};

    if (hdr_frmt == FORMAT_1_E2SM_RC_CTRL_HDR) {
        dst.format = FORMAT_1_E2SM_RC_CTRL_HDR;
        dst.frmt_1 = gen_rc_ctrl_hdr_frmt_1(ue_id, ric_style_type, ctrl_act_id);
    } else {
        assert(0!=0 && "not implemented the fill func for this ctrl hdr frmt");
    }

    return dst;
}

static
void gen_target_primary_cell_id(seq_ran_param_t* Target_primary_cell_id)
{
    // Target Primary Cell ID, STRUCTURE (Target Primary Cell ID)
    Target_primary_cell_id->ran_param_id = Target_primary_cell_id_8_4_4_1;
    Target_primary_cell_id->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    Target_primary_cell_id->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(Target_primary_cell_id->ran_param_val.strct != NULL && "Memory exhausted");
    Target_primary_cell_id->ran_param_val.strct->sz_ran_param_struct = 1;
    Target_primary_cell_id->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(Target_primary_cell_id->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // CHOICE Target Cell, STRUCTURE (Target Primary Cell ID -> CHOICE Target Cell )
    seq_ran_param_t* CHOICE_target_cell = &Target_primary_cell_id->ran_param_val.strct->ran_param_struct[0];
    CHOICE_target_cell->ran_param_id = CHOICE_target_cell_8_4_4_1;
    CHOICE_target_cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    CHOICE_target_cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(CHOICE_target_cell->ran_param_val.strct != NULL && "Memory exhausted");
    CHOICE_target_cell->ran_param_val.strct->sz_ran_param_struct = 2;
    CHOICE_target_cell->ran_param_val.strct->ran_param_struct = calloc(2, sizeof(seq_ran_param_t));
    assert(CHOICE_target_cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // NR Cell, STRUCTURE (CHOICE target cell -> NR Cell)
    seq_ran_param_t* NR_cell = &CHOICE_target_cell->ran_param_val.strct->ran_param_struct[0];
    NR_cell->ran_param_id = NR_cell_8_4_4_1;
    NR_cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    NR_cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(NR_cell->ran_param_val.strct != NULL && "Memory exhausted");
    NR_cell->ran_param_val.strct->sz_ran_param_struct = 1;
    NR_cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(NR_cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // NR CGI, ELEMENT (NR Cell -> NR CGI)
    seq_ran_param_t* NR_cgi = &NR_cell->ran_param_val.strct->ran_param_struct[0];
    NR_cgi->ran_param_id = NR_CGI_8_4_4_1;
    NR_cgi->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    NR_cgi->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
    assert(NR_cgi->ran_param_val.flag_false != NULL && "Memory exhausted");
    // NR CGI IE in TS 38.423 [15] Clause 9.2.2.7
    NR_cgi->ran_param_val.flag_false->type = BIT_STRING_RAN_PARAMETER_VALUE;
    byte_array_t nr_cgi_ba = cp_str_to_ba("30");
    NR_cgi->ran_param_val.flag_false->bit_str_ran.buf = nr_cgi_ba.buf;
    NR_cgi->ran_param_val.flag_false->bit_str_ran.len = nr_cgi_ba.len;

    // E-ULTRA Cell, STRUCTURE (CHOICE Target Cell -> E-ULTRA cell)
    seq_ran_param_t* E_ultra_cell = &CHOICE_target_cell->ran_param_val.strct->ran_param_struct[1];
    E_ultra_cell->ran_param_id = E_ULTRA_Cell_8_4_4_1;
    E_ultra_cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    E_ultra_cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(E_ultra_cell->ran_param_val.strct != NULL && "Memory exhausted");
    E_ultra_cell->ran_param_val.strct->sz_ran_param_struct = 1;
    E_ultra_cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(E_ultra_cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

    // E-ULTRA CGI, ELEMENT (E-ULTRA Cell -> E-ULTRA CGI)
    seq_ran_param_t* E_ultra_cgi = &E_ultra_cell->ran_param_val.strct->ran_param_struct[0];
    E_ultra_cgi->ran_param_id = E_ULTRA_CGI_8_4_4_1;
    E_ultra_cgi->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    E_ultra_cgi->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
    assert(E_ultra_cgi->ran_param_val.flag_false != NULL && "Memory exhausted");
    // E-UTRA CGI IE in TS 38.423 [15] Clause 9.2.2.8
    E_ultra_cgi->ran_param_val.flag_false->type = BIT_STRING_RAN_PARAMETER_VALUE;
    const char e_utra_cgi_val[] = "8"; // TODO
    byte_array_t e_ultra_cgi_ba = cp_str_to_ba(e_utra_cgi_val);
    E_ultra_cgi->ran_param_val.flag_false->bit_str_ran.buf = e_ultra_cgi_ba.buf;
    E_ultra_cgi->ran_param_val.flag_false->bit_str_ran.len = e_ultra_cgi_ba.len;

    return;
}

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1_hand_over()
{
    e2sm_rc_ctrl_msg_frmt_1_t dst = {0};

    // 8.4.4.1
    // Target Primary Cell ID, STRUCTURE (len 1)
    // > CHOICE Target Cell, STRUCTURE (len 2)
    // >> NR Cell, STRUCTURE (len 1)
    // >>> NR CGI, ELEMENT
    // >> E-ULTRA Cell, STRUCTURE (len 1)
    // >>> E-ULTRA CGI, ELEMENT

    dst.sz_ran_param = 1;
    dst.ran_param = calloc(1, sizeof(seq_ran_param_t));
    assert(dst.ran_param != NULL && "Memory exhausted");

    gen_target_primary_cell_id(&dst.ran_param[0]);
    // TODO: List of PDU sessions for handover
    // TODO: List of PRBs for handover
    // TODO: List of secondary cells to be setup
    return dst;
}

static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg(e2sm_rc_ctrl_msg_e msg_frmt)
{
    e2sm_rc_ctrl_msg_t dst = {0};

    if (msg_frmt == FORMAT_1_E2SM_RC_CTRL_MSG) {
        dst.format = msg_frmt;
        dst.frmt_1 = gen_rc_ctrl_msg_frmt_1_hand_over();
    } else {
        assert(0!=0 && "not implemented the fill func for this ctrl msg frmt");
    }

    return dst;
}


static
ue_id_e2sm_t gen_rc_ue_id(ue_id_e2sm_e type)
{
    ue_id_e2sm_t ue_id = {0};
    if (type == GNB_UE_ID_E2SM) {
        ue_id.type = GNB_UE_ID_E2SM;
        ue_id.gnb.ran_ue_id = malloc(sizeof(uint64_t));
        *ue_id.gnb.ran_ue_id = 1;
        ue_id.gnb.amf_ue_ngap_id = 0;
        ue_id.gnb.guami.plmn_id.mcc = 1;
        ue_id.gnb.guami.plmn_id.mnc = 1;
        ue_id.gnb.guami.plmn_id.mnc_digit_len = 2;
        ue_id.gnb.guami.amf_region_id = 0;
        ue_id.gnb.guami.amf_set_id = 0;
        ue_id.gnb.guami.amf_ptr = 0;
    } else {
        assert(0!=0 && "not supported UE ID type");
    }
    return ue_id;
}

void start_control(){
  // RC Control
  // CONTROL Service Style 3: Connected mode mobility control
  // Action ID 1: Handover Control
  // E2SM-RC Control Header Format 1
  // E2SM-RC Control Message Format 1

  rc_ctrl_req_data_t rc_ctrl = {0};
  ue_id_e2sm_t ue_id = gen_rc_ue_id(GNB_UE_ID_E2SM);

  rc_ctrl.hdr = gen_rc_ctrl_hdr(FORMAT_1_E2SM_RC_CTRL_HDR, ue_id, 3, Handover_control_7_6_4_1);
  rc_ctrl.msg = gen_rc_ctrl_msg(FORMAT_1_E2SM_RC_CTRL_MSG);

  int64_t st = time_now_us();
  for(size_t i =0; i < nodes.len; ++i){
    // TODO: Find a way to send nr_cgi and e_ultra_cgi
    control_sm_xapp_api(&nodes.n[i].id, SM_RC_ID, &rc_ctrl);
  }
  printf("[xApp]: Control Loop Latency: %ld us\n", time_now_us() - st);

  free_rc_ctrl_req_data(&rc_ctrl);
}

// RC CTRL -> End

// RC SUB -> start

static e2sm_rc_act_def_frmt_1_t gen_rc_act_def_frm_1(const act_name_id_t* action)
{
  e2sm_rc_act_def_frmt_1_t act_def_frm_1 = {0};

  size_t count = 0;
  while (action[count].name != NULL) {
    count++;
  }

  // Parameters to be Reported List
  // [1-65535]
  // 8.2.2
  act_def_frm_1.sz_param_report_def = count;
  act_def_frm_1.param_report_def = calloc(act_def_frm_1.sz_param_report_def, sizeof(param_report_def_t));
  assert(act_def_frm_1.param_report_def != NULL && "Memory exhausted");
  for(size_t i = 0; i < act_def_frm_1.sz_param_report_def; i++) {
    act_def_frm_1.param_report_def[i].ran_param_id = action[i].id;
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

static e2sm_rc_action_def_t gen_rc_act_def(const act_name_id_t* act, e2sm_rc_act_def_format_e act_frm)
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

      *rc_sub.ad = gen_rc_act_def((const act_name_id_t*)args.sub_oran_sm[j].actions, act_type);

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

  // No Label
  dst.label_info_lst[0].noLabel = calloc(1, sizeof(enum_value_e));
  assert(dst.label_info_lst[0].noLabel != NULL && "Memory exhausted");
  *dst.label_info_lst[0].noLabel = TRUE_ENUM_VALUE;

  return dst;
}

static
kpm_act_def_format_1_t gen_act_def_frmt_1(const sub_oran_sm_t action, uint32_t period_ms, size_t cell_id)
{
  kpm_act_def_format_1_t dst = {0};

  dst.gran_period_ms = period_ms;

  // [1, 65535]
  dst.meas_info_lst_len = action.act_len;
  dst.meas_info_lst = calloc(dst.meas_info_lst_len, sizeof(meas_info_format_1_lst_t));
  assert(dst.meas_info_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.meas_info_lst_len; i++) {
    dst.meas_info_lst[i] = gen_meas_info_format_1_lst(action.actions[i].name);
  }

  dst.cell_global_id = malloc(sizeof(cell_global_id_t));
  dst.cell_global_id->type = NR_CGI_RAT_TYPE;
  dst.cell_global_id->nr_cgi.nr_cell_id = LST_NR_CELL_ID[cell_id];

  return dst;
}

static
kpm_act_def_format_4_t gen_act_def_frmt_4(const sub_oran_sm_t action, uint32_t period_ms, size_t cell_idx)
{
  kpm_act_def_format_4_t dst = {0};

  dst.matching_cond_lst_len = 1;

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
  assert(dst.matching_cond_lst != NULL && "Memory exhausted");

  dst.matching_cond_lst[0].test_info_lst.test_cond_type = S_NSSAI_TEST_COND_TYPE;
  dst.matching_cond_lst[0].test_info_lst.S_NSSAI = TRUE_TEST_COND_TYPE;

  dst.matching_cond_lst[0].test_info_lst.test_cond = calloc(1, sizeof(test_cond_e));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond = EQUAL_TEST_COND;

  dst.matching_cond_lst[0].test_info_lst.test_cond_value = calloc(1, sizeof(test_cond_value_e));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value != NULL && "Memory exhausted");
  dst.matching_cond_lst[0].test_info_lst.test_cond_value->type =  INTEGER_TEST_COND_VALUE;
  dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value = malloc(sizeof(int64_t));
  assert(dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value != NULL && "Memory exhausted");
  *dst.matching_cond_lst[0].test_info_lst.test_cond_value->int_value = 1;

  //printf("[xApp]: Filter UEs by S-NSSAI criteria where SST = %lu\n", *dst.matching_cond_lst[0].test_info_lst.int_value);

  dst.action_def_format_1 = gen_act_def_frmt_1(action, period_ms, cell_idx);  // 8.2.1.2.1

  return dst;
}

static
kpm_act_def_t gen_act_def(const sub_oran_sm_t act, format_action_def_e act_frm, uint32_t period_ms, size_t cell_idx)
{
  kpm_act_def_t dst = {0};

  if (act_frm == FORMAT_1_ACTION_DEFINITION) {
    dst.type = FORMAT_1_ACTION_DEFINITION;
    dst.frm_1 = gen_act_def_frmt_1(act, period_ms, cell_idx);
  } else if (act_frm == FORMAT_4_ACTION_DEFINITION) {
    dst.type = FORMAT_4_ACTION_DEFINITION;
    dst.frm_4 = gen_act_def_frmt_4(act, period_ms, cell_idx);
  } else {
    assert(0!=0 && "not support action definition type");
  }

  return dst;
}

static
size_t send_sub_req_kpm(e2_node_connected_t* n, fr_args_t args, sm_ans_xapp_t *kpm_handle, size_t n_handle, size_t cell_idx)
{
  for (int32_t j = 0; j < args.sub_oran_sm_len; j++) {
    if (!strcasecmp(args.sub_oran_sm[j].name, "kpm")) {
      kpm_sub_data_t kpm_sub = {0};
      defer({free_kpm_sub_data(&kpm_sub);});

      // KPM Event Trigger
      uint64_t period_ms = args.sub_oran_sm[j].time;
      kpm_sub.ev_trg_def = gen_ev_trig(period_ms);
      printf("[xApp]: reporting period = %lu [ms]\n", period_ms);

      // KPM Action Definition
      kpm_sub.sz_ad = 1;
      kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
      assert(kpm_sub.ad != NULL && "Memory exhausted");

      format_action_def_e act_type;
      if(args.sub_oran_sm[j].format == 1) {
        act_type = FORMAT_1_ACTION_DEFINITION;
      } else if (args.sub_oran_sm[j].format == 4) {
        act_type = FORMAT_4_ACTION_DEFINITION;
      } else {
        assert(0 != 0 && "not supported action definition format");
      }

      *kpm_sub.ad = gen_act_def((const sub_oran_sm_t)args.sub_oran_sm[j], act_type, period_ms, cell_idx);

      kpm_handle[n_handle] = report_sm_xapp_api(&n->id, SM_KPM_ID, &kpm_sub, sm_cb_kpm);
      assert(kpm_handle[n_handle].success == true);
      n_handle += 1;
    }
  }
  return n_handle;
}

void start_rc_sub(fr_args_t args){
  int max_handle = 256;
  // RC indication
  sm_ans_xapp_t *rc_handle = NULL;
  if (max_handle > 0) {
    rc_handle = calloc(max_handle, sizeof(sm_ans_xapp_t *));
    assert(rc_handle != NULL);
  }

  size_t n_handle = 0;
  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    n_handle = send_sub_req_rc(n, args, rc_handle, n_handle);
  }

  sleep(2);

  for(size_t i = 0; i < n_handle; ++i){
    rm_report_sm_xapp_api(rc_handle[i].u.handle);
  }

  free(rc_handle);
}

void start_monitor_each_cell(fr_args_t args, size_t cell_id, int max_handle, lst_kpm_handle_t* lst_kpm_handle){
  // KPM indication
  sm_ans_xapp_t* cur_kpm_handle = lst_kpm_handle[cell_id].kpm_handle;

  if (max_handle > 0) {
    cur_kpm_handle = calloc(max_handle, sizeof(sm_ans_xapp_t *));
    assert(cur_kpm_handle != NULL);
  }

  lst_kpm_handle[cell_id].n_handle = 0;
  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_t* n = &nodes.n[i];
    send_sub_req_kpm(n, args, cur_kpm_handle, lst_kpm_handle[cell_id].n_handle, cell_id);
  }
}

int main(int argc, char *argv[])
{
    fr_args_t args = init_fr_args(argc, argv);
    defer({ free_fr_args(&args); });

    //Init the xApp
    init_xapp_api(&args);
    sleep(1);

    nodes = e2_nodes_xapp_api();
    defer({ free_e2_node_arr(&nodes); });
    assert(nodes.len > 0);
    printf("Connected E2 nodes = %d\n", nodes.len);

    start_rc_sub(args);

    start_control();

    sleep(2);


    int max_handle = 256;
    lst_kpm_handle_t *lst_kpm_handle =  calloc(2, sizeof(lst_kpm_handle_t *));

    for(size_t cell_idx; cell_idx < 2; cell_idx ++){
      start_monitor_each_cell(args, cell_idx, max_handle, lst_kpm_handle);
    }

    sleep(5);

    // TODO: Make delete request
//    for(size_t i = 0; i < 2 ; ++i){
//        rm_report_sm_xapp_api(lst_kpm_handle[i].kpm_handle[0].u.handle);

//      free(lst_kpm_handle[i].kpm_handle);
//    }

    //Stop the xApp
    while(try_stop_xapp_api() == false)
        usleep(1000);

    printf("Test xApp run SUCCESSFULLY\n");

}

