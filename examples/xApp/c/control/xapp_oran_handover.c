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
#include "../../../../src/sm/kpm_sm/kpm_data_ie_wrapper.h"
//#include "../../../../src/sm/kpm_sm/kpm_sm_v03.00/ie/kpm_data_ie.h"
#include "../../../../src/sm/kpm_sm/kpm_sm_id_wrapper.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

static e2_node_arr_t nodes;
bool stop_flag = false;
bool triggerCondition = false;
nr_cgi_t nr_cgi;

static void sigint_handler(int sig)
{
    printf("signal %d received !\n", sig);
    stop_flag = true;
}

static
pthread_mutex_t mtx;

static
ue_id_e2sm_t gen_rc_ue_id(ue_id_e2sm_e type)
{
    ue_id_e2sm_t ue_id = {0};
    if (type == GNB_UE_ID_E2SM) {
        ue_id.type = GNB_UE_ID_E2SM;
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
    char nr_cgi_val[30];
    sprintf(nr_cgi_val, "%lu", nr_cgi.nr_cell_id);
    byte_array_t nr_cgi_ba = cp_str_to_ba(nr_cgi_val);
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
    // > RAN UE Id, ELEMENT // TODO: Not follow to standard
    // > CHOICE Target Cell, STRUCTURE (len 2)
    // >> NR Cell, STRUCTURE (len 1)
    // >>> NR CGI, ELEMENT
    // >> E-ULTRA Cell, STRUCTURE (len 1)
    // >>> E-ULTRA CGI, ELEMENT

    dst.sz_ran_param = 2;
    dst.ran_param = calloc(2, sizeof(seq_ran_param_t));
    assert(dst.ran_param != NULL && "Memory exhausted");

    seq_ran_param_t* amr_ran_ue_id = &dst.ran_param[0];
    amr_ran_ue_id->ran_param_id = Amarisoft_ran_ue_id;
    amr_ran_ue_id->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    amr_ran_ue_id->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
    assert(amr_ran_ue_id->ran_param_val.flag_false != NULL && "Memory exhausted");
    amr_ran_ue_id->ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
    amr_ran_ue_id->ran_param_val.flag_false->int_ran = 11; // TODO: Fix this number

    gen_target_primary_cell_id(&dst.ran_param[1]);
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
    *dst.matching_cond_lst[0].test_info_lst.test_cond_value =  INTEGER_TEST_COND_VALUE;
    dst.matching_cond_lst[0].test_info_lst.int_value = malloc(sizeof(int64_t));
    assert(dst.matching_cond_lst[0].test_info_lst.int_value != NULL && "Memory exhausted");
    *dst.matching_cond_lst[0].test_info_lst.int_value = 1;

    printf("[xApp]: Filter UEs by S-NSSAI criteria where SST = %lu\n", *dst.matching_cond_lst[0].test_info_lst.int_value);

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

static
void sm_cb_kpm(sm_ag_if_rd_t const* rd, global_e2_node_id_t const* e2_node){
    assert(rd != NULL);
    assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
    assert(rd->ind.type == KPM_STATS_V3_0);

    kpm_ind_data_t const* kpm = &rd->ind.kpm.ind;
    kpm_ric_ind_hdr_format_1_t const* hdr_frm_1 = &kpm->hdr.kpm_ric_ind_hdr_format_1;

    int64_t now = time_now_us();
    {
        lock_guard(&mtx);
        static int counter = 1;
        #ifdef KPM_V2
                // collectStartTime (32bit) unit is second
            printf("%7d, KPM v2 ind_msg latency > %ld s (minimum time unit is in second) from E2-node type %d ID %d\n",
                   counter, now/1000000 - hdr_frm_1->collectStartTime,
                   e2_node->type, e2_node->nb_id.nb_id);
        #elif defined(KPM_V3)
                // collectStartTime (64bit) unit is micro-second
            printf("%7d, KPM v3 ind_msg latency = %ld μs from E2-node type %d ID %d\n",
                   counter, now - hdr_frm_1->collectStartTime,
                   e2_node->type, e2_node->nb_id.nb_id);
        #else
                static_assert(0!=0, "Unknown KPM version");
        #endif
        if (kpm->msg.type == FORMAT_1_INDICATION_MESSAGE) {
            kpm_ind_msg_format_1_t const* msg_frm_1 = &kpm->msg.frm_1;
            for (size_t i = 0; i < msg_frm_1->meas_data_lst_len; i++) {
                const char* name = copy_ba_to_str(&msg_frm_1->meas_info_lst[i].meas_type.name);
                printf("[kpm] info name %s \n", name);
                for (size_t j = 0; j < msg_frm_1->meas_data_lst[i].meas_record_len; j++) {
                    if (msg_frm_1->meas_data_lst[i].meas_record_lst[j].value == INTEGER_MEAS_VALUE) {
                        uint32_t val = msg_frm_1->meas_data_lst[i].meas_record_lst[j].int_val;
                        printf("meas record INTEGER_MEAS_VALUE value %d\n", val);
                        if (val > 100) {
                            triggerCondition = true;
                            nr_cgi.nr_cell_id = (uint64_t) val;
                        } else if (msg_frm_1->meas_data_lst[i].meas_record_lst[j].value == REAL_MEAS_VALUE)
                            printf("meas record REAL_MEAS_VALUE value %f\n",
                                   msg_frm_1->meas_data_lst[i].meas_record_lst[j].real_val);
                        else
                            printf("meas record NO_VALUE_MEAS_VALUE value\n");
                    }
                }
            }
        } else if (kpm->msg.type == FORMAT_3_INDICATION_MESSAGE){
            kpm_ind_msg_format_3_t const* msg_frm_3 = &kpm->msg.frm_3;
            for (size_t i = 0; i < msg_frm_3->ue_meas_report_lst_len; i++){
                uint64_t id = *msg_frm_3->meas_report_per_ue[i].ue_meas_report_lst.gnb.ran_ue_id;
                printf("[KPM] KPM measurement report %zu with ran_ue_id %ld \n", i, id);
                kpm_ind_msg_format_1_t const* msg_frm_1 = &msg_frm_3->meas_report_per_ue[i].ind_msg_format_1;
                for (size_t i = 0; i < msg_frm_1->meas_data_lst_len; i++) {
                    for (size_t j = 0; j < msg_frm_1->meas_data_lst[i].meas_record_len; j++) {
                        if (msg_frm_1->meas_data_lst[i].meas_record_lst[j].value == INTEGER_MEAS_VALUE) {
                            uint32_t val = msg_frm_1->meas_data_lst[i].meas_record_lst[j].int_val;
                            printf("meas record INTEGER_MEAS_VALUE value %d\n", val);
                            if (val > 100) {
                                triggerCondition = true;
                                nr_cgi.nr_cell_id = (uint64_t) val;
                            } else if (msg_frm_1->meas_data_lst[i].meas_record_lst[j].value == REAL_MEAS_VALUE)
                                printf("meas record REAL_MEAS_VALUE value %f\n",
                                       msg_frm_1->meas_data_lst[i].meas_record_lst[j].real_val);
                            else
                                printf("meas record NO_VALUE_MEAS_VALUE value\n");
                        }
                    }
                }
            }
        }
        counter++;
    }
}

static
size_t send_sub_req(e2_node_connected_t* n, fr_args_t args, sm_ans_xapp_t *kpm_handle, size_t n_handle){
    for (int32_t i=0; i < args.sub_oran_sm_len; i++){
        if(!strcasecmp(args.sub_oran_sm[i].name, "kpm")){
            kpm_sub_data_t kpm_sub = {0};
            defer({free_kpm_sub_data(&kpm_sub);});

            // KPM Event Trigger
            uint64_t period_ms = args.sub_oran_sm[i].time;
            kpm_sub.ev_trg_def = gen_ev_trig(period_ms);
            printf("[xApp]: reporting period = %lu [ms]\n", period_ms);

            // KPM Action Definition
            kpm_sub.sz_ad = 1;
            kpm_sub.ad = calloc(1, sizeof(kpm_act_def_t));
            assert(kpm_sub.ad != NULL && "Memory exhausted");

            format_action_def_e act_type;
            if(args.sub_oran_sm[i].format == 1) {
                act_type = FORMAT_1_ACTION_DEFINITION;
            } else if (args.sub_oran_sm[i].format == 4) {
                act_type = FORMAT_4_ACTION_DEFINITION;
            } else {
                assert(0 != 0 && "not supported action definition format");
            }

            *kpm_sub.ad = gen_act_def((const char**)args.sub_oran_sm[i].actions, act_type, period_ms);

            kpm_handle[n_handle] = report_sm_xapp_api(&n->id, SM_KPM_ID, &kpm_sub, sm_cb_kpm);
            assert(kpm_handle[n_handle].success == true);
            n_handle += 1;
        }
    }
    return n_handle;
}

void *threadFunc(){
    while(!stop_flag){
        if (triggerCondition && nr_cgi.nr_cell_id != 0){
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

            triggerCondition = false;
            nr_cgi.nr_cell_id = 0;
        }
    }
}

int main(int argc, char *argv[]){
    pthread_t threadCtrl;
    fr_args_t args = init_fr_args(argc, argv);
    defer({ free_fr_args(&args); });

    //Init the xApp
    init_xapp_api(&args);
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);
    sleep(1);

    // Get e2 nodes
    nodes = e2_nodes_xapp_api();
    assert(nodes.len > 0);
    defer({free_e2_node_arr(&nodes);});
    printf("Connected E2 nodes = %d\n", nodes.len);

    int rc_ctrl = pthread_create(&threadCtrl, NULL, threadFunc, NULL);
    int rc = pthread_mutex_init(&mtx, NULL);
    assert(rc == 0 && rc_ctrl == 0);

    int max_handle = 256;
    sm_ans_xapp_t *kpm_handle = NULL;
    if (max_handle > 0){
        kpm_handle = calloc(max_handle, sizeof(sm_ans_xapp_t));
        assert(kpm_handle != NULL);
    }

    size_t n_handle = 0;
    for (int i = 0; i < nodes.len; i++) {
        e2_node_connected_t* n = &nodes.n[i];
        for (size_t j = 0; j < n->len_rf; j++)
            printf("Registered node %d ran func id = %d \n ", i, n->ack_rf[j].id);

        n_handle = send_sub_req(n, args, kpm_handle, n_handle);
    }

    sleep(5);
    printf("CTRL+C detect\n");
    for(size_t i = 0; i < n_handle; ++i){
        rm_report_sm_xapp_api(kpm_handle[i].u.handle);
    }

    free(kpm_handle);

    //Stop the xApp
    while(try_stop_xapp_api() == false)
        usleep(1000);

    stop_flag = true;
    int rc_end = pthread_join(threadCtrl, NULL);
    assert(rc_end == 0);

    printf("Test xApp run SUCCESSFULLY\n");
}