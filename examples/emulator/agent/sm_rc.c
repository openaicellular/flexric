#include "sm_rc.h"
#include "../../../test/rnd/fill_rnd_data_rc.h"
#include "../../../src/sm/rc_sm/ie/ir/lst_ran_param.h"
#include "../../../src/sm/rc_sm/ie/ir/ran_param_list.h"
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void init_rc_sm(void)
{
  // No allocation needed
}

void free_rc_sm(void)
{
  // No allocation needed
}

typedef enum {
  RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID = 202,   // 8.2.4  RAN Parameters for Report Service Style 4

  END_E2SM_RC_RAN_PARAM_ID
} ran_param_id_e;

bool read_rc_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == RAN_CTRL_STATS_V1_03);
  assert(0!=0 && "Not implemented");
  return true;
}

#if defined(NGRAN_GNB) || defined(NGRAN_GNB_CU)
static
void fill_rc_ev_trig(ran_func_def_ev_trig_t* ev_trig)
{
  // Sequence of EVENT TRIGGER styles
  // [1 - 63]
  ev_trig->sz_seq_ev_trg_style = 1;
  ev_trig->seq_ev_trg_style = calloc(ev_trig->sz_seq_ev_trg_style, sizeof(seq_ev_trg_style_t));
  assert(ev_trig->seq_ev_trg_style != NULL && "Memory exhausted");

  seq_ev_trg_style_t* ev_trig_style = &ev_trig->seq_ev_trg_style[0];

  // RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  //  INTEGER
  ev_trig_style->style = 4;

  // RIC Event Trigger Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3
  //PrintableString(SIZE(1..150,...))
  const char ev_style_name[] = "UE Information Change";
  ev_trig_style->name = cp_str_to_ba(ev_style_name);

  // RIC Event Trigger Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  ev_trig_style->format = FORMAT_4_E2SM_RC_EV_TRIGGER_FORMAT;

  // Sequence of RAN Parameters for L2 Variables
  // [0 - 65535]
  ev_trig->sz_seq_ran_param_l2_var = 0;
  ev_trig->seq_ran_param_l2_var = NULL;

  //Sequence of Call Process Types
  // [0-65535]
  ev_trig->sz_seq_call_proc_type = 0;
  ev_trig->seq_call_proc_type = NULL;

  // Sequence of RAN Parameters for Identifying UEs
  // 0-65535
  ev_trig->sz_seq_ran_param_id_ue = 0;
  ev_trig->seq_ran_param_id_ue = NULL;

  // Sequence of RAN Parameters for Identifying Cells
  // 0-65535
  ev_trig->sz_seq_ran_param_id_cell = 0;
  ev_trig->seq_ran_param_id_cell = NULL;
}

static
seq_ran_param_3_t fill_rc_ran_param(void)
{
  seq_ran_param_3_t ran_param = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1- 4294967295]
  ran_param.id = RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  // [1-150] 
  const char ran_param_name[] = "RRC State Changed To";
  ran_param.name = cp_str_to_ba(ran_param_name);

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  ran_param.def = NULL;

  return ran_param;
}

static
void fill_rc_report(ran_func_def_report_t* report)
{
  // Sequence of REPORT styles
  // [1 - 63]
  report->sz_seq_report_sty = 1;
  report->seq_report_sty = calloc(report->sz_seq_report_sty, sizeof(seq_report_sty_t));
  assert(report->seq_report_sty != NULL && "Memory exhausted");

  seq_report_sty_t* report_style = &report->seq_report_sty[0];

    // RIC Report Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  report_style->report_type = 4;

  // RIC Report Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // PrintableString(SIZE(1..150,...)) 
  const char report_name[] = "UE Information";
  report_style->name = cp_str_to_ba(report_name);

  // Supported RIC Event Trigger Style Type 
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  report_style->ev_trig_type = FORMAT_4_E2SM_RC_EV_TRIGGER_FORMAT;

  // RIC Report Action Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  report_style->act_frmt_type = FORMAT_1_E2SM_RC_ACT_DEF;

  // RIC Indication Header Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  report_style->ind_hdr_type = FORMAT_1_E2SM_RC_IND_HDR;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  // INTEGER
  report_style->ind_msg_type = FORMAT_2_E2SM_RC_IND_MSG;

  // Sequence of RAN Parameters Supported
  // [0 - 65535]
  report_style->sz_seq_ran_param = 1;
  report_style->ran_param = calloc(report_style->sz_seq_ran_param, sizeof(seq_ran_param_3_t));
  assert(report_style->ran_param != NULL && "Memory exhausted");
  report_style->ran_param[0] = fill_rc_ran_param();
}

static
seq_ctrl_act_2_t fill_rc_ctrl_act(void)
{
  seq_ctrl_act_2_t ctrl_act = {0};

  // Control Action ID
  // Mandatory
  // 9.3.6
  // [1-65535]
  ctrl_act.id = 2;

  // Control Action Name
  // Mandatory
  // 9.3.7
  // [1-150]
  const char control_act_name[] = "QoS flow mapping configuration";
  ctrl_act.name = cp_str_to_ba(control_act_name);

  // Sequence of Associated RAN Parameters
  // [0-65535]
  ctrl_act.sz_seq_assoc_ran_param = 2;
  ctrl_act.assoc_ran_param = calloc(ctrl_act.sz_seq_assoc_ran_param, sizeof(seq_ran_param_3_t));
  assert(ctrl_act.assoc_ran_param != NULL && "Memory exhausted");

  seq_ran_param_3_t* assoc_ran_param = ctrl_act.assoc_ran_param;

  // DRB ID
  assoc_ran_param[0].id = 1;
  const char ran_param_drb_id[] = "DRB ID";
  assoc_ran_param[0].name = cp_str_to_ba(ran_param_drb_id);
  assoc_ran_param[0].def = NULL;

  // List of QoS Flows to be modified in DRB
  assoc_ran_param[1].id = 2;
  const char ran_param_list_qos_flow[] = "List of QoS Flows to be modified in DRB";
  assoc_ran_param[1].name = cp_str_to_ba(ran_param_list_qos_flow);
  assoc_ran_param[1].def = calloc(1, sizeof(ran_param_def_t));
  assert(assoc_ran_param[1].def != NULL && "Memory exhausted");

  assoc_ran_param[1].def->type = LIST_RAN_PARAMETER_DEF_TYPE;
  assoc_ran_param[1].def->lst = calloc(1, sizeof(ran_param_type_t));
  assert(assoc_ran_param[1].def->lst != NULL && "Memory exhausted");

  ran_param_type_t* lst = assoc_ran_param[1].def->lst;
  lst->sz_ran_param = 2;
  lst->ran_param = calloc(lst->sz_ran_param, sizeof(ran_param_lst_struct_t));
  assert(lst->ran_param != NULL && "Memory exhausted");

  // QoS Flow Identifier
  lst->ran_param[0].ran_param_id = 4;
  const char ran_param_qos_flow_id[] = "QoS Flow Identifier";
  lst->ran_param[0].ran_param_name = cp_str_to_ba(ran_param_qos_flow_id);
  lst->ran_param[0].ran_param_def = NULL;

  // QoS Flow Mapping Indication
  lst->ran_param[1].ran_param_id = 5;
  const char ran_param_qos_flow_mapping_ind[] = "QoS Flow Mapping Indication";
  lst->ran_param[1].ran_param_name = cp_str_to_ba(ran_param_qos_flow_mapping_ind);
  lst->ran_param[1].ran_param_def = NULL;

  return ctrl_act;
}

static
void fill_rc_control(ran_func_def_ctrl_t* ctrl)
{
  // Sequence of CONTROL styles
  // [1 - 63]
  ctrl->sz_seq_ctrl_style = 1;
  ctrl->seq_ctrl_style = calloc(ctrl->sz_seq_ctrl_style, sizeof(seq_ctrl_style_t));
  assert(ctrl->seq_ctrl_style != NULL && "Memory exhausted");

  seq_ctrl_style_t* ctrl_style = &ctrl->seq_ctrl_style[0];

  // RIC Control Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2
  ctrl_style->style_type = 1;

  //RIC Control Style Name
  //Mandatory
  //9.3.4
  // [1 -150]
  const char control_name[] = "Radio Bearer Control";
  ctrl_style->name = cp_str_to_ba(control_name);

  // RIC Control Header Format Type
  // Mandatory
  // 9.3.5
  ctrl_style->hdr = FORMAT_1_E2SM_RC_CTRL_HDR;

  // RIC Control Message Format Type
  // Mandatory
  // 9.3.5
  ctrl_style->msg = FORMAT_1_E2SM_RC_CTRL_MSG;

  // RIC Call Process ID Format Type
  // Optional
  ctrl_style->call_proc_id_type = NULL;

  // RIC Control Outcome Format Type
  // Mandatory
  // 9.3.5
  ctrl_style->out_frmt = FORMAT_1_E2SM_RC_CTRL_OUT;

  // Sequence of Control Actions
  // [0-65535]
  ctrl_style->sz_seq_ctrl_act = 1;
  ctrl_style->seq_ctrl_act = calloc(ctrl_style->sz_seq_ctrl_act, sizeof(seq_ctrl_act_2_t));
  assert(ctrl_style->seq_ctrl_act != NULL && "Memory exhausted");
  ctrl_style->seq_ctrl_act[0] = fill_rc_ctrl_act();

  // Sequence of Associated RAN 
  // Parameters for Control Outcome
  // [0- 255]
  ctrl_style->sz_ran_param_ctrl_out = 0;
  ctrl_style->ran_param_ctrl_out = NULL;
}

static
seq_ins_ind_t fill_rc_ins_ind(void)
{
  seq_ins_ind_t ins_ind = {0};

  // Insert Indication ID
  // Mandatory
  // 9.3.16
  // [1-65535]
  ins_ind.id=1; // INSERT Service Style 3, Indication Handover Control Request, 7.5.4.1

  // Insert Indication Name
  // Mandatory
  // 9.3.17
  // [1-150]
  const char ins_ind_name[] = "Handover Control request";
  ins_ind.name=cp_str_to_ba(ins_ind_name);

  // Sequence of Insert Indications
  // [0-65535]
  ins_ind.sz_seq_ins_ind = 1;
  ins_ind.seq_ins_ind = calloc(ins_ind.sz_seq_ins_ind, sizeof(seq_ran_param_3_t));
  assert(ins_ind.seq_ins_ind != NULL && "Memory exhausted");
  ins_ind.seq_ins_ind[0].id = 1;
  const char ran_param_name[] = "Target Primary Cell ID";
  ins_ind.seq_ins_ind[0].name = cp_str_to_ba(ran_param_name);

  return ins_ind;
}

static
void fill_rc_insert(ran_func_def_insert_t* insert)
{
  // Sequence of INSERT styles
  // [1 - 63]
  insert->sz_seq_ins_sty = 1;
  insert->seq_ins_sty = calloc(insert->sz_seq_ins_sty, sizeof(seq_ins_sty_t));
  assert(insert->seq_ins_sty != NULL && "Memory exhausted");

  seq_ins_sty_t* insert_style = &insert->seq_ins_sty[0];

  // RIC Insert Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  // INTEGER
  insert_style->style_type = 3;

  // RIC Insert Style Name
  // Mandatory
  // 9.3.4
  // 6.2.2.3.
  // [1-150]
  const char insert_name[] = "Connected Mode Mobility Control Request";
  insert_style->name = cp_str_to_ba(insert_name);

  // Supported RIC Event Trigger Style Type
  // Mandatory
  // 9.3.3
  // 6.2.2.2.
  insert_style->ev_trig_style_type = FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT;

  // RIC Action Definition Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  insert_style->act_def_frmt_type = FORMAT_3_E2SM_RC_ACT_DEF;

  // Sequence of Insert Indications
  // [0-65535]
  insert_style->sz_seq_ins_ind = 1;
  insert_style->seq_ins_ind = calloc(insert_style->sz_seq_ins_ind, sizeof(seq_ins_ind_t));
  assert(insert_style->seq_ins_ind != NULL && "Memory exhausted");
  insert_style->seq_ins_ind[0] = fill_rc_ins_ind();


  // RIC Indication Header Format Type
  // Mandatoyr
  // 9.3.5
  // 6.2.2.4.
  insert_style->ind_hdr_frmt_type = FORMAT_2_E2SM_RC_IND_HDR;

  // RIC Indication Message Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  insert_style->ind_msg_frmt_type = FORMAT_5_E2SM_RC_IND_MSG;

  // RIC Call Process ID Format Type
  // Mandatory
  // 9.3.5
  // 6.2.2.4.
  insert_style->call_proc_id_type = 1; // RIC Call Process ID IE Format 1
}
#endif

static
e2sm_rc_func_def_t fill_rc_ran_def(void)
{
  e2sm_rc_func_def_t def = {0};

#if defined(NGRAN_GNB) || defined(NGRAN_GNB_CU)
  // RAN Function Definition for EVENT TRIGGER
  // Optional
  // 9.2.2.2
  def.ev_trig = calloc(1, sizeof(ran_func_def_ev_trig_t));
  assert(def.ev_trig != NULL && "Memory exhausted");
  fill_rc_ev_trig(def.ev_trig);

  // RAN Function Definition for REPORT
  // Optional
  // 9.2.2.3
  def.report = calloc(1, sizeof(ran_func_def_report_t));
  assert(def.report != NULL && "Memory exhausted");
  fill_rc_report(def.report);

  // RAN Function Definition for CONTROL
  // Optional
  // 9.2.2.5
  def.ctrl = calloc(1, sizeof(ran_func_def_ctrl_t));
  assert(def.ctrl != NULL && "Memory exhausted");
  fill_rc_control(def.ctrl);

  // RAN Function Definition for INSERT
  // Optional
  // 9.2.2.4
  // def.insert = NULL;
  def.insert = calloc(1, sizeof(ran_func_def_insert_t));
  assert(def.insert != NULL && "Memory exhausted");
  fill_rc_insert(def.insert);

  // RAN Function Definition for POLICY
  // Optional
  // 9.2.2.6
  def.policy = NULL;
#endif

  return def;
}

void read_rc_setup_sm(void* data)
{
  assert(data != NULL);
//  assert(data->type == RAN_CTRL_V1_3_AGENT_IF_E2_SETUP_ANS_V0);
  rc_e2_setup_t* rc = (rc_e2_setup_t*)data;

  /* Fill the RAN Function Definition with currently supported measurements */
  
  // RAN Function Name is already filled in fill_ran_function_name() in rc_sm_agent.c

  rc->ran_func_def = fill_rc_ran_def();
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


    }
  }

  sm_ag_if_ans_t ans = {.type = CTRL_OUTCOME_SM_AG_IF_ANS_V0};
  ans.ctrl_out.type = RAN_CTRL_V1_3_AGENT_IF_CTRL_ANS_V0;
  return ans;
}

static
uint32_t sta_ric_id;

static
void free_aperiodic_subscription(uint32_t ric_req_id)
{
  assert(ric_req_id == sta_ric_id); 
  (void)ric_req_id;
}

static
void* emulate_rrc_msg(void* ptr)
{
  (void)ptr; 
  for(size_t i = 0; i < 3; ++i){
    usleep(rand()%4000);
    rc_ind_data_t* d = calloc(1, sizeof(rc_ind_data_t)); 
    assert(d != NULL && "Memory exhausted");
    d->hdr.format = FORMAT_1_E2SM_RC_IND_HDR;
    d->hdr.frmt_1 = fill_rnd_rc_ind_hdr_frmt_1();
    d->msg.format = FORMAT_2_E2SM_RC_IND_MSG;
    d->msg.frmt_1 = fill_rnd_ind_msg_frmt_1();
    d->msg.frmt_2 = fill_rnd_ind_msg_frmt_2();

    async_event_agent_api(sta_ric_id, d);
    printf("Event for RIC Req ID %u generated\n", sta_ric_id);
  }

  return NULL;
}

typedef enum {
    Target_Primary_CELL_ID_8_4_4_1 = 1,
    CHOICE_Target_CELL_8_4_4_1 = 2,
    NR_CELL_8_4_4_1 = 3,
    NR_CGI_8_4_4_1 = 4,
    E_UTRA_CELL_8_4_4_1 = 5,
    E_UTRA_CGI_8_4_4_1 = 6,
    LIST_of_PDU_sessions_for_handover_8_4_4_1 = 7,
    PDU_session_Item_for_handover_8_4_4_1 = 8,
    PDU_Session_ID_8_4_4_1 = 9,
    List_of_QoS_flows_in_the_PDU_session_8_4_4_1 = 10,
    QoS_flow_item_8_4_4_1 = 11,
    QoS_Flow_Identifier_8_4_4_1 = 12,
    List_of_DRBs_for_handover_8_4_4_1 = 13,
    DRB_item_for_handover_8_4_4_1 = 14,
    DRB_ID_8_4_4_1 = 15,
    List_of_QoS_flows_in_the_DRB_8_4_4_1 = 16,
    QoS_flow_Item_8_4_4_1 = 17,
    QoS_flow_Identifier_8_4_4_1 = 18,
    List_of_Secondary_cells_to_be_setup_8_4_4_1 = 19,
    Secondary_cell_Item_to_be_setup_8_4_4_1 = 20,
    Secondary_cell_8_4_4_1 = 21,
} handover_control_param_id_e;

e2sm_rc_ind_msg_frmt_5_t fill_handover_control_ind_msg_frmt_5(void)
{
  // 8.4.4.1
    // Target Primary Cell ID, STRUCTURE (len 1)
    // >CHOICE Target Cell, STRUCTURE (len 2)
    // >>NR Cell, STRUCTURE (len 1)
    // >>>NR CGI, ELEMENT (len 1)
    // >>E-UTRA Cell, STRUCTURE (len 1)
    // >>>E-UTRA CGI, ELEMENT (len 1)
  e2sm_rc_ind_msg_frmt_5_t dst = {0}; 

  // List of RAN parameters requested
  // [0-65535]
  dst.sz_seq_ran_param = 1;

  if(dst.sz_seq_ran_param > 0){
    dst.seq_ran_param = calloc(dst.sz_seq_ran_param, sizeof(seq_ran_param_t));
    assert(dst.seq_ran_param != NULL && "memory exhausted");
  }
  
  for(size_t i = 0; i < dst.sz_seq_ran_param; ++i){
    dst.seq_ran_param[i].ran_param_id = Target_Primary_CELL_ID_8_4_4_1;
    dst.seq_ran_param[i].ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    dst.seq_ran_param[i].ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
    assert(dst.seq_ran_param[i].ran_param_val.strct != NULL && "Memory exhausted" );
    dst.seq_ran_param[i].ran_param_val.strct->sz_ran_param_struct = 1;
    dst.seq_ran_param[i].ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(dst.seq_ran_param[i].ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted" );
    // CHOICE Target Cell, Structure
    seq_ran_param_t* Choice_target_cell_struct = dst.seq_ran_param[i].ran_param_val.strct->ran_param_struct;
    Choice_target_cell_struct->ran_param_id = CHOICE_Target_CELL_8_4_4_1;
    Choice_target_cell_struct->ran_param_val.type=STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    Choice_target_cell_struct->ran_param_val.strct = calloc(1,sizeof(ran_param_struct_t));
    assert(Choice_target_cell_struct->ran_param_val.strct != NULL && "Memory exhausted");
    Choice_target_cell_struct->ran_param_val.strct->sz_ran_param_struct = 1;
    Choice_target_cell_struct->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(Choice_target_cell_struct->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted" );
    // NR Cell, Structure
    seq_ran_param_t* NR_Cell_struct = Choice_target_cell_struct->ran_param_val.strct->ran_param_struct;
    NR_Cell_struct->ran_param_id=NR_CELL_8_4_4_1;
    NR_Cell_struct->ran_param_val.type=STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    NR_Cell_struct->ran_param_val.strct=calloc(1,sizeof(ran_param_struct_t));
    assert(NR_Cell_struct->ran_param_val.strct != NULL && "Memory exhausted");
    NR_Cell_struct->ran_param_val.strct->sz_ran_param_struct = 1;
    NR_Cell_struct->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    assert(NR_Cell_struct->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted" );
    // NR CGI, ELEMENT
    seq_ran_param_t* NR_CGI_element = NR_Cell_struct->ran_param_val.strct->ran_param_struct;
    NR_CGI_element->ran_param_id=NR_CGI_8_4_4_1;
    NR_CGI_element->ran_param_val.type=ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    NR_CGI_element->ran_param_val.flag_false=calloc(1, sizeof(ran_parameter_value_t));
    assert(NR_CGI_element->ran_param_val.flag_false != NULL && "Memory exhausted");
    NR_CGI_element->ran_param_val.flag_false->type=BIT_STRING_RAN_PARAMETER_VALUE;
    NR_CGI_element->ran_param_val.flag_false->bit_str_ran=cp_str_to_ba("42424242");

    // // E-UTRA Cell, Structure
    // seq_ran_param_t* E_UTRA_Cell_struct = Choice_target_cell_struct->ran_param_val.strct->ran_param_struct;
    // E_UTRA_Cell_struct->ran_param_id=E_UTRA_CELL_8_4_4_1;
    // E_UTRA_Cell_struct->ran_param_val.type=STRUCTURE_RAN_PARAMETER_VAL_TYPE;
    // E_UTRA_Cell_struct->ran_param_val.strct=calloc(1,sizeof(ran_param_struct_t));
    // assert(E_UTRA_Cell_struct->ran_param_val.strct != NULL && "Memory exhausted");
    // E_UTRA_Cell_struct->ran_param_val.strct->sz_ran_param_struct = 1;
    // E_UTRA_Cell_struct->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
    // assert(E_UTRA_Cell_struct->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted" );
    // // E-UTRA CGI, ELEMENT
    // seq_ran_param_t* E_UTRA_CGI_element = E_UTRA_Cell_struct->ran_param_val.strct->ran_param_struct;
    // E_UTRA_CGI_element->ran_param_id=E_UTRA_CGI_8_4_4_1;
    // E_UTRA_CGI_element->ran_param_val.type=ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
    // E_UTRA_CGI_element->ran_param_val.flag_false=calloc(1, sizeof(ran_parameter_value_t));
    // assert(E_UTRA_CGI_element->ran_param_val.flag_false != NULL && "Memory exhausted");
    // E_UTRA_CGI_element->ran_param_val.flag_false->type=BIT_STRING_RAN_PARAMETER_VALUE;
    // E_UTRA_CGI_element->ran_param_val.flag_false->bit_str_ran=cp_str_to_ba("42424242");
  }

  return dst;
}

static
void* emulate_ins_ind(void* ptr)
{
  (void)ptr; 
  for(size_t i = 0; i < 1; ++i){
    usleep(rand()%4000);
    rc_ind_data_t* d = calloc(1, sizeof(rc_ind_data_t)); 
    assert(d != NULL && "Memory exhausted");
    d->hdr.format = FORMAT_2_E2SM_RC_IND_HDR;
    d->hdr.frmt_2 = fill_rnd_rc_ind_hdr_frmt_2();
    d->msg.format = FORMAT_5_E2SM_RC_IND_MSG;
    d->msg.frmt_5 = fill_handover_control_ind_msg_frmt_5();
    // d->msg.frmt_5 = fill_rnd_ind_msg_frmt_5();

    async_event_agent_api(sta_ric_id, d);
    printf("Event for RIC Req ID %u generated\n", sta_ric_id);
  }

  return NULL;
}

static
pthread_t t_ran_ctrl;

sm_ag_if_ans_t write_subs_rc_sm(void const* src)
{
  assert(src != NULL); // && src->type == RAN_CTRL_SUBS_V1_03);

  wr_rc_sub_data_t* wr_rc = (wr_rc_sub_data_t*)src;
  printf("ric req id %d \n", wr_rc->ric_req_id);

  sta_ric_id = wr_rc->ric_req_id;

  int rc = pthread_create(&t_ran_ctrl, NULL, emulate_ins_ind, NULL);
  assert(rc == 0);

  sm_ag_if_ans_t ans = {.type = SUBS_OUTCOME_SM_AG_IF_ANS_V0};
  ans.subs_out.type = APERIODIC_SUBSCRIPTION_FLRC;
  ans.subs_out.aper.free_aper_subs = free_aperiodic_subscription;

  return ans;
}
