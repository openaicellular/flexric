#include "rc_sm_amr.h"

#include "../../../../test/rnd/fill_rnd_data_rc.h"
#include "../../../sm/agent_if/read/sm_ag_if_rd.h"
#include "../../../sm/agent_if/write/sm_ag_if_wr.h"
#include "../../../sm/rc_sm/rc_sm_id.h"
#include "../../../sm/rc_sm/ie/ir/ran_param_struct.h"
#include "../../../util/alg_ds/alg/defer.h"
#include "../../../util/time_now_us.h"
#include "../e2_agent_amr_api.h"
#include "../io_ran/rc_msgs_amr.h"

#include <stdio.h>

static
e2sm_plmn_t convert_plm_mcc_mnc(char const* plmn)
{
  assert(plmn != NULL);
  e2sm_plmn_t dst = {.mnc_digit_len = 2};

  char tmp[5] = {0};

  memcpy(tmp, plmn+3, 2);
  dst.mnc = atoi(tmp);

  memcpy(tmp, plmn, 3);
  dst.mcc = atoi(tmp);
  
  return dst;
}

static
guami_t fill_guami(msg_config_get_amr_t const* cfg, ue_lst_amr_t const* ue_amr)
{
  assert(cfg != NULL);
  assert(ue_amr != NULL);
  
  guami_t dst = {0}; 

  // Mandatory
  // PLMN Identity 6.2.3.1
  dst.plmn_id = convert_plm_mcc_mnc(cfg->global_gnb_id.plmn);

  // Mandatory
  // AMF Region ID BIT STRING (SIZE(8))
  // openair3/NGAP/ngap_gNB_defs.h
  dst.amf_region_id = 0;

  // Mandatory
  //  AMF Set ID BIT STRING (SIZE(10))
  dst.amf_set_id = 0;

  // Mandatory
  // AMF Pointer BIT STRING (SIZE(6))
  dst.amf_ptr = 0;

  return dst;
}

static
ue_id_e2sm_t fill_ue_id(msg_config_get_amr_t const* cfg, ue_lst_amr_t const* ue_amr)
{
  assert(cfg != NULL);
  assert(ue_amr != NULL);

  // Only 5G supported
  ue_id_e2sm_t dst = {.type = GNB_UE_ID_E2SM};
  // 6.2.3.16
  // Mandatory
  // AMF UE NGAP ID
  // Defined in TS 38.413 [6] (NGAP) clause 9.3.3.1.
  // Defined in TS 38.401 [2].
  // fill with openair3/NGAP/ngap_gNB_ue_context.h:61
  dst.gnb.amf_ue_ngap_id = ue_amr->amf_ue_id;

  // Mandatory
  // GUAMI 6.2.3.17 
  dst.gnb.guami = fill_guami(cfg, ue_amr); 

  // gNB-CU UE F1AP ID List
  // C-ifCUDUseparated 
  //size_t gnb_cu_ue_f1ap_lst_len;  // [1,4]
  //uint32_t *gnb_cu_ue_f1ap_lst;

  //gNB-CU-CP UE E1AP ID List
  //C-ifCPUPseparated 
  //size_t gnb_cu_cp_ue_e1ap_lst_len;  // [1, 65535]
  //uint32_t *gnb_cu_cp_ue_e1ap_lst;

  // RAN UE ID
  // Optional
  // 6.2.3.25
  // OCTET STRING (SIZE (8))
  // Defined in TS 38.473 (F1AP) 
  // clause 9.2.2.1
  // UE CONTEXT SETUP REQUEST
  dst.gnb.ran_ue_id = calloc(1, sizeof(uint64_t));
  assert(dst.gnb.ran_ue_id != NULL && "Memory exhausted");
  *dst.gnb.ran_ue_id = ue_amr->ran_ue_id;

  // M-NG-RAN node UE XnAP ID
  // C- ifDCSetup
  // 6.2.3.19
  // uint32_t *ng_ran_node_ue_xnap_id;

  // Global gNB ID
  // 6.2.3.3
  // Optional
  // This IE shall not be used. Global NG-RAN Node ID IE shall replace this IE 
  // global_gnb_id_t *global_gnb_id;

  // Global NG-RAN Node ID
  // C-ifDCSetup
  // 6.2.3.2
  // global_ng_ran_node_id_t *global_ng_ran_node_id;

  return dst;
}

static
nr_cgi_t fill_nr_cgi(msg_config_get_amr_t const* cfg)
{
  assert(cfg != NULL);
  nr_cgi_t dst = {0}; 

  dst.plmn_id = convert_plm_mcc_mnc(cfg->global_gnb_id.plmn);
  // Not using nr_cell_id:36 since bit fields are nto addressable, 
  // and thus, memcpy does not work   
  dst.nr_cell_id = cfg->global_gnb_id.gnb_id; // bit string of 36 bits

  return dst;
}

static
cell_global_id_t fill_cell_global_id_t(msg_config_get_amr_t const* cfg, ue_lst_amr_t const* ue_amr) 
{
  assert(cfg != NULL);
  assert(ue_amr != NULL);
  cell_global_id_t  dst = {0}; 

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_RAT_TYPE;

  dst.nr_cgi = fill_nr_cgi(cfg);  

  return dst;
}

static
seq_ue_info_t fill_seq_ue_info(msg_config_get_amr_t const* cfg, ue_lst_amr_t const* ue_amr)
{
  assert(cfg != NULL);
  assert(ue_amr != NULL);
  seq_ue_info_t dst = {0}; 

  // UE ID
  // Mandatory
  // 9.3.10
  // Only 5G supported!
  dst.ue_id = fill_ue_id(cfg, ue_amr); 

  // UE Context Information
  // Optiona;
  // OCTET STRING
  dst.ue_ctx_info = NULL; 

  // Cell Global ID
  // Mandatory
  // 9.3.36
  dst.cell_global_id = fill_cell_global_id_t(cfg, ue_amr);

  return dst;
}

static
void fill_ue_ctx_info(rc_msgs_amr_t const* rc_msg, e2sm_rc_ind_msg_frmt_4_t* out)
{
  assert(rc_msg != NULL);
  assert(out != NULL);

  out->sz_seq_ue_info = rc_msg->ues.sz;

  if(out->sz_seq_ue_info > 0){
    out->seq_ue_info = calloc(out->sz_seq_ue_info, sizeof(seq_ue_info_t));
    assert(out->seq_ue_info != NULL && "Memory exhaiusted");
  }

  for(size_t i = 0; i < out->sz_seq_ue_info; ++i){
    out->seq_ue_info[i] = fill_seq_ue_info(&rc_msg->cfg, &rc_msg->ues.ue_lst[i]);
  }
}

static
cell_global_id_t fill_cell_global_id(rc_msgs_amr_t const* rc_msg)
{
  assert(rc_msg != NULL);
  cell_global_id_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_RAT_TYPE;

  dst.nr_cgi = fill_nr_cgi(&rc_msg->cfg) ; // 6.2.3.7

  return dst;
}

static
cell_pci_t fill_cell_pci(rc_msgs_amr_t const* rc_msg) 
{
  assert(rc_msg != NULL);

  cell_pci_t dst = {0};

  // CHOICE RAT type
  // Mandatory
  dst.type = NR_RAT_TYPE;

  dst.nr_pci = rc_msg->cfg.arr_nr_cells.nr_cells->n_id_nrcell;

  return dst;
}

static
cell_arfcn_t fill_cell_arfcn(rc_msgs_amr_t const* rc_msg)
{
  assert(rc_msg != NULL);
  cell_arfcn_t dst = {.type = NR_RAT_TYPE};

  dst.nr_arfcn = rc_msg->cfg.arr_nr_cells.nr_cells->ssb_nr_arfcn;

  return dst;
}

static
nr_cgi_t fill_cgi_2( ncgi_amr_t const* src)
{
  assert(src != NULL);

  nr_cgi_t dst = {0}; 

  dst.plmn_id = convert_plm_mcc_mnc(src->plmn);  // 6.2.3.1
  dst.nr_cell_id = src->nci; // bit string of 36 bits

  return dst;
}

static
nr_freq_info_t fill_nr_freq_info(rc_msgs_amr_t const* rc_msg, ncell_list_amr_t const* ncell_lst)
{
  assert(rc_msg != NULL);
  assert(ncell_lst != NULL);

  nr_freq_info_t dst = {0}; 

  //NR ARFCN
  //Mandatory
  //6.2.3.30
  if(ncell_lst->dl_earfcn != NULL){
    dst.arfcn = *ncell_lst->dl_earfcn;
  }  

  // [1,32]
  dst.sz_frq_bnd_it = 1; 
  dst.frq_bnd_it = calloc(1, sizeof(nr_frq_bnd_it_t));
  assert(dst.frq_bnd_it != NULL && "Memory exhausted");
  // At a first glance I did not find
  // the band of the neightbour cells in Amarisoft
  // but it must be somewhere
  // I write an stupid number instead of a real value
  // not to confuse anyone
  dst.frq_bnd_it->bnd = 1; 

  // NRFrequency Shift
  // 7p5khz
  // Optional
  dst.freq_shift_7p5khz = NULL;

  return dst;
}


static
nr_nghbr_cell_t fill_nghbr_cell(rc_msgs_amr_t const* rc_msg, ncell_list_amr_t const* ncell_lst)
{
  assert(rc_msg != NULL);
  assert(ncell_lst != NULL);

  nr_nghbr_cell_t dst = {0};

//src/proxy_ag/amarisoft/ie/types/ncell_list_amr.c
//rc_msg->cfg.arr_nr_cells.nr_cells->arr_ncell_lst->ncell_lst->ncgi-> nr_cells->

  // 9.3.41
  // NR CGI
  // 6.2.3.7
  // Mandatory
  dst.cgi = fill_cgi_2(rc_msg->cfg.arr_nr_cells.nr_cells->arr_ncell_lst->ncell_lst->ncgi);

  // 9.3.42
  // NR PCI
  // Mandatory
  assert(ncell_lst->n_id_nrcell != NULL); 
  assert(*ncell_lst->n_id_nrcell < 1008);
  dst.pci = *ncell_lst->n_id_nrcell; //[0,1007]

  // 9.3.43
  // 5GS TAC
  // 6.2.3.31
  // Defined in TS 38.473
  // 9.3.1.29
  // Mandatory
  // Using one from Apple
  // https://en.wikipedia.org/wiki/Type_Allocation_Code
  // 01124500	Apple
  // How it can fit in 3 bytes?
  memcpy(dst.tac, "01124500", 3);  

  // NR Mode Info
  // Mandatory
  if(memcmp(rc_msg->cfg.arr_nr_cells.nr_cells->mode, "TDD", 3) == 0)
    dst.mode_info = TDD_MODE_NR_NGHBR_CELL_E; 
  else if(memcmp(rc_msg->cfg.arr_nr_cells.nr_cells->mode, "FDD", 3))
    dst.mode_info = FDD_MODE_NR_NGHBR_CELL_E; 
  else
    assert(0 !=0 && "Unknown mode");

  // 9.3.44
  // NR Frequency Info
  // 6.2.3.36.
  // Bug in the standard!!!
  // 6.2.3.36 should be 6.2.3.35
  // for O-RAN.WG3.E2SM-R003-v05.00
  // NR Frequency Info 
  // Mandatory
  dst.nr_freq_info = fill_nr_freq_info(rc_msg, ncell_lst);

  // Xn X2 Established
  // Mandatory
  dst.xn_x2_established = false;

  // HO Validated
  // Mandatory
  dst.ho_validated = ncell_lst->handover_target;

  // Version
  // Mandatory
  // [1-65535] 
  dst.version = (rand() % 65535) + 1;

  return dst;
}


static
nghbr_rel_info_t fill_neighbour_rela_tbl(rc_msgs_amr_t const* rc_msg)
{
  assert(rc_msg != NULL);

  nghbr_rel_info_t dst = {0};

  // Serving Cell PCI
  // Mandatory
  // 9.3.39
  dst.pci = fill_cell_pci(rc_msg);

  // Serving Cell ARFCN
  // Mandatory
  // 9.3.40
  dst.arfcn = fill_cell_arfcn(rc_msg);

  // Neighbour Cell List
  // [1,65535]
  dst.sz_nghbr_cell = rc_msg->cfg.arr_nr_cells.nr_cells->arr_ncell_lst->sz;
  if(dst.sz_nghbr_cell > 0){
    dst.nghbr_cell = calloc(dst.sz_nghbr_cell, sizeof(nr_nghbr_cell_t));
    assert(dst.nghbr_cell != NULL && "Memory exhausted");
  }
  for(size_t i = 0; i < dst.sz_nghbr_cell; ++i){
    dst.nghbr_cell[i] = fill_nghbr_cell(rc_msg, &rc_msg->cfg.arr_nr_cells.nr_cells->arr_ncell_lst->ncell_lst[i]);
  }


  return dst;
}

static
seq_cell_info_2_t fill_seq_cell_info_2(rc_msgs_amr_t const* rc_msg)
{
  assert(rc_msg != NULL);

  seq_cell_info_2_t dst = {0};

  // Cell Global ID
  // Mandatory
  // 9.3.36
  // 6.2.2.5. 
  dst.cell_global_id = fill_cell_global_id(rc_msg);

  // Cell Context Information
  // Optional
  // OCTET STRING
  dst.cell_ctx_info = NULL;

  // Neighbour Relation Table
  // Optional
  // 9.3.38
  dst.neighbour_rela_tbl = calloc(1, sizeof(nghbr_rel_info_t));
  assert(dst.neighbour_rela_tbl != NULL && "Memory exhausted");
  *dst.neighbour_rela_tbl = fill_neighbour_rela_tbl(rc_msg);  




  return dst;
}

static
void fill_nghbr_rel_tbl(rc_msgs_amr_t const* rc_msg, e2sm_rc_ind_msg_frmt_4_t* out)
{
  assert(rc_msg != NULL);
  assert(out != NULL);

  // Sequence of Cell Information
  // [0-65535]
  out->sz_seq_cell_info_2 = rc_msg->cfg.arr_nr_cells.nr_cells->arr_ncell_lst->sz; 
  if(out->sz_seq_cell_info_2 > 0){
    out->seq_cell_info_2 = calloc(out->sz_seq_cell_info_2, sizeof(seq_cell_info_2_t)); 
    assert(out->seq_cell_info_2 != NULL && "Memory exhausted");
  }

  for(size_t i = 0; i < out->sz_seq_cell_info_2; ++i){
    out->seq_cell_info_2[i] = fill_seq_cell_info_2(rc_msg);
  }
}

static
void fill_hdr_on_demand(e2sm_rc_ind_hdr_t* hdr)
{
  assert(hdr != NULL);
  // Header
  hdr->format = FORMAT_1_E2SM_RC_IND_HDR;
  hdr->frmt_1.ev_trigger_id = NULL;
}

static  
void fill_msg_on_demand(e2sm_rc_act_def_frmt_1_t const* in, rc_msgs_amr_t const* amr, e2sm_rc_ind_msg_t* out)
{
  assert(in != NULL);
  assert(amr != NULL);
  assert(out != NULL);

  // Message
  out->format = FORMAT_4_E2SM_RC_IND_MSG;
  e2sm_rc_ind_msg_frmt_4_t* out_frmt_4 = &out->frmt_4; // 9.2.1.4.4

  for(size_t i = 0; i < in->sz_param_report_def; ++i){
    param_report_def_t const* prd = &in->param_report_def[i];
    uint32_t const rpi = prd->ran_param_id; 
    assert((rpi == UE_CTX_INFO_8_2_5 || rpi == NEIGHBOUR_RELATION_TABLE_8_2_5) && "Not implemented or unknown");
    if(prd->ran_param_id == UE_CTX_INFO_8_2_5){
      fill_ue_ctx_info(amr, out_frmt_4); 
    } else if(prd->ran_param_id == NEIGHBOUR_RELATION_TABLE_8_2_5){
      fill_nghbr_rel_tbl(amr, out_frmt_4); 
    } else {
      assert(0 != 0 && "Unknown RAN Param ID");
    }
  }
}

static
bool frmt_1_read_rc_sm_amr(rc_rd_ind_data_t* data)
{
  assert(data != NULL);

  //9.2.1.2.1
  const uint32_t on_demand_susbcription = 5; 
  assert(data->act_def->ric_style_type == on_demand_susbcription); 
  assert(data->act_def->format == FORMAT_1_E2SM_RC_ACT_DEF);
  e2sm_rc_act_def_frmt_1_t const* in_frmt_1 = &data->act_def->frmt_1;

  rc_msgs_amr_t rc_msg = {0};
  defer({ free_rc_msgs_amr(&rc_msg); });

  // Fill the data from the RAN
  fill_msg_rc_sm_api(&rc_msg);

  rc_ind_data_t* ind = &data->ind;;
  // Header
  fill_hdr_on_demand(&ind->hdr);
  // Message
  fill_msg_on_demand(in_frmt_1, &rc_msg, &ind->msg);

  return true;
}

bool frmt_2_read_rc_sm_amr(rc_rd_ind_data_t* ind)
{
  assert(ind != NULL);
  assert(0!=0 && "Not implemented");
  return true;
}


bool frmt_3_read_rc_sm_amr(rc_rd_ind_data_t* ind)
{
  assert(ind != NULL);
  assert(0!=0 && "Not implemented");
  return true;
}

bool frmt_4_read_rc_sm_amr(rc_rd_ind_data_t* ind)
{
  assert(ind != NULL);
  assert(0!=0 && "Not implemented");
  return true;
}

typedef bool (*fp_sm_rc)(rc_rd_ind_data_t*);
fp_sm_rc arr[4] = {
 frmt_1_read_rc_sm_amr,
 frmt_2_read_rc_sm_amr,
 frmt_3_read_rc_sm_amr,
 frmt_4_read_rc_sm_amr,
};



// 7.6.2
static
sm_ag_if_ans_t radio_bearer_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}

// 7.6.3
static
sm_ag_if_ans_t radio_resource_alloc_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}

static
nr_cgi_t target_cell_ho_ctrl(e2sm_rc_ctrl_msg_t const* msg)
{
  assert(msg != NULL);

  nr_cgi_t dst = {0}; 
  assert(msg->format == FORMAT_1_E2SM_RC_CTRL_MSG);
  e2sm_rc_ctrl_msg_frmt_1_t const* frmt_1 = &msg->frmt_1; 
  assert(frmt_1->sz_ran_param == 1);

  // Target_primary_cell_id_8_4_4_1
  seq_ran_param_t const* rp = frmt_1->ran_param;
  assert(rp->ran_param_id == Target_primary_cell_id_8_4_4_1);
  assert(rp->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE);
  assert(rp->ran_param_val.strct->ran_param_struct != NULL);
  // CHOICE_target_cell_8_4_4_1
  seq_ran_param_t const* rp2 = rp->ran_param_val.strct->ran_param_struct;
  assert(rp2->ran_param_id == CHOICE_target_cell_8_4_4_1);
  assert(rp2->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE);
  assert(rp2->ran_param_val.strct->ran_param_struct != NULL);
  // NR_cell_8_4_4_1 
  seq_ran_param_t const* rp3 = rp2->ran_param_val.strct->ran_param_struct;
  assert(rp3->ran_param_id == NR_cell_8_4_4_1);
  assert(rp3->ran_param_val.type == STRUCTURE_RAN_PARAMETER_VAL_TYPE);
  assert(rp3->ran_param_val.strct->ran_param_struct != NULL);
  // NR_CGI_8_4_4_1
  seq_ran_param_t const* rp4 = rp3->ran_param_val.strct->ran_param_struct;
  assert(rp4->ran_param_id == NR_CGI_8_4_4_1);
  assert(rp4->ran_param_val.type == ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE);
  assert(rp4->ran_param_val.flag_false->type == BIT_STRING_RAN_PARAMETER_VALUE);

  memcpy(&dst.nr_cell_id, rp4->ran_param_val.flag_false->bit_str_ran.buf ,8);

  return dst;
}

static
sm_ag_if_ans_t ho_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);
  
  assert(rc_ctrl->hdr.format == FORMAT_1_E2SM_RC_CTRL_HDR);
  uint32_t const conn_mode_mobility_style = 3;
  assert(rc_ctrl->hdr.frmt_1.ric_style_type == conn_mode_mobility_style);
  assert(rc_ctrl->msg.format == FORMAT_1_E2SM_RC_CTRL_MSG);
  uint32_t const ho_ctrl_act_id = 1;
  assert(rc_ctrl->hdr.frmt_1.ctrl_act_id == ho_ctrl_act_id);

  ue_id_e2sm_t const* ue = &rc_ctrl->hdr.frmt_1.ue_id;
  nr_cgi_t const target_cell = target_cell_ho_ctrl(&rc_ctrl->msg);
  defer({ free_nr_cgi((nr_cgi_t*)&target_cell); }); 

  // Call Hand Over
  assert(ue->gnb.ran_ue_id != NULL); 
  uint64_t const ran_ue_id = *ue->gnb.ran_ue_id; 

  // Wait for the answer
  rc_msgs_amr_t msg = {0};
  defer({free_rc_msgs_amr(&msg); });
  ho_rc_sm_api(target_cell.nr_cell_id, ran_ue_id, &msg);

  // Create the answer. BS from the standard. Just return the
  // time. We print error so that at least we know that an error occurred
  if(msg.ho.error != NULL)
    printf("[PROXY-AGENT]: Hand Over error: %s\n", msg.ho.error);

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0}; 
  ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
  ans.ctrl_out.rc.format = FORMAT_1_E2SM_RC_CTRL_OUT;

  e2sm_rc_ctrl_out_frmt_1_t* frmt_1 = &ans.ctrl_out.rc.frmt_1; // 9.2.1.8.1

  frmt_1->sz_seq_ran_param_2 = 1;
  frmt_1->ran_param = calloc(1, sizeof(seq_ran_param_2_t));
  assert(frmt_1->ran_param != NULL && "Memory exhausted");

  frmt_1->ran_param->ran_param_id = 1;
  ran_parameter_value_t* rpv = &frmt_1->ran_param->ran_param_value;
  
  rpv->type = OCTET_STRING_RAN_PARAMETER_VALUE;
  byte_array_t* ba = &rpv->octet_str_ran;

  ba->len = 8;
  ba->buf = calloc(ba->len, sizeof(uint8_t));
  assert(ba->buf != NULL && "Memory exhausted");
 
  int64_t const t0 = time_now_us();
  memcpy(ba->buf, &t0, sizeof(int64_t));

  return ans;
}

static
sm_ag_if_ans_t cond_ho_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);
  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}

static
sm_ag_if_ans_t daps_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);
  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}



typedef sm_ag_if_ans_t (*fp_conn_mob)(rc_ctrl_req_data_t* const rc_ctrl);

fp_conn_mob arr_conn_mob[] = {
  ho_ctrl,
  cond_ho_ctrl,
  daps_ctrl,
};

// 7.6.4
static
sm_ag_if_ans_t conn_mode_mobility(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);
  assert(rc_ctrl->hdr.format == FORMAT_1_E2SM_RC_CTRL_HDR);
  uint32_t const conn_mode_mobility_style = 3;
  assert(rc_ctrl->hdr.frmt_1.ric_style_type == conn_mode_mobility_style);
  assert(rc_ctrl->msg.format == FORMAT_1_E2SM_RC_CTRL_MSG);

  return arr_conn_mob[rc_ctrl->hdr.frmt_1.ctrl_act_id -1](rc_ctrl); 
}

// 7.6.4
static
sm_ag_if_ans_t radio_access_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}

// 7.6.5
static
sm_ag_if_ans_t dc_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}

// 7.6.6
static
sm_ag_if_ans_t ca_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;

}

// 7.6.7
static
sm_ag_if_ans_t idle_mobility_ctrl(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;

}

// 7.6.8
static
sm_ag_if_ans_t ue_info_assigment(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;

}

// 7.6.9
static
sm_ag_if_ans_t meas_report_config(rc_ctrl_req_data_t* const rc_ctrl)
{
  assert(rc_ctrl != NULL);

  assert(0 != 0 && "Not implemented");

  sm_ag_if_ans_t ans = {0}; 
  return ans;
}

// sm_ag_if_ans_t mult_actions_ctrl(rc_ctrl_req_data_t* const rc_ctrl);



typedef sm_ag_if_ans_t (*fp_wc_rc)(rc_ctrl_req_data_t* const rc_ctrl);

fp_wc_rc arr_write_ctrl[] =
{
  radio_bearer_ctrl,
  radio_resource_alloc_ctrl,
  conn_mode_mobility,
  radio_access_ctrl,
  dc_ctrl,
  ca_ctrl,
  idle_mobility_ctrl, 
  ue_info_assigment, 
  meas_report_config,
};


sm_ag_if_ans_t write_ctrl_rc_sm_amr(void const* data)
{
  assert(data != NULL);

  rc_ctrl_req_data_t* const rc_ctrl = (rc_ctrl_req_data_t* const)data;

  assert(rc_ctrl->hdr.format == FORMAT_1_E2SM_RC_CTRL_HDR);
  return arr_write_ctrl[rc_ctrl->hdr.frmt_1.ric_style_type -1](rc_ctrl);

  assert(0 != 0 && "Debug point");

  sm_ag_if_ans_t ans = {0};

  return ans;
}

void init_rc_sm_amr(void)
{

}

void free_rc_sm_amr(void)
{

}



void read_rc_setup_sm_amr(void* data)
{
  assert(data != NULL);

  rc_e2_setup_t* rc = (rc_e2_setup_t*)data;
  // This needs to be changed
  rc->ran_func_def = fill_rc_ran_func_def();
}

bool read_rc_sm_amr(void* data)
{
  assert(data != NULL);
  rc_rd_ind_data_t* rc = (rc_rd_ind_data_t*)data;
  assert(rc->act_def != NULL && "Cannot be NULL");

  return arr[rc->act_def->format](rc);
}

static
void free_aperiodic_subscription(uint32_t ric_req_id)
{
  (void)ric_req_id;
}

sm_ag_if_ans_t write_subs_rc_sm_amr(void const* src)
{
  assert(src != NULL);

  wr_rc_sub_data_t* wr_rc = (wr_rc_sub_data_t*)src;
  printf("ric req id %d \n", wr_rc->ric_req_id);


  assert(0!=0 && "Debug Point");
  //fwd_e2_ran_wr_sub_ev(&get_proxy_agent()->ran_if, wr_rc);

  sm_ag_if_ans_t ans = {.type = SUBS_OUTCOME_SM_AG_IF_ANS_V0};
  ans.subs_out.type = APERIODIC_SUBSCRIPTION_FLRC;
  ans.subs_out.aper.free_aper_subs = free_aperiodic_subscription;

  return ans;
}

