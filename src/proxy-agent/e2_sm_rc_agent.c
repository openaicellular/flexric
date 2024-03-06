/*
 * Layer between flexric E2SM callbacks and WS interface
 * it manages SUBSCRIPTION procedure (indication) and CTRL procedure
 */
#include "e2_sm_rc_agent.h"
#include "../../test/rnd/fill_rnd_data_rc.h"
#include "../sm/rc_sm/ie/ir/lst_ran_param.h"
#include "../sm/rc_sm/ie/ir/ran_param_list.h"
#include "../sm/rc_sm/ie/ir/e2sm_rc_ind_msg_frmt_2.h"
#include "../sm/rc_sm/ie/ir/seq_ue_id.h"
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <libwebsockets.h>

#include "e2_sm_agent.h"
#include "ran_if.h"
#include "proxy_agent.h"
#include "ran_msg_hdlr.h"
#include "ringbuffer.h"
#include "notif_e2_ran.h"

void init_rc_sm(void)
{
    // No allocation needed
}

void free_rc_sm(void)
{
    // No allocation needed
}

bool read_rc_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == RAN_CTRL_STATS_V1_03);
  assert(0!=0 && "Not implemented");
}

void read_rc_setup_sm(void* data)
{

  assert(data != NULL);
//  assert(data->type == RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0);
  rc_e2_setup_t* rc = (rc_e2_setup_t*)data;
  rc->ran_func_def = fill_rc_ran_func_def();
}

sm_ag_if_ans_t write_ctrl_rc_sm(void const* data)
{
    assert(data != NULL);
//  assert(data->type == RAN_CONTROL_CTRL_V1_03 );

    rc_ctrl_req_data_t const* ctrl = (rc_ctrl_req_data_t const*)data;
    if(ctrl->hdr.format == FORMAT_1_E2SM_RC_CTRL_HDR){
        if(ctrl->hdr.frmt_1.ric_style_type == 1 && ctrl->hdr.frmt_1.ctrl_act_id == 2){
            printf("QoS flow mapping configuration \n");
            e2sm_rc_ctrl_msg_frmt_1_t const* frmt_1 = &ctrl->msg.frmt_1;
            for(size_t i = 0; i < frmt_1->sz_ran_param; ++i){
                seq_ran_param_t const* rp = frmt_1->ran_param;
                if(rp[i].ran_param_id == 1){
                    assert(rp[i].ran_param_val.type == ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE );
                    printf("DRB ID %ld \n", rp[i].ran_param_val.flag_true->int_ran);
                } else if(rp[i].ran_param_id == 2){
                    assert(rp[i].ran_param_val.type == LIST_RAN_PARAMETER_VAL_TYPE);
                    printf("List of QoS Flows to be modified \n");
                    for(size_t j = 0; j < ctrl->msg.frmt_1.ran_param[i].ran_param_val.lst->sz_lst_ran_param; ++j){
                        lst_ran_param_t const* lrp = rp[i].ran_param_val.lst->lst_ran_param;
                        // The following assertion should be true, but there is a bug in the std
                        // check src/sm/rc_sm/enc/rc_enc_asn.c:1085 and src/sm/rc_sm/enc/rc_enc_asn.c:984
                        // assert(lrp[j].ran_param_id == 3);
                        assert(lrp[j].ran_param_struct.ran_param_struct[0].ran_param_id == 4) ;
                        assert(lrp[j].ran_param_struct.ran_param_struct[0].ran_param_val.type == ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE);

                        int64_t qfi = lrp[j].ran_param_struct.ran_param_struct[0].ran_param_val.flag_true->int_ran;
                        assert(qfi > -1 && qfi < 65);

                        assert(lrp[j].ran_param_struct.ran_param_struct[1].ran_param_id == 5);
                        assert(lrp[j].ran_param_struct.ran_param_struct[1].ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE);
                        int64_t dir = lrp[j].ran_param_struct.ran_param_struct[1].ran_param_val.flag_false->int_ran;
                        assert(dir == 0 || dir == 1);
                        printf("qfi = %ld dir %ld \n", qfi, dir);
                    }
                }
            }


        } else if (ctrl->hdr.frmt_1.ric_style_type == 2 && ctrl->hdr.frmt_1.ctrl_act_id == Slice_level_PRB_quotal_7_6_3_1) {
            e2sm_rc_ctrl_msg_frmt_1_t const* msg = &ctrl->msg.frmt_1;
            assert(msg->sz_ran_param == 1 && "not support msg->sz_ran_param != 1");
            seq_ran_param_t* RRM_Policy_Ratio_List = &msg->ran_param[0];
            assert(RRM_Policy_Ratio_List->ran_param_id == RRM_Policy_Ratio_List_8_4_3_6 && "wrong RRM_Policy_Ratio_List id");
            assert(RRM_Policy_Ratio_List->ran_param_val.type == LIST_RAN_PARAMETER_VAL_TYPE && "wrong RRM_Policy_Ratio_List type");
            if (RRM_Policy_Ratio_List->ran_param_val.lst) {
                size_t num_slice = RRM_Policy_Ratio_List->ran_param_val.lst->sz_lst_ran_param;
                for (size_t i = 0; i < num_slice; i++) {
                    lst_ran_param_t* RRM_Policy_Ratio_Group = &RRM_Policy_Ratio_List->ran_param_val.lst->lst_ran_param[i];
                    //Bug in rc_enc_asn.c:1003, asn didn't define ran_param_id for lst_ran_param_t...
                    //assert(RRM_Policy_Ratio_Group->ran_param_id == RRM_Policy_Ratio_Group_8_4_3_6 && "wrong RRM_Policy_Ratio_Group id");
                    assert(RRM_Policy_Ratio_Group->ran_param_struct.sz_ran_param_struct == 4 && "wrong RRM_Policy_Ratio_Group->ran_param_struct.sz_ran_param_struct");
                    assert(RRM_Policy_Ratio_Group->ran_param_struct.ran_param_struct != NULL && "NULL RRM_Policy_Ratio_Group->ran_param_struct.ran_param_struct");

                    seq_ran_param_t* RRM_Policy = &RRM_Policy_Ratio_Group->ran_param_struct.ran_param_struct[0];
                    assert(RRM_Policy->ran_param_id == RRM_Policy_8_4_3_6 && "wrong RRM_Policy id");
                    assert(RRM_Policy->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE && "wrong RRM_Policy type");
                    assert(RRM_Policy->ran_param_val.strct != NULL && "NULL RRM_Policy->ran_param_val.strct");
                    assert(RRM_Policy->ran_param_val.strct->sz_ran_param_struct == 1 && "wrong RRM_Policy->ran_param_val.strct->sz_ran_param_struct");
                    assert(RRM_Policy->ran_param_val.strct->ran_param_struct != NULL && "NULL RRM_Policy->ran_param_val.strct->ran_param_struct");

                    seq_ran_param_t* RRM_Policy_Member_List = &RRM_Policy->ran_param_val.strct->ran_param_struct[0];
                    assert(RRM_Policy_Member_List->ran_param_id == RRM_Policy_Member_List_8_4_3_6 && "wrong RRM_Policy_Member_List id");
                    assert(RRM_Policy_Member_List->ran_param_val.type == LIST_RAN_PARAMETER_VAL_TYPE && "wrong RRM_Policy_Member_List type");
                    assert(RRM_Policy_Member_List->ran_param_val.lst != NULL && "NULL RRM_Policy_Member_List->ran_param_val.lst");
                    assert(RRM_Policy_Member_List->ran_param_val.lst->sz_lst_ran_param == 1 && "wrong RRM_Policy_Member_List->ran_param_val.lst->sz_lst_ran_param");
                    assert(RRM_Policy_Member_List->ran_param_val.lst->lst_ran_param != NULL && "NULL RRM_Policy_Member_List->ran_param_val.lst->lst_ran_param");

                    lst_ran_param_t* RRM_Policy_Member = &RRM_Policy_Member_List->ran_param_val.lst->lst_ran_param[0];
                    //Bug in rc_enc_asn.c:1003, asn didn't define ran_param_id for lst_ran_param_t ...
                    //assert(RRM_Policy_Member->ran_param_id == RRM_Policy_Member_8_4_3_6 && "wrong RRM_Policy_Member id");
                    assert(RRM_Policy_Member->ran_param_struct.sz_ran_param_struct == 2 && "wrong RRM_Policy_Member->ran_param_struct.sz_ran_param_struct");
                    assert(RRM_Policy_Member->ran_param_struct.ran_param_struct != NULL && "NULL RRM_Policy_Member->ran_param_struct.ran_param_struct");

                    // TODO: check PLMN ID

                    seq_ran_param_t* S_NSSAI = &RRM_Policy_Member->ran_param_struct.ran_param_struct[1];
                    assert(S_NSSAI->ran_param_id == S_NSSAI_8_4_3_6 && "wrong S_NSSAI id");
                    assert(S_NSSAI->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE && "wrong S_NSSAI type");
                    assert(S_NSSAI->ran_param_val.strct != NULL && "NULL S_NSSAI->ran_param_val.strct");
                    assert(S_NSSAI->ran_param_val.strct->sz_ran_param_struct == 2 && "wrong S_NSSAI->ran_param_val.strct->sz_ran_param_struct");
                    assert(S_NSSAI->ran_param_val.strct->ran_param_struct != NULL && "NULL S_NSSAI->ran_param_val.strct->ran_param_struct");

                    seq_ran_param_t* SST = &S_NSSAI->ran_param_val.strct->ran_param_struct[0];
                    assert(SST->ran_param_id == SST_8_4_3_6 && "wrong SST id");
                    assert(SST->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE && "wrong SST type");
                    assert(SST->ran_param_val.flag_false != NULL && "NULL SST->ran_param_val.flag_false");
                    assert(SST->ran_param_val.flag_false->type == OCTET_STRING_RAN_PARAMETER_VALUE && "wrong SST->ran_param_val.flag_false type");
                    seq_ran_param_t* SD = &S_NSSAI->ran_param_val.strct->ran_param_struct[1];
                    assert(SD->ran_param_id == SD_8_4_3_6 && "wrong SD id");
                    assert(SD->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE && "wrong SD type");
                    assert(SD->ran_param_val.flag_false != NULL && "NULL SD->ran_param_val.flag_false");
                    assert(SD->ran_param_val.flag_false->type == OCTET_STRING_RAN_PARAMETER_VALUE && "wrong SD->ran_param_val.flag_false type");
                    ///// SLICE LABEL NAME /////
                    char* sst_str = copy_ba_to_str(&SST->ran_param_val.flag_false->octet_str_ran);
                    char* sd_str = copy_ba_to_str(&SD->ran_param_val.flag_false->octet_str_ran);

                    ///// SLICE NVS CAP /////
                    seq_ran_param_t* Dedicated_PRB_Policy_Ratio = &RRM_Policy_Ratio_Group->ran_param_struct.ran_param_struct[3];
                    assert(Dedicated_PRB_Policy_Ratio->ran_param_id == Min_PRB_Policy_Ratio_8_4_3_6 && "wrong Dedicated_PRB_Policy_Ratio id");
                    assert(Dedicated_PRB_Policy_Ratio->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE && "wrong Dedicated_PRB_Policy_Ratio type");
                    assert(Dedicated_PRB_Policy_Ratio->ran_param_val.flag_false != NULL && "NULL Dedicated_PRB_Policy_Ratio->ran_param_val.flag_false");
                    assert(Dedicated_PRB_Policy_Ratio->ran_param_val.flag_false->type == INTEGER_RAN_PARAMETER_VALUE && "wrong Dedicated_PRB_Policy_Ratio->ran_param_val.flag_false type");
                    int nvs_cap = Dedicated_PRB_Policy_Ratio->ran_param_val.flag_false->int_ran;
                    printf("RC SM: configure slice %ld, label %s %s, nvs_cap %d\n", i, sst_str, sd_str, nvs_cap);
                    free(sst_str);
                }
            } else {
                printf("RRM_Policy_Ratio_List->ran_param_val.lst is NULL\n");
            }
        }

        else if(ctrl->hdr.frmt_1.ric_style_type == 3 && ctrl->hdr.frmt_1.ctrl_act_id == Handover_control_7_6_4_1) {
            printf("[E2 AGENT]: Recv control message for handover \n");
            e2sm_rc_ctrl_msg_frmt_1_t const *msg = &ctrl->msg.frmt_1;
            assert(msg->sz_ran_param == 1 && "not support msg->sz_ran_param != 2");

            seq_ran_param_t *target_primary_cell_id = &msg->ran_param[0];
            assert(target_primary_cell_id->ran_param_id == Target_primary_cell_id_8_4_4_1 &&
                   "Wrong Target_primary_cell_id id ");
            assert(target_primary_cell_id->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE &&
                   "wrong Target_primary_cell_id type");
            assert(target_primary_cell_id->ran_param_val.strct != NULL &&
                   "NULL target_primary_cell_id->ran_param_val.strct");
            assert(target_primary_cell_id->ran_param_val.strct->sz_ran_param_struct == 1 &&
                   "wrong target_primary_cell_id->ran_param_val.strct->sz_ran_param_struct");
            assert(target_primary_cell_id->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL target_primary_cell_id->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *choice_target_cell = &target_primary_cell_id->ran_param_val.strct->ran_param_struct[0];
            assert(choice_target_cell->ran_param_id == CHOICE_target_cell_8_4_4_1 && "wrong CHOICE_target_cell id");
            assert(choice_target_cell->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE &&
                   "wrong CHOICE_target_cell type");
            assert(choice_target_cell->ran_param_val.strct != NULL && "NULL CHOICE_target_cell->ran_param_val.strct");
            assert(choice_target_cell->ran_param_val.strct->sz_ran_param_struct == 2 &&
                   "wrong CHOICE_target_cell->ran_param_val.strct->sz_ran_param_struct");
            assert(choice_target_cell->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL CHOICE_target_cell->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *nr_cell = &choice_target_cell->ran_param_val.strct->ran_param_struct[0];
            assert(nr_cell->ran_param_id == NR_cell_8_4_4_1 && "wrong NR_cell id");
            assert(nr_cell->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE && "wrong NR_cell type");
            assert(nr_cell->ran_param_val.strct != NULL && "NULL nr_cell->ran_param_val.strct");
            assert(nr_cell->ran_param_val.strct->sz_ran_param_struct == 1 &&
                   "wrong NR_cell->ran_param_val.strct->sz_ran_param_struct");
            assert(nr_cell->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL NR_cell->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *nr_cgi = &nr_cell->ran_param_val.strct->ran_param_struct[0];
            assert(nr_cgi->ran_param_id == NR_CGI_8_4_4_1 && "wrong NR_CGI id");
            assert(nr_cgi->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE && "wrong NR_CGI type");
            assert(nr_cgi->ran_param_val.flag_false != NULL && "NULL NR_CGI->ran_param_val.flag_false");
            assert(nr_cgi->ran_param_val.flag_false->type == BIT_STRING_RAN_PARAMETER_VALUE &&
                   "wrong NR_CGI->ran_param_val.flag_false type");
            char *nr_cgi_str = copy_ba_to_str(&nr_cgi->ran_param_val.flag_false->bit_str_ran);

            seq_ran_param_t *e_ultra_cell = &choice_target_cell->ran_param_val.strct->ran_param_struct[1];
            assert(e_ultra_cell->ran_param_id == E_ULTRA_Cell_8_4_4_1 && "wrong id");
            assert(e_ultra_cell->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE && "wrong e_ultra_cell type");
            assert(e_ultra_cell->ran_param_val.strct != NULL && "NULL e_ultra_cell->ran_param_val.strct");
            assert(e_ultra_cell->ran_param_val.strct->sz_ran_param_struct == 1 &&
                   "wrong e_ultra_cell->ran_param_val.strct->sz_ran_param_struct");
            assert(e_ultra_cell->ran_param_val.strct->ran_param_struct != NULL &&
                   "NULL e_ultra_cell->ran_param_val.strct->ran_param_struct");

            seq_ran_param_t *e_ultra_cgi = &e_ultra_cell->ran_param_val.strct->ran_param_struct[0];
            assert(e_ultra_cgi->ran_param_id == E_ULTRA_CGI_8_4_4_1 && "wrong E_ULTRA_CGI id");
            assert(e_ultra_cgi->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE &&
                   "wrong E_ULTRA_CGI type");
            assert(e_ultra_cgi->ran_param_val.flag_false != NULL && "NULL E_ULTRA_CGI->ran_param_val.flag_false");
            assert(e_ultra_cgi->ran_param_val.flag_false->type == BIT_STRING_RAN_PARAMETER_VALUE &&
                   "wrong E_ULTRA_CGI->ran_param_val.flag_false type");
            char *e_ultra_cgi_str = copy_ba_to_str(&e_ultra_cgi->ran_param_val.flag_false->bit_str_ran);

            ue_id_e2sm_t ue_id = ctrl->hdr.frmt_1.ue_id;
            assert(ue_id.type == GNB_UE_ID_E2SM && "Wrong ue_id_e2sm type");
            //assert(ue_id.gnb.ran_ue_id != NULL && "NULL GNB_RAN_UE_ID");
            if (ue_id.gnb.ran_ue_id != NULL)
              printf("RC SM: Handover control, handover ran_ue_id %ld to target cell NR_CGI %s\n", *ue_id.gnb.ran_ue_id, nr_cgi_str);
            else
              printf("RC SM: Handover control, decrease cell gain of current cell NR_CGI %s\n", nr_cgi_str);

            free(nr_cgi_str);
            free(e_ultra_cgi_str);
        }
    }

    sm_ag_if_wr_ctrl_t  rc_ctrl = {.type = RAN_CONTROL_CTRL_V1_03};
    rc_ctrl.rc_ctrl.hdr = cp_e2sm_rc_ctrl_hdr(&ctrl->hdr);
    rc_ctrl.rc_ctrl.msg = cp_e2sm_rc_ctrl_msg(&ctrl->msg);

    ctrl_ev_t ctrl_ev = {.sm_id = SM_RC_ID,
                         .req = rc_ctrl,
                         .ric_id = {0}};

    fwd_e2_ran_ctrl(&get_proxy_agent()->ran_if, ctrl_ev);

    sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
    ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
    return ans;
}

static
e2sm_rc_ind_hdr_frmt_1_t fill_rnd_rc_ind_hdr_frmt_1(void)
{
  e2sm_rc_ind_hdr_frmt_1_t dst = {0};

  dst.ev_trigger_id = malloc(sizeof(uint16_t));
  assert(dst.ev_trigger_id != NULL && "Memory exhausted" );

  // Event Trigger Condition ID
  // Optional
  // 9.3.21
  // [1 - 65535]
  *dst.ev_trigger_id = (rand() % 65535) + 1;

  return dst;
}

static
e2sm_rc_ind_hdr_t rc_ind_hdr()
{
  e2sm_rc_ind_hdr_t hdr = {0};

  hdr.format = FORMAT_1_E2SM_RC_IND_HDR;
  // No event_trigger_condition in rc_event_trigger format 2.
  // TODO: Research this
  hdr.frmt_1 = fill_rnd_rc_ind_hdr_frmt_1();

  return hdr;
}

static
gnb_e2sm_t fill_gnb_data(const amarisoft_ue_stats_t* ue, global_e2_node_id_t id)
{
  gnb_e2sm_t gnb = {0};

  gnb.amf_ue_ngap_id = ue->amf_ue_id;

  gnb.guami.plmn_id = (e2sm_plmn_t) {.mcc = id.plmn.mcc, .mnc = id.plmn.mnc, .mnc_digit_len = id.plmn.mnc_digit_len};
  // TODO: need metrics from AMR
  gnb.guami.amf_region_id = (rand() % 2^8) + 0;
  // TODO: need metrics from AMR
  gnb.guami.amf_set_id = (rand() % 2^10) + 0;
  // TODO: need metrics from AMR
  gnb.guami.amf_ptr = (rand() % 2^6) + 0;

  gnb.ran_ue_id = (uint64_t*)malloc(sizeof(uint64_t));
  *(gnb.ran_ue_id) = (uint64_t)ue->ran_ue_id;
  return gnb;
}

static
ran_param_struct_t fill_neighbor_cell_item(amr_ncell_list_t cur_cell){
  ran_param_struct_t res = {0};
  res.sz_ran_param_struct = 1;
  res.ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
  assert(res.ran_param_struct != NULL && "Memory exhausted");
  // CHOICE Neighbor cell
  // 21530
  res.ran_param_struct[0].ran_param_id = 21530;
  res.ran_param_struct[0].ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  res.ran_param_struct[0].ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t ));
  assert(res.ran_param_struct[0].ran_param_val.strct != NULL && "Memory exhausted");
  ran_param_struct_t* CHOICE_Neighbor_cell = res.ran_param_struct[0].ran_param_val.strct;
  CHOICE_Neighbor_cell->sz_ran_param_struct = 1;
  CHOICE_Neighbor_cell->ran_param_struct = calloc(1, sizeof(seq_ran_param_t ));
  assert(CHOICE_Neighbor_cell->ran_param_struct != NULL && "Memory exhausted");
  // NR Cell
  // 8.1.1.1
  // 21531
  CHOICE_Neighbor_cell->ran_param_struct[0].ran_param_id = 21531;
  CHOICE_Neighbor_cell->ran_param_struct[0].ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  CHOICE_Neighbor_cell->ran_param_struct[0].ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t ));
  assert(CHOICE_Neighbor_cell->ran_param_struct[0].ran_param_val.strct != NULL && "Memory exhausted");
  ran_param_struct_t* nr_cell = CHOICE_Neighbor_cell->ran_param_struct->ran_param_val.strct;
  nr_cell->sz_ran_param_struct = 1; // Support only 5G
  nr_cell->ran_param_struct = calloc(1, sizeof(seq_ran_param_t ));
  assert(nr_cell->ran_param_struct != NULL && "Memory exhausted");
  // NR PCI
  // 8.1.1.1
  // 10001
  nr_cell->ran_param_struct[0].ran_param_id = 10002;
  nr_cell->ran_param_struct[0].ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  nr_cell->ran_param_struct[0].ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(nr_cell->ran_param_struct[0].ran_param_val.flag_false != NULL && "Memory exhausted");
  ran_parameter_value_t* nr_pci = nr_cell->ran_param_struct[0].ran_param_val.flag_false;
  nr_pci->type = INTEGER_RAN_PARAMETER_VALUE;
  nr_pci->int_ran = cur_cell.n_id_nrcell;

  return res;
}

static
seq_ran_param_t fill_random_seq(){
  seq_ran_param_t res = {0};

  res.ran_param_id = 99999;
  res.ran_param_val.type= ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  res.ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(res.ran_param_val.flag_false != NULL && "Memory exhausted");
  res.ran_param_val.flag_false->type = INTEGER_RAN_PARAMETER_VALUE;
  res.ran_param_val.flag_false->int_ran = rand()%1024;

  return res;
}


static
seq_ran_param_t fill_rc_value(size_t ue_index, const ran_ind_t* ws_ind, const param_report_def_t* param_def)
{
  const amarisoft_ue_stats_t cur_ue = ws_ind->ue_stats[ue_index];
  seq_ran_param_t seq_ran_param = {0};
  switch (param_def->ran_param_id) {
    case 6: // Cell global ID
      if (ws_ind->ran_config.len_nr_cell > 0){
        seq_ran_param.ran_param_id = param_def->ran_param_id;
        // O-RAN Spec Bug:
        // in E2SM-RC v01.03 8.2.2, it defines Cell Global ID as ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE
        // however, in E2SM v2.03 6.2.2.5, it specifies Cell Global ID includes 6.2.3.7 NR CGI,
        // which includes two mandatory IE: PLMN Identity (OCTET STRING) and NR Cell Identity (BIT STRING)
        // (?Cell Global ID should be defined as STRUCTURE_RAN_PARAMETER_VAL_TYPE)
        seq_ran_param.ran_param_val.type= ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
        for(size_t i=0; i < ws_ind->ran_config.len_nr_cell; i++){
          if (cur_ue.cells[0].cell_id == ws_ind->ran_config.nr_cells[i].cell_id){
            char val[50];
            sprintf(val, "%d", ws_ind->ran_config.nr_cells[i].n_id_nrcell);
            byte_array_t ba = cp_str_to_ba(val);
            seq_ran_param.ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
            assert(seq_ran_param.ran_param_val.flag_false != NULL && "Memory exhausted");
            seq_ran_param.ran_param_val.flag_false->type = BIT_STRING_RAN_PARAMETER_VALUE;
            seq_ran_param.ran_param_val.flag_false->octet_str_ran = ba;
            break;
          }
        }
        if(seq_ran_param.ran_param_val.flag_false == NULL){
          printf("[E2-AGENT] Action definition id %d - UE ran_ue_id %d - cannot find cell ID\n", param_def->ran_param_id, cur_ue.ran_ue_id);
          goto random_seq;
        }
      } else {
        printf("[E2-AGENT] Action definition id %d - UE ran_ue_id %d - no cell connected\n", param_def->ran_param_id, cur_ue.ran_ue_id);
        goto random_seq;
      }
      break;
    case 21528:
      if (ws_ind->ran_config.len_nr_cell > 0){
        seq_ran_param.ran_param_id = 21528;
        seq_ran_param.ran_param_val.type = LIST_RAN_PARAMETER_VAL_TYPE;
        seq_ran_param.ran_param_val.lst = calloc(1, sizeof(ran_param_list_t));
        assert(seq_ran_param.ran_param_val.lst != NULL && "Memory exhausted");
        // UE Context Information
        // 8.1.1.17
        // List of Neighbor cells
        for(size_t i=0; i < ws_ind->ran_config.len_nr_cell; i++){
          nr_cell_conf_t cur_cell = ws_ind->ran_config.nr_cells[i];
          if (cur_ue.cells[0].cell_id == cur_cell.cell_id){
            if (cur_cell.len_ncell > 0){
              seq_ran_param.ran_param_val.lst->sz_lst_ran_param = cur_cell.len_ncell;
              seq_ran_param.ran_param_val.lst->lst_ran_param = calloc(cur_cell.len_ncell, sizeof(lst_ran_param_t));
              assert(seq_ran_param.ran_param_val.lst->lst_ran_param != NULL && "Memory exhausted");
              lst_ran_param_t* list_ne_cell = seq_ran_param.ran_param_val.lst->lst_ran_param;

              for (size_t cell_idx = 0; cell_idx < cur_cell.len_ncell; cell_idx++){
                // Neighbor Cell Item
                // 21529
                // No id in cell item
                list_ne_cell[cell_idx].ran_param_struct = fill_neighbor_cell_item(cur_cell.ncell_list[cell_idx]);
              }
            } else {
              printf("[E2-AGENT] Action definition id %d - UE ran_ue_id %d - cell pci %d - no neighbor cell\n", param_def->ran_param_id, cur_ue.ran_ue_id, cur_cell.n_id_nrcell);
              goto random_seq;
            }
          }
        }
      } else {
        printf("[E2-AGENT] Action definition id %d - UE ran_ue_id %d - no cell connected\n", param_def->ran_param_id, cur_ue.ran_ue_id);
        goto random_seq;
      }
      break;
    default:
      printf("[E2-AGENT] No implementation value for action definition id %d\n", param_def->ran_param_id);
      goto random_seq;
      break;
    random_seq:
      seq_ran_param = fill_random_seq();
  }
  return seq_ran_param;
}

static
e2sm_rc_ind_msg_frmt_2_t rc_ind_msg_frmt_2(const ran_ind_t* ws_ind, e2sm_rc_act_def_frmt_1_t ad_frmt_1){

  e2sm_rc_ind_msg_frmt_2_t msg_frm_2 = {0};

  // Fill UE Measurement reports
  msg_frm_2.sz_seq_ue_id = ws_ind->len_ue_stats;
  msg_frm_2.seq_ue_id = calloc(msg_frm_2.sz_seq_ue_id, sizeof(seq_ue_id_t));
  assert(msg_frm_2.seq_ue_id != NULL && "Memory exhausted");

  for (size_t i = 0; i < msg_frm_2.sz_seq_ue_id; i++){

    // Fill UE ID data
    msg_frm_2.seq_ue_id[i].ue_id.type = GNB_UE_ID_E2SM;
    msg_frm_2.seq_ue_id[i].ue_id.gnb = fill_gnb_data(&ws_ind->ue_stats[i], ws_ind->global_e2_node_id);

    // Fill UE related info
    msg_frm_2.seq_ue_id[i].sz_seq_ran_param = ad_frmt_1.sz_param_report_def;
    msg_frm_2.seq_ue_id[i].seq_ran_param = calloc(ad_frmt_1.sz_param_report_def, sizeof(seq_ran_param_t));

    for (size_t j = 0; j < ad_frmt_1.sz_param_report_def; j++){
      msg_frm_2.seq_ue_id[i].seq_ran_param[j] = fill_rc_value(i, ws_ind, &ad_frmt_1.param_report_def[j]);
    }
  }
  return msg_frm_2;
}

static
e2sm_rc_ind_msg_frmt_3_t rc_ind_msg_frmt_3(const ran_ind_t* ws_ind, e2sm_rc_act_def_frmt_1_t ad_frmt_1){

  e2sm_rc_ind_msg_frmt_3_t msg_frm_3 = {0};
  //assert(ad_frmt_1.sz_param_report_def == 1 && "only support one action definition ID when there is no UE connected");
  //uint32_t act_id = ad_frmt_1.param_report_def[0].ran_param_id;
  //assert(act_id == 6 && "only support act id 6");

  // Fill Cell information
  msg_frm_3.sz_seq_cell_info = ws_ind->ran_config.len_nr_cell;
  msg_frm_3.seq_cell_info = calloc(msg_frm_3.sz_seq_cell_info, sizeof(seq_cell_info_t));
  assert(msg_frm_3.seq_cell_info != NULL && "Memory exhausted");

  for (size_t i = 0; i < msg_frm_3.sz_seq_cell_info; i++){

    // M: Fill Cell Global ID
    msg_frm_3.seq_cell_info[i].cell_global_id.type = NR_CGI_RAT_TYPE;
    msg_frm_3.seq_cell_info[i].cell_global_id.nr_cgi.plmn_id.mcc = ws_ind->ran_config.nr_cells[i].ncgi.plmn_id.mcc;
    msg_frm_3.seq_cell_info[i].cell_global_id.nr_cgi.plmn_id.mnc = ws_ind->ran_config.nr_cells[i].ncgi.plmn_id.mnc;
    msg_frm_3.seq_cell_info[i].cell_global_id.nr_cgi.plmn_id.mnc_digit_len = ws_ind->ran_config.nr_cells[i].ncgi.plmn_id.mnc_digit_len;
    msg_frm_3.seq_cell_info[i].cell_global_id.nr_cgi.nr_cell_id = ws_ind->ran_config.nr_cells[i].n_id_nrcell;

    // O: Neighbour Relation Table
    // maybe can fill from xn connect info ?
  }
  return msg_frm_3;
}

void proxy_fill_rnd_rc_ind_data(uint32_t ric_req_id, rc_sub_data_t rc){
  ran_ind_t ws_ind = get_ringbuffer_data();
  e2sm_rc_action_def_t ad = rc.ad[0]; // Support 1 ad

  // Current support 1 action def - check on_subscription_rc_sm_ag
  rc_ind_data_t* d = calloc(1, sizeof(rc_ind_data_t));
  assert(d != NULL && "Memory exhausted");

  // RIC Report Service Style 2 - Call Process Outcome
  if (ws_ind.len_ue_stats > 0){
    switch (ad.format) {
      case FORMAT_1_E2SM_RC_ACT_DEF:
        d->hdr = rc_ind_hdr();
        d->msg.format = FORMAT_2_E2SM_RC_IND_MSG;
        // Check E2 Node NG-RAN Type
        if (E2AP_NODE_IS_MONOLITHIC(ws_ind.global_e2_node_id.type)){
          d->msg.frmt_2 = rc_ind_msg_frmt_2(&ws_ind, ad.frmt_1);
        } else {
          assert(0 != 0 && "NG-RAN Type not implemented");
        }
        break;
      default:
        assert(0 != 0 && "Unknown act definition RC format");
    }
  } else { // RIC Report Service Style 3 - E2 Node Information
    //printf("[E2-AGENT] no UE connected to Amarisoft RAN - Fill random value\n");
    //*d = fill_rnd_rc_ind_data();
    switch (ad.format) {
      case FORMAT_1_E2SM_RC_ACT_DEF:
        d->hdr = rc_ind_hdr();
        d->msg.format = FORMAT_3_E2SM_RC_IND_MSG;
        // Check E2 Node NG-RAN Type
        if (E2AP_NODE_IS_MONOLITHIC(ws_ind.global_e2_node_id.type)){
          d->msg.frmt_3 = rc_ind_msg_frmt_3(&ws_ind, ad.frmt_1);
        } else {
          assert(0 != 0 && "NG-RAN Type not implemented");
        }
        break;
      default:
        assert(0 != 0 && "Unknown act definition RC format");
    }
  }

  async_event_agent_api(ric_req_id, d);
  printf("[E2-AGENT] Event for RIC Req ID %u generated\n", ric_req_id);
}

sm_ag_if_ans_t write_subs_rc_sm(void const* src)
{
  assert(src != NULL);
  wr_rc_sub_data_t* wr_rc = (wr_rc_sub_data_t*)src;

  fwd_e2_ran_wr_sub_ev(&get_proxy_agent()->ran_if, wr_rc);

  sm_ag_if_ans_t ans = {0};

  return ans;
}

