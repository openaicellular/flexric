#include "../../../../src/xApp/e42_xapp_api.h"
#include <poll.h>
#include <signal.h>
#include "../../../../src/sm/rc_sm/ie/ir/ran_param_struct.h"
#include "../../../../src/util/alg_ds/ds/latch_cv/latch_cv.h"
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../../../src/sm/rc_sm/rc_sm_id.h"

static
ue_id_e2sm_t* ue_id = NULL;

static
nr_cgi_t* target_cell = NULL; 

static
global_e2_node_id_t* src_e2_node = NULL;

static
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

static
latch_cv_t latch;

// Event Trigger 
static
e2sm_rc_event_trigger_t gen_ev_trig(void)
{
  // On Demand trigger. Snapshot. 
  // No need to send a subscription delete afterwards
  e2sm_rc_event_trigger_t dst = {.format = FORMAT_5_E2SM_RC_EV_TRIGGER_FORMAT};
  dst.frmt_5.on_demand = TRUE_ON_DEMAND_FRMT_5;

  return dst;
}

static
e2sm_rc_act_def_frmt_1_t gen_rc_frmt_1(void)
{
  e2sm_rc_act_def_frmt_1_t dst = {0}; 
  
    dst.sz_param_report_def = 2;
  dst.param_report_def = calloc(dst.sz_param_report_def, sizeof(param_report_def_t));
  assert(dst.param_report_def != NULL && "Memory exhausted");
  dst.param_report_def[0].ran_param_id = UE_CTX_INFO_8_2_5;
  dst.param_report_def[1].ran_param_id = NEIGHBOUR_RELATION_TABLE_8_2_5;

  return dst;
}

// Action Definition
e2sm_rc_action_def_t gen_act_def(void)
{
  e2sm_rc_action_def_t dst = {0}; 

  //  RIC Style Type
  //  Mandatory
  //  9.3.3
  // Defined in common 6.2.2.2.

  // 5 - On Demand Report
  // This style is used to report snapshot of UE related information and
  // Cell related information upon request from Near-RT RIC.
  dst.ric_style_type = 5; 

  //9.2.1.2.1
  dst.format = FORMAT_1_E2SM_RC_ACT_DEF;
  dst.frmt_1 = gen_rc_frmt_1();

  return dst;
}

// 7.4.6
// REPORT Service Style 5: On Demand Report
static
rc_sub_data_t on_demand_rc_sub(void)
{
  rc_sub_data_t dst = {0}; 

  // Event Trigger
  dst.et = gen_ev_trig();

  // Action Definition
  dst.sz_ad = 1; 
  dst.ad = calloc(dst.sz_ad, sizeof(e2sm_rc_action_def_t));
  assert(dst.ad != NULL && "Memory exhausted");
  dst.ad[0] = gen_act_def();

  return dst;
}

void cb(sm_ag_if_rd_t const *rd, global_e2_node_id_t const *n) 
{
  assert(n != NULL);
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);
  assert(rd->ind.type == RAN_CTRL_STATS_V1_03);

  rc_ind_data_t const* ind = &rd->ind.rc.ind;

  // Header
  assert(ind->hdr.format == FORMAT_1_E2SM_RC_IND_HDR);
  // Ignoring the Event trigger condition as it is an
  // on demand message

  // Message
  assert(ind->msg.format == FORMAT_4_E2SM_RC_IND_MSG);
  e2sm_rc_ind_msg_frmt_4_t const* frmt_4 = &ind->msg.frmt_4; // 9.2.1.4.4
 
  // Assume that the RAN with the UE is the one
  // to perform the handover. It should not be 
  // a data race as only one RAN is supposed to 
  // have UEs
  if(frmt_4->sz_seq_ue_info > 0){
    lock_guard(&mtx);
    if(ue_id == NULL){
      ue_id = calloc(1, sizeof(ue_id_e2sm_t));
      assert(ue_id != NULL && "Memory exhausted");
      *ue_id = cp_ue_id_e2sm(&frmt_4->seq_ue_info[0].ue_id);   

      src_e2_node = calloc(1, sizeof(global_e2_node_id_t)); 
      assert(src_e2_node != NULL && "Memory exhausted");
      *src_e2_node = cp_global_e2_node_id(n);

      target_cell = calloc(1, sizeof(nr_cgi_t) ); 
      assert(target_cell != NULL && "Memory exhausted");
      *target_cell = cp_nr_cgi(&frmt_4->seq_cell_info_2[0].neighbour_rela_tbl->nghbr_cell[0].cgi);
    } 
  }

  // Syncronize. Notify that one message arrived
  count_down_latch_cv(&latch);
}

// 7.6.4
// CONTROL Service Style 3: Connected Mode Mobility
static
e2sm_rc_ctrl_hdr_t ho_hdr(ue_id_e2sm_t const* ue)
{
  assert(ue != NULL);

  e2sm_rc_ctrl_hdr_t dst = {0}; 

  dst.format = FORMAT_1_E2SM_RC_CTRL_HDR;
  dst.frmt_1.ric_style_type = 3;
  dst.frmt_1.ctrl_act_id = Handover_control_7_6_4_1; // Handover Control 7.6.4
  dst.frmt_1.ue_id = cp_ue_id_e2sm(ue);

  return dst;
}

static
seq_ran_param_t* add_struct_tail(seq_ran_param_t* tail, uint32_t ran_param_id)
{
  assert(tail != NULL);

  tail->ran_param_id = ran_param_id;
  tail->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  tail->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(tail->ran_param_val.strct != NULL && "Memory exhausted");

  tail->ran_param_val.strct->sz_ran_param_struct = 1;
  tail->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
  assert(tail->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");
  return tail->ran_param_val.strct->ran_param_struct;
}

static
void add_elm_tail(seq_ran_param_t* tail, uint32_t ran_param_id, nr_cgi_t const* target)
{
  assert(tail != NULL);
  assert(target != NULL);

  tail->ran_param_id = ran_param_id;
  tail->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  tail->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(tail->ran_param_val.flag_false != NULL && "Memory exhausted");

  // NR CGI IE in TS 38.423 [15] Clause 9.2.2.7
  // Bug: PLMN should also be considered
  tail->ran_param_val.flag_false->type = BIT_STRING_RAN_PARAMETER_VALUE; 
  tail->ran_param_val.flag_false->bit_str_ran.buf = calloc(8, sizeof(uint8_t));
  tail->ran_param_val.flag_false->bit_str_ran.len = 8;
  memcpy(tail->ran_param_val.flag_false->bit_str_ran.buf, &target->nr_cell_id ,8);
}

// 8.4.4.1
// Handover Control
static
e2sm_rc_ctrl_msg_t ho_msg(nr_cgi_t const* target)
{
  assert(target != NULL);

  e2sm_rc_ctrl_msg_t dst = {0};

  dst.format = FORMAT_1_E2SM_RC_CTRL_MSG;

  dst.frmt_1.sz_ran_param = 1;
  dst.frmt_1.ran_param = calloc(1, sizeof(seq_ran_param_t));

  seq_ran_param_t* tail = dst.frmt_1.ran_param;
  // Target primary cell
  tail = add_struct_tail(tail, Target_primary_cell_id_8_4_4_1); 
  tail = add_struct_tail(tail, CHOICE_target_cell_8_4_4_1); 
  tail = add_struct_tail(tail, NR_cell_8_4_4_1); 
  add_elm_tail(tail, NR_CGI_8_4_4_1, target);

  return dst;
}

// 7.6.4
// CONTROL Service Style 3: Connected Mode Mobility
static
rc_ctrl_req_data_t hand_over_rc_ctrl_msg(ue_id_e2sm_t const* ue, nr_cgi_t const* target)
{
  assert(ue != NULL);
  assert(target != NULL);

  rc_ctrl_req_data_t dst = {0}; 
  
  dst.hdr = ho_hdr(ue);
  dst.msg = ho_msg(target);

  return dst;
}

int main(int argc, char *argv[])
{
  fr_args_t args = init_fr_args(argc, argv);
  defer({ free_fr_args(&args); });

  //Init the xApp
  init_xapp_api(&args);
  poll(NULL, 0, 1000);

  e2_node_arr_xapp_t arr = e2_nodes_xapp_api();
  defer({ free_e2_node_arr_xapp(&arr); });
 
  assert(arr.len == 2 && "2 nodes needed to perform a hand over");
  sm_ans_xapp_t* hndl = calloc(arr.len, sizeof(sm_ans_xapp_t)); 
  defer({ free(hndl); });

  // Init latch to syncronize threads
  latch = init_latch_cv(arr.len);
  defer({ free_latch_cv(&latch); } );

  // Generate RAN CONTROL Subscription
  rc_sub_data_t rc_sub = on_demand_rc_sub();
  defer({ free_rc_sub_data(&rc_sub); });

  // Retrieve information about the E2 Nodes in the callback func (cb)
  for(size_t i = 0; i < arr.len; ++i){
    hndl[i] = report_sm_xapp_api(&arr.n[i].id, SM_RC_ID, &rc_sub, cb);
    assert(hndl[i].success == true);
  }

  // Syncronize. Wait until all the previous messages arrive using a latch
  wait_latch_cv(&latch);

  // Generate RAN CONTROL Control msg 
  rc_ctrl_req_data_t ho = hand_over_rc_ctrl_msg(ue_id, target_cell); 
  defer( { free_rc_ctrl_req_data(&ho); });

  // Send the Handover cmd 
  sm_ans_xapp_t ans = control_sm_xapp_api(src_e2_node, SM_RC_ID, &ho);
  assert(ans.success == true);

  // stop the xApp
  while(try_stop_xapp_api() == false)
    poll(NULL, 0, 1000);

  free_ue_id_e2sm(ue_id);
  free_nr_cgi(target_cell); 
  free_global_e2_node_id(src_e2_node);

  return 0;
}

