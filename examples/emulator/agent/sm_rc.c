#include "sm_rc.h"
#include "../../../test/rnd/fill_rnd_data_rc.h"
#include "../../../src/sm/rc_sm/ie/ir/lst_ran_param.h"
#include "../../../src/sm/rc_sm/ie/ir/ran_param_list.h"
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef enum{
    DRX_parameter_configuration_7_6_3_1 = 1,
    SR_periodicity_configuration_7_6_3_1 = 2,
    SPS_parameters_configuration_7_6_3_1 = 3,
    Configured_grant_control_7_6_3_1 = 4,
    CQI_table_configuration_7_6_3_1 = 5,
    Slice_level_PRB_quotal_7_6_3_1 = 6,
} rc_ctrl_service_style_2_act_id_e;

typedef enum {
    RRM_Policy_Ratio_List_8_4_3_6 = 1,
    RRM_Policy_Ratio_Group_8_4_3_6 = 2,
    RRM_Policy_8_4_3_6 = 3,
    RRM_Policy_Member_List_8_4_3_6 = 4,
    RRM_Policy_Member_8_4_3_6 = 5,
    PLMN_Identity_8_4_3_6 = 6,
    S_NSSAI_8_4_3_6 = 7,
    SST_8_4_3_6 = 8,
    SD_8_4_3_6 = 9,
    Min_PRB_Policy_Ratio_8_4_3_6 = 10,
    Max_PRB_Policy_Ratio_8_4_3_6 = 11,
    Dedicated_PRB_Policy_Ratio_8_4_3_6 = 12,
} slice_level_PRB_quota_param_id_e;

void read_rc_sm(void* data)
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
          assert(Dedicated_PRB_Policy_Ratio->ran_param_id == Dedicated_PRB_Policy_Ratio_8_4_3_6 && "wrong Dedicated_PRB_Policy_Ratio id");
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
  }

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
  return ans;
}

static
void* emulate_rrc_msg(void* ptr)
{
  
  uint32_t* ric_id = (uint32_t*)ptr; 
  for(size_t i = 0; i < 5; ++i){
    usleep(rand()%4000);
    rc_ind_data_t* d = calloc(1, sizeof(rc_ind_data_t)); 
    assert(d != NULL && "Memory exhausted");
    *d = fill_rnd_rc_ind_data();
    async_event_agent_api(*ric_id, d);
    printf("Event for RIC Req ID %u generated\n", *ric_id);
  }

  free(ptr);
  return NULL;
}

static
pthread_t t_ran_ctrl;

sm_ag_if_ans_t write_subs_rc_sm(void const* src)
{
  assert(src != NULL); // && src->type == RAN_CTRL_SUBS_V1_03);

  wr_rc_sub_data_t* wr_rc = (wr_rc_sub_data_t*)src;
  printf("ric req id %d \n", wr_rc->ric_req_id);

  uint32_t* ptr = malloc(sizeof(uint32_t));
  assert(ptr != NULL);
  *ptr = wr_rc->ric_req_id;

  int rc = pthread_create(&t_ran_ctrl, NULL, emulate_rrc_msg, ptr);
  assert(rc == 0);

  sm_ag_if_ans_t ans = {0}; 

  return ans;
}

