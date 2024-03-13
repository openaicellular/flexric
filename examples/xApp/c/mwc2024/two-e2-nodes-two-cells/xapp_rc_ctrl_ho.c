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
#include <unistd.h>

// 3GPP 38.423
// 9.2.2.7
static uint64_t TARGET_NR_CGI;
static uint64_t SOURCE_NR_CGI;
static int RAN_UE_ID;

static
uint64_t get_nb_id(uint64_t nr_cgi){
  // Extract 36 bits from input
  uint64_t res = nr_cgi & ((1UL << 36) - 1);
  // nb_id default number of bits is 28 bits
  // Shift to right 36 - 28 = 8
  return res >> 8;
}

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
    char nr_cgi[10];
    sprintf(nr_cgi, "%ld", TARGET_NR_CGI);

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
    CHOICE_target_cell->ran_param_val.strct->sz_ran_param_struct = 1;
    CHOICE_target_cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
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
    byte_array_t nr_cgi_ba = cp_str_to_ba(nr_cgi);
    NR_cgi->ran_param_val.flag_false->bit_str_ran.buf = nr_cgi_ba.buf;
    NR_cgi->ran_param_val.flag_false->bit_str_ran.len = nr_cgi_ba.len;

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
        // optional: ran ue id
//        ue_id.gnb.ran_ue_id = NULL;
        ue_id.gnb.ran_ue_id = malloc(sizeof(uint64_t));
        *ue_id.gnb.ran_ue_id = RAN_UE_ID;
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

void start_control(e2_node_arr_xapp_t nodes){
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
  uint64_t global_nb_id = get_nb_id(SOURCE_NR_CGI);
  sm_ans_xapp_t ans = {.success = false};
  for(size_t i =0; i < nodes.len; ++i){
    // Only control one E2 Node
    if (nodes.n[i].id.nb_id.nb_id == global_nb_id)
      ans = control_sm_xapp_api(&nodes.n[i].id, SM_RC_ID, &rc_ctrl);
  }

  if (!ans.success)
    printf("[xApp]: Can not send control message with nb_id: %ld \n", global_nb_id);

  printf("[xApp]: Control Loop Latency: %ld us\n", time_now_us() - st);

  free_rc_ctrl_req_data(&rc_ctrl);
}

// RC CTRL -> End

int main(int argc, char *argv[])
{
    fr_args_t args = init_fr_args(argc, argv);
    defer({ free_fr_args(&args); });
    for (int i = argc - 1; i > 0; i--){
      if(i == argc - 1) // NR CGI
        // only read decimal value - base 10
        TARGET_NR_CGI = atoi(argv[i]);
      if(i == argc - 2) // RAN UE ID
        SOURCE_NR_CGI = atoi(argv[i]);
      if(i == argc - 3) // RAN UE ID
        RAN_UE_ID = atoi(argv[i]);
      if(i < argc - 3)
        break;
    }

    //Init the xApp
    init_xapp_api(&args);
    sleep(1);

    e2_node_arr_xapp_t nodes = e2_nodes_xapp_api();
    defer({ free_e2_node_arr_xapp(&nodes); });
    assert(nodes.len > 0);
    printf("Connected E2 nodes = %d\n", nodes.len);

    start_control(nodes);
    sleep(3);

    //Stop the xApp
    while(try_stop_xapp_api() == false)
        usleep(1000);

    printf("Test xApp run SUCCESSFULLY\n");

}

