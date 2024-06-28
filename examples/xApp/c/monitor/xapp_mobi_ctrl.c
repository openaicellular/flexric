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
 * distributed under the License is distributed on an "AS IS" BAS
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
#include "../../../../src/util/alg_ds/alg/defer.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include "../../../../src/lib/sm/dec/dec_ue_id.h"
#include "../../../../src/sm/rc_sm/rc_sm_id.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

// #include "OCTET_STRING.h"
// #include "NR_UL-DCCH-Message.h"
// #include "NR_MeasurementReport.h"
// #include "NR_MeasurementReport-IEs.h"
// #include "NR_MeasResults.h"
// #include "NR_MeasResultListNR.h.h"
// #include "NR_MeasResultServMO.h"
// #include "NR_MeasResultServMOList.h"


typedef enum {
  RRC_MESSAGE_E2SM_RC_RAN_PARAM_ID_REPORT_1 = 3,    // 8.2.1 RAN Parameters for Report Service Style 1
  UE_ID_E2SM_RC_RAN_PARAM_ID_REPORT_1 = 4,          // 8.2.1 RAN Parameters for Report Service Style 1
  RRC_STATE_CHANGED_TO_E2SM_RC_RAN_PARAM_ID = 202,  // 8.2.4 RAN Parameters for Report Service Style 4

  END_E2SM_RC_RAN_PARAM_ID
} ran_param_id_e;

static
pthread_mutex_t mtx;

static
ue_id_e2sm_t ue_id_for_HO = {0};

typedef enum{
    Handover_Control_7_6_4_1 = 1,
    Conditional_Handover_Control_7_6_4_1 = 2,
    DAPS_Handover_Control_7_6_4_1 = 3,
} rc_ctrl_service_style_3_act_id_e;

typedef enum {
    Target_Primary_Cell_ID_8_4_4_1 = 1,
    Target_Cell_8_4_4_1 = 2,
    NR_Cell_8_4_4_1 = 3,
    NR_CGI_8_4_4_1 = 4,
    E_UTRA_Cell_8_4_4_1 = 5,
    E_UTRA_CGI_8_4_4_1 = 6,
    List_of_PDU_sessions_for_handover_8_4_4_1 = 7,
    PDU_session_Item_for_handover_8_4_4_1 = 8,
    PDU_Session_ID_8_4_4_1 = 9,
    List_of_QoS_flows_in_the_PDU_session_8_4_4_1 = 10,
    QoS_flow_Item_13_8_4_4_1 = 11,
    QoS_Flow_Identifier_8_4_4_1 = 12,
    List_of_DRBs_for_handover_8_4_4_1 = 13,
    DRB_item_for_handover_8_4_4_1 = 14,
    DRB_ID_8_4_4_1 = 15,
    List_of_QoS_flows_in_the_DRB_8_4_4_1 = 16,
    QoS_flow_Item_18_8_4_4_1 = 17,
    QoS_flow_Identifier_8_4_4_1 = 18,
    List_of_Secondary_cells_to_be_setup_8_4_4_1 = 19,
    Secondary_cell_Item_to_be_setup_8_4_4_1 = 20,
    Secondary_cell_ID_8_4_4_1 = 21,
} handover_control_id_e;

// ======================================= REPORT SERVICE ============================================

//Print Octet String value
static
void log_octet_str_ran_param_value(byte_array_t octet_str, uint32_t id)
{
  switch (id) {
    case RRC_MESSAGE_E2SM_RC_RAN_PARAM_ID_REPORT_1:
      printf("\nDecode and print RRC Message!\n");
    //   NR_UL_DCCH_Message_t *msg = NULL;
    //   asn_dec_rval_t dec_rval = uper_decode(NULL, &asn_DEF_NR_UL_DCCH_Message,
    //                                     (void **)&msg, octet_str.buf, octet_str.len, 0, 0);
    //   assert(dec_rval.code == RC_OK);
    //   xer_fprint(stdout, &asn_DEF_NR_UL_DCCH_Message, msg);
    //   // look inside the message
    //   if (msg->message.present == NR_UL_DCCH_MessageType_PR_c1 && msg->message.choice.c1->present == NR_UL_DCCH_MessageType__c1_PR_measurementReport){
    //     NR_MeasurementReport_t *measurementReport = msg->message.choice.c1->choice.measurementReport;

    //     if(measurementReport->criticalExtensions.present == NR_MeasurementReport__criticalExtensions_PR_measurementReport){
    //       NR_MeasurementReport_IEs_t *ies = measurementReport->criticalExtensions.choice.measurementReport;
          
    //       if (ies->measResults.measResultServingMOList.list.count > 0 && ies->measResults.measResultServingMOList.list.array[0]->measResultServingCell.measResult.cellResults.resultsSSB_Cell != NULL) {
    //         NR_MeasQuantityResults_t* meas_results = ies->measResults.measResultServingMOList.list.array[0]->measResultServingCell.measResult.cellResults.resultsSSB_Cell;
    //         if (meas_results->rsrp != NULL) {
    //           printf("RSRP Value: %ld\n", *(meas_results->rsrp));
    //         }
    //       } else {
    //         printf("Measurement Results or SSB Cell Results are NULL\n");
    //       }

    //       // Read out the measurement results for the neighboring cells
    //       if(ies->measResults.measResultNeighCells!=NULL && ies->measResults.measResultNeighCells->choice.measResultListNR!=NULL){
    //         printf(
    //             "There is a measurement list of Neighboring Cells of length %d\n",
    //             ies->measResults.measResultNeighCells->choice.measResultListNR->list.count);
    //       }
    //     }
    //   }
    //   ASN_STRUCT_FREE(asn_DEF_NR_UL_DCCH_Message, msg);

      break;

    case UE_ID_E2SM_RC_RAN_PARAM_ID_REPORT_1: {
      ue_id_e2sm_t *dec_ue_id_data = NULL;
      const enum asn_transfer_syntax syntax = ATS_ALIGNED_BASIC_PER;
      const asn_dec_rval_t rval = asn_decode(NULL, syntax, &asn_DEF_UEID, (void **)&dec_ue_id_data, octet_str.buf, octet_str.len);
      assert(rval.code == RC_OK && "Are you sending data in ATS_ALIGNED_BASIC_PER syntax?");
      ue_id_for_HO = *dec_ue_id_data;   //update UE ID to be Handed over from RIC REPORT
      break;
    }  

    default:
      printf("Only decoding for RRC Message and UE ID are supported!\n");
  }
}

static
void log_element_ran_param_value(ran_parameter_value_t* param_value, uint32_t id)
{
  assert(param_value != NULL);

  switch (param_value->type) {
    case OCTET_STRING_RAN_PARAMETER_VALUE:
      log_octet_str_ran_param_value(param_value->octet_str_ran, id);
      break;

    default:
      printf("Add corresponding print function for the RAN Parameter Value (other than Octet string)\n");
  }
}

static
void log_ran_param_name_frmt_1(uint32_t id)
{
  switch (id) {
    case RRC_MESSAGE_E2SM_RC_RAN_PARAM_ID_REPORT_1:
      printf("RAN Parameter Name = RRC Message\n");
      break;

    case UE_ID_E2SM_RC_RAN_PARAM_ID_REPORT_1:
      printf("RAN Parameter Name = UE ID\n");
      break;

    default:
      printf("Add corresponding RAN Parameter ID for REPORT Service Style 1\n");
  }
}

static
void log_ind_msg_frmt_1(const e2sm_rc_ind_msg_t* msg)
{
  /* log properly format 1 with "RRC Message" and "UE ID" RAN Parameter IDs*/
  const e2sm_rc_ind_msg_frmt_1_t* ind_msg_frmt_1 = &msg->frmt_1;

  static int counter = 1;
  {
    lock_guard(&mtx);

    printf("\n%7d RC Indication Message\n", counter);

    // List parameters
    for (size_t j = 0; j < ind_msg_frmt_1->sz_seq_ran_param; j++) {
      seq_ran_param_t* const ran_param_item = &ind_msg_frmt_1->seq_ran_param[j];

      log_ran_param_name_frmt_1(ran_param_item->ran_param_id);
      printf("RAN Parameter ID is: %d\n", ran_param_item->ran_param_id);

      switch (ran_param_item->ran_param_val.type) {
        case ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE:
          log_element_ran_param_value(ran_param_item->ran_param_val.flag_false, ran_param_item->ran_param_id);
          break;

        case ELEMENT_KEY_FLAG_TRUE_RAN_PARAMETER_VAL_TYPE:
          log_element_ran_param_value(ran_param_item->ran_param_val.flag_true, ran_param_item->ran_param_id);
          break;

        default:
          printf("Add corresponding function for the RAN Parameter Value Type (other than element)\n");
      }
    }

    counter++;
  }
}

typedef void (*log_ind_msg_data)(const e2sm_rc_ind_msg_t* msg);

static
log_ind_msg_data rc_msg[END_E2SM_RC_IND_MSG] = {
  log_ind_msg_frmt_1,
  NULL,
  NULL,
  NULL,
  NULL,
};

static
void sm_cb_rc(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);

  // log properly INDICATION formats
  const e2sm_rc_ind_msg_format_e type = rd->ind.rc.ind.msg.format;
  rc_msg[type](&rd->ind.rc.ind.msg);
}

static
param_report_def_t fill_param_report(uint32_t const ran_param_id, ran_param_def_t const* ran_param_def)
{
  param_report_def_t param_report = {0};

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  param_report.ran_param_id = ran_param_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  if (ran_param_def != NULL) {
    param_report.ran_param_def = calloc(1, sizeof(ran_param_def_t));
    assert(param_report.ran_param_def != NULL && "Memory exhausted");
    *param_report.ran_param_def = cp_ran_param_def(ran_param_def);
  }

  return param_report;
}

static
rrc_msg_id_t fill_rrc_msg_id_3(void)
{
  rrc_msg_id_t rrc_msg_id = {0};

  // CHOICE RRC Message Type
  rrc_msg_id.type = NR_RRC_MESSAGE_ID;

  switch (rrc_msg_id.type) {
    case NR_RRC_MESSAGE_ID: {
      rrc_msg_id.nr = UL_DCCH_NR_RRC_CLASS; // RRC Message Class
      rrc_msg_id.rrc_msg_id = MEASUREMENT_REPORT_UL_DCCH_RRC_MSG_ID; //RRC Message ID
      break;
    }

    default:
      assert(false && "Add LTE RRC Message type. At the moment, only NR RRC Message type is supported");
  }

  return rrc_msg_id;
}

static
msg_ev_trg_t fill_msg_ev_trig_3(msg_type_ev_trg_e const trigger_type)
{
  msg_ev_trg_t msg_ev_trig = {0};

  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  msg_ev_trig.ev_trigger_cond_id = 1; // this parameter contains rnd value, but must be matched in ind hdr
  /* For each information change configured, Event Trigger Condition ID is assigned
  so that E2 Node can reply to Near-RT RIC in the RIC INDICATION message to inform
  which event(s) are the cause for triggering. */

  // CHOICE Trigger Type
  msg_ev_trig.msg_type = trigger_type;

  if (trigger_type == RRC_MSG_MSG_TYPE_EV_TRG) {
    msg_ev_trig.rrc_msg = fill_rrc_msg_id_3();
  }
  else {
    assert(false && "Incorrect Trigger Type for Event Trigger Type 1!");
  }

  // Message Direction
  // Optional
  msg_ev_trig.msg_dir = NULL;

  // Associated UE Info
  // Optional
  // 9.3.26
  msg_ev_trig.assoc_ue_info = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  msg_ev_trig.log_or = NULL;

  return msg_ev_trig;
}

static
network_interface_e2rc_t fill_net_id_4(void)
{
  network_interface_e2rc_t net_id = {0};

  // CHOICE Network Type
  net_id.ni_type = F1_NETWORK_INTERFACE_TYPE;

  switch (net_id.ni_type) {
    case F1_NETWORK_INTERFACE_TYPE: {
      // NI Identifier
      //net_id.ni_id = ; 
      // NI Message
      net_id.ni_msg_id->interface_proc_id = UE_CONTEXT_SETUP_INTERFACE_PROC_ID;
      net_id.ni_msg_id->msg_type = INITIATINGMESSAGE_NETWORK_INTERFACE_MSG_ID;
      break;
    }

    default:
      assert(false && "Add other Network Interface types. At the moment, only F1 Network Interface is supported");
  }

  return net_id;
}

static
rrc_msg_id_t fill_rrc_msg_id_4(void)
{
  rrc_msg_id_t rrc_msg_id = {0};

  // CHOICE RRC Message Type
  rrc_msg_id.type = NR_RRC_MESSAGE_ID;

  switch (rrc_msg_id.type) {
    case NR_RRC_MESSAGE_ID: {
      rrc_msg_id.nr = UL_DCCH_NR_RRC_CLASS; // RRC Message Class
      rrc_msg_id.rrc_msg_id = RRC_SETUP_COMPLETE_UL_DCCH_RRC_MSG_ID; //RRC Message ID
      break;
    }

    default:
      assert(false && "Add LTE RRC Message type. At the moment, only NR RRC Message type is supported");
  }

  return rrc_msg_id;
}

static
msg_ev_trg_t fill_msg_ev_trig_4(msg_type_ev_trg_e const trigger_type)
{
  msg_ev_trg_t msg_ev_trig = {0};

  //  Event Trigger Condition ID
  //  Mandatory
  //  9.3.21
  msg_ev_trig.ev_trigger_cond_id = 1; // this parameter contains rnd value, but must be matched in ind hdr
  /* For each information change configured, Event Trigger Condition ID is assigned
  so that E2 Node can reply to Near-RT RIC in the RIC INDICATION message to inform
  which event(s) are the cause for triggering. */

  // CHOICE Trigger Type
  msg_ev_trig.msg_type = trigger_type;

  switch (trigger_type) {
    case NETWORK_INTERFACE_MSG_TYPE_EV_TRG: {
      msg_ev_trig.net = fill_net_id_4();
      break;
    }

    case RRC_MSG_MSG_TYPE_EV_TRG: {
      msg_ev_trig.rrc_msg = fill_rrc_msg_id_4();
      break;
    }

    default:
      assert(false && "Incorrect Trigger Type for Event Trigger Type 1!");
  }

  // Message Direction
  // Optional
  msg_ev_trig.msg_dir = NULL;

  // Associated UE Info
  // Optional
  // 9.3.26
  msg_ev_trig.assoc_ue_info = NULL;

  // Logical OR
  // Optional
  // 9.3.25
  msg_ev_trig.log_or = NULL;

  return msg_ev_trig;
}

static
rc_sub_data_t gen_rc_sub_msg(ran_func_def_report_t const* ran_func)
{
  assert(ran_func != NULL);

  rc_sub_data_t rc_sub = {0};

  for (size_t i = 0; i < ran_func->sz_seq_report_sty; i++) {

    if (cmp_str_ba("Message Copy", ran_func->seq_report_sty[i].name) == 0) {  // as defined in section 7.4.2, formats used for SUBSCRIPTION msg are known
      // fill properly Event Trigger Format 1 and Action Definition Format 1 with "RRC Message" and "UE ID" RAN Parameter IDs      
      size_t const sz_1 = ran_func->seq_report_sty[i].sz_seq_ran_param;
      printf("REPORT Format 1! Number of RAN Parameters is %ld\n", sz_1);

      // Generate Event Trigger
      rc_sub.et.format = ran_func->seq_report_sty[i].ev_trig_type;
      assert(rc_sub.et.format == FORMAT_1_E2SM_RC_EV_TRIGGER_FORMAT && "Event Trigger Format received not valid");
      rc_sub.et.frmt_1.sz_msg_ev_trg = sz_1;
      rc_sub.et.frmt_1.msg_ev_trg = calloc(sz_1, sizeof(msg_ev_trg_t));
      assert(rc_sub.et.frmt_1.msg_ev_trg != NULL && "Memory exhausted");

      // Generate Action Definition
      rc_sub.sz_ad = 1;
      rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
      assert(rc_sub.ad != NULL && "Memory exhausted");
      rc_sub.ad[0].ric_style_type = 1; // REPORT Service Style 1: Message Copy
      rc_sub.ad[0].format = ran_func->seq_report_sty[i].act_frmt_type;
      assert(rc_sub.ad[0].format == FORMAT_1_E2SM_RC_ACT_DEF && "Action Definition Format received not valid");
      rc_sub.ad[0].frmt_1.sz_param_report_def = sz_1;
      rc_sub.ad[0].frmt_1.param_report_def = calloc(sz_1, sizeof(param_report_def_t));
      assert(rc_sub.ad[0].frmt_1.param_report_def != NULL && "Memory exhausted");

      // Fill RAN Parameter Info
      for (size_t j = 0; j < sz_1; j++) {
        assert(( cmp_str_ba("RRC Message", ran_func->seq_report_sty[i].ran_param[j].name) == 0 || 
              cmp_str_ba("UE ID", ran_func->seq_report_sty[i].ran_param[j].name) == 0 ) && 
              "Add requested RAN Parameter. At the moment, only UE ID and RRC Message supported");

        msg_type_ev_trg_e const trigger_type = RRC_MSG_MSG_TYPE_EV_TRG;
        uint32_t const ran_param_id = ran_func->seq_report_sty[i].ran_param[j].id;
        ran_param_def_t const* ran_param_def = ran_func->seq_report_sty[i].ran_param[j].def;
        
        // Fill Event Trigger
        if (cmp_str_ba("RRC Message", ran_func->seq_report_sty[i].ran_param[j].name) == 0) {
          rc_sub.et.frmt_1.msg_ev_trg[j] = fill_msg_ev_trig_3(trigger_type);
        }
        else if (cmp_str_ba("UE ID", ran_func->seq_report_sty[i].ran_param[j].name) == 0) {
          rc_sub.et.frmt_1.msg_ev_trg[j] = fill_msg_ev_trig_4(trigger_type);
        }
        
        // Fill Action Definition
        rc_sub.ad[0].frmt_1.param_report_def[j] = fill_param_report(ran_param_id, ran_param_def);
      }      
    } else {
      assert(false && "Add requested REPORT Style. At the moment, only \"UE Information\" and \"Message Copy\" are supported");
    }
  }

  return rc_sub;
}

static
bool eq_sm(sm_ran_function_t const* elem, int const id)
{
  if (elem->id == id)
    return true;

  return false;
}

static
size_t find_sm_idx(sm_ran_function_t* rf, size_t sz, bool (*f)(sm_ran_function_t const*, int const), int const id)
{
  for (size_t i = 0; i < sz; i++) {
    if (f(&rf[i], id))
      return i;
  }

  assert(0 != 0 && "SM ID could not be found in the RAN Function List");
}

// ======================================= CONTROL SERVICE ============================================

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
void gen_nr_cell_id(seq_ran_param_t* NR_Cell)
{
  NR_Cell->ran_param_id = NR_Cell_8_4_4_1;
  NR_Cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  NR_Cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(NR_Cell->ran_param_val.strct != NULL && "Memory exhausted");
  NR_Cell->ran_param_val.strct->sz_ran_param_struct = 1;
  NR_Cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
  assert(NR_Cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

  seq_ran_param_t* NR_CGI = &NR_Cell->ran_param_val.strct->ran_param_struct[0];
  NR_CGI->ran_param_id = NR_CGI_8_4_4_1;
  NR_CGI->ran_param_val.type = ELEMENT_KEY_FLAG_FALSE_RAN_PARAMETER_VAL_TYPE;
  NR_CGI->ran_param_val.flag_false = calloc(1, sizeof(ran_parameter_value_t));
  assert(NR_CGI->ran_param_val.flag_false != NULL && "Memory exhausted");
  NR_CGI->ran_param_val.flag_false->type = OCTET_STRING_RAN_PARAMETER_VALUE;
  // In 3GPP 38.423, NR CGI = PLMN Identity (OCTET STRING) + NR Cell Identity (BIT STRING)
  // Not sure which data type is for NR CGI

}

static
void gen_target_primary_cell_id(seq_ran_param_t* Target_Primary_Cell_ID)
{
  Target_Primary_Cell_ID->ran_param_id = Target_Primary_Cell_ID_8_4_4_1;
  Target_Primary_Cell_ID->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  Target_Primary_Cell_ID->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(Target_Primary_Cell_ID->ran_param_val.strct != NULL && "Memory exhausted");
  Target_Primary_Cell_ID->ran_param_val.strct->sz_ran_param_struct = 1;
  Target_Primary_Cell_ID->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
  assert(Target_Primary_Cell_ID->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

  seq_ran_param_t* Target_Cell = &Target_Primary_Cell_ID->ran_param_val.strct->ran_param_struct[0];
  Target_Cell->ran_param_id = Target_Cell_8_4_4_1;
  Target_Cell->ran_param_val.type = STRUCTURE_RAN_PARAMETER_VAL_TYPE;
  Target_Cell->ran_param_val.strct = calloc(1, sizeof(ran_param_struct_t));
  assert(Target_Cell->ran_param_val.strct != NULL && "Memory exhausted");
  Target_Cell->ran_param_val.strct->sz_ran_param_struct = 1;
  Target_Cell->ran_param_val.strct->ran_param_struct = calloc(1, sizeof(seq_ran_param_t));
  assert(Target_Cell->ran_param_val.strct->ran_param_struct != NULL && "Memory exhausted");

  gen_nr_cell_id(&Target_Cell->ran_param_val.strct->ran_param_struct[0]);

  return;
}

static
e2sm_rc_ctrl_msg_frmt_1_t gen_rc_ctrl_msg_frmt_1_handover_control(void)
{
  e2sm_rc_ctrl_msg_frmt_1_t dst = {0};

  // 8.4.4.1

  // Target Primary Cell ID, STRUCTURE
  dst.sz_ran_param = 1;
  dst.ran_param = calloc(1, sizeof(seq_ran_param_t));
  assert(dst.ran_param != NULL && "Memory exhausted");
  gen_target_primary_cell_id(&dst.ran_param[0]);

  return dst;
}

static
e2sm_rc_ctrl_msg_t gen_rc_ctrl_msg(e2sm_rc_ctrl_msg_e msg_frmt)
{
  e2sm_rc_ctrl_msg_t dst = {0};

  if (msg_frmt == FORMAT_1_E2SM_RC_CTRL_MSG) {
    dst.format = msg_frmt;
    dst.frmt_1 = gen_rc_ctrl_msg_frmt_1_handover_control();
  } else {
    assert(0!=0 && "not implemented the fill func for this ctrl msg frmt");
  }

  return dst;
}

// =====================================================================================================

volatile sig_atomic_t keep_running = true; // Flag to control loop execution

static
void signal_handler(int sig) {
    if (sig == SIGINT) { // SIGINT is the signal for Ctrl+C
        keep_running = false;
        printf("\nCtrl+C detected. Exiting gracefully...\n");
    }
}

int main(int argc, char* argv[])
{
  fr_args_t args = init_fr_args(argc, argv);

  // Init the xApp
  init_xapp_api(&args);
  sleep(1);

  e2_node_arr_xapp_t nodes = e2_nodes_xapp_api();
  defer({ free_e2_node_arr_xapp(&nodes); });

  assert(nodes.len > 0);

  printf("Connected E2 nodes = %d\n", nodes.len);

  pthread_mutexattr_t attr = {0};
  int rc = pthread_mutex_init(&mtx, &attr);
  assert(rc == 0);

  // RAN Control REPORT handle
  sm_ans_xapp_t* hndl = calloc(nodes.len, sizeof(sm_ans_xapp_t));
  assert(hndl != NULL);

  ////////////
  // START RC
  ////////////
  int const RC_ran_function = 3;

  // RC Control
  // CONTROL Service Style 3: Connected Mode Mobility Control
  // Action ID 1: Handover Control
  // E2SM-RC Control Header Format 1
  // E2SM-RC Control Message Format 1
  rc_ctrl_req_data_t rc_ctrl = {0};

  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_xapp_t* n = &nodes.n[i];

    size_t const idx = find_sm_idx(n->rf, n->len_rf, eq_sm, RC_ran_function);
    assert(n->rf[idx].defn.type == RC_RAN_FUNC_DEF_E && "RC is not the received RAN Function");
    // if REPORT Service is supported by E2 node, send SUBSCRIPTION message
    if (n->rf[idx].defn.rc.report != NULL) {
      // Generate RC SUBSCRIPTION message
      rc_sub_data_t rc_sub = gen_rc_sub_msg(n->rf[idx].defn.rc.report);     

      hndl[i] = report_sm_xapp_api(&n->id, RC_ran_function, &rc_sub, sm_cb_rc);
      assert(hndl[i].success == true);
      free_rc_sub_data(&rc_sub);
    }

    rc_ctrl.hdr = gen_rc_ctrl_hdr(FORMAT_1_E2SM_RC_CTRL_HDR, ue_id_for_HO, 3, Handover_Control_7_6_4_1);
    rc_ctrl.msg = gen_rc_ctrl_msg(FORMAT_1_E2SM_RC_CTRL_MSG);
    control_sm_xapp_api(&nodes.n[i].id, SM_RC_ID, &rc_ctrl);
    free_rc_ctrl_req_data(&rc_ctrl);

  }
  ////////////
  // END RC
  ////////////

  signal(SIGINT, signal_handler); // Register the signal handler

  printf("Press Ctrl+C to stop.\n");

  while (keep_running) {}  // while loop to keep xApp running until Ctrl+C

  for (int i = 0; i < nodes.len; ++i) {
    // Remove the handle previously returned
    if (hndl[i].success == true)
      rm_report_sm_xapp_api(hndl[i].u.handle);
  }
  free(hndl);

  // Stop the xApp
  while (try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY!\n");

}
