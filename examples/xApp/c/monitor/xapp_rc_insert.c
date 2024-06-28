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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

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

static
pthread_mutex_t mtx;


static
void print_octet_string_hex(const byte_array_t *data) {
  printf("Printing Received Octet string. The length of the string is %ld\n", data->len);
  for (size_t i = 0; i < data->len; i++) {
      printf("%02X ", data->buf[i]); 
  }
  printf("\n");
}

//Print Octet String value
static
void log_octet_str_ran_param_value(byte_array_t octet_str)
{
  print_octet_string_hex(&octet_str);
}

void log_bit_str_ran_param_value(byte_array_t arr)
{
  printf("Bit Str Content: %s\n",arr.buf);
}

static
void log_element_ran_param_value(ran_parameter_value_t* param_value)
{
  assert(param_value != NULL);

  switch (param_value->type) {

    case OCTET_STRING_RAN_PARAMETER_VALUE:
      log_octet_str_ran_param_value(param_value->octet_str_ran);
      break;

    case BIT_STRING_RAN_PARAMETER_VALUE:
      log_bit_str_ran_param_value(param_value->bit_str_ran);
      break;


    default:
      printf("Add corresponding print function for the RAN Parameter Value (other than Integer and Octet string)\n");
  }
}

static
void log_handover_control_ran_param_name(uint32_t id)
{
  switch (id) {
    case Target_Primary_CELL_ID_8_4_4_1:
      printf("RAN Parameter Name = Target Primary Cell ID\n");
      break;
    
    case CHOICE_Target_CELL_8_4_4_1:
      printf("RAN Parameter Name = CHOICE Target Cell\n");
      break;

    case NR_CELL_8_4_4_1:
      printf("RAN Parameter Name = NR Cell\n");
      break;

    case NR_CGI_8_4_4_1:
      printf("RAN Parameter Name = NR CGI\n");
      break;

    case E_UTRA_CELL_8_4_4_1:
      printf("RAN Parameter Name = E-UTRA Cell\n");
      break;

    case E_UTRA_CGI_8_4_4_1:
      printf("RAN Parameter Name = E-UTRA CGI\n");
      break;

    default:
      printf("Add corresponding RAN Parameter ID\n");
  }
}


static
void sm_cb_rc_insert_handover_control(sm_ag_if_rd_t const* rd)
{
  assert(rd != NULL);
  assert(rd->type == INDICATION_MSG_AGENT_IF_ANS_V0);

  // Reading Indication Message Format 5
  e2sm_rc_ind_msg_frmt_5_t const* ind_msg_frmt_5 = &rd->ind.rc.ind.msg.frmt_5;

  static int counter = 1;
  {
    lock_guard(&mtx);

    printf("\n%7d RC Indication Message\n", counter);

    for (size_t i = 0; i < ind_msg_frmt_5->sz_seq_ran_param; i++) {
      seq_ran_param_t* const ran_param_item = &ind_msg_frmt_5->seq_ran_param[i];

        log_handover_control_ran_param_name(ran_param_item->ran_param_id);
        log_handover_control_ran_param_name(ran_param_item->ran_param_val.strct->ran_param_struct->ran_param_id);
        log_handover_control_ran_param_name(ran_param_item->ran_param_val.strct->ran_param_struct->ran_param_val.strct->ran_param_struct->ran_param_id);
        log_handover_control_ran_param_name(ran_param_item->ran_param_val.strct->ran_param_struct->ran_param_val.strct->ran_param_struct->ran_param_val.strct->ran_param_struct->ran_param_id);
        log_element_ran_param_value(ran_param_item->ran_param_val.strct->ran_param_struct->ran_param_val.strct->ran_param_struct->ran_param_val.strct->ran_param_struct->ran_param_val.flag_false);
    }

    counter++;
  }
}



static
rc_sub_data_t gen_rc_insert_sub_msg(ran_func_def_insert_t const* ran_func)
{
  assert(ran_func != NULL);

  rc_sub_data_t rc_sub = {0};

  for (size_t i = 0; i < ran_func->sz_seq_ins_sty; i++) {
    // as defined in section 7.5.4, formats used for SUBSCRIPTION msg are known
    assert(cmp_str_ba("Connected Mode Mobility Control Request", ran_func->seq_ins_sty[i].name) == 0 && "Add requested INSERT Style. At the moment, only Connected Mode Mobility Control Request supported");
    
    // size_t const sz = ran_func->seq_ins_sty[i].sz_seq_ins_ind;

    // Generate Event Trigger - Event Trigger Style 2
    rc_sub.et.format = ran_func->seq_ins_sty[i].ev_trig_style_type;
    assert(rc_sub.et.format == FORMAT_2_E2SM_RC_EV_TRIGGER_FORMAT && "Event Trigger Format received not valid");
    rc_sub.et.frmt_2.call_proc_type_id = 3;
    rc_sub.et.frmt_2.call_break_id = 1;

    // Generate Action Definition
    rc_sub.sz_ad = 1;
    rc_sub.ad = calloc(rc_sub.sz_ad, sizeof(e2sm_rc_action_def_t));
    assert(rc_sub.ad != NULL && "Memory exhausted");
    rc_sub.ad[0].ric_style_type = 3; // INSERT Service Style 3: Connected Mode Mobility Control Request
    rc_sub.ad[0].format = ran_func->seq_ins_sty[i].act_def_frmt_type;
    assert(rc_sub.ad[0].format == FORMAT_3_E2SM_RC_ACT_DEF && "Action Definition Format received not valid");
    
    // 8.4.4.1
    // Target Primary Cell ID, STRUCTURE (len 1)
    // >CHOICE Target Cell, STRUCTURE (len 2)
    // >>NR Cell, STRUCTURE (len 1)
    // >>>NR CGI, ELEMENT (len 1)
    // >>E-UTRA Cell, STRUCTURE (len 1)
    // >>>E-UTRA CGI, ELEMENT (len 1)
    
    rc_sub.ad[0].frmt_3.sz_ran_param_ins = 1;
    rc_sub.ad[0].frmt_3.ran_param = calloc(1, sizeof(ran_param_ins_t));
    assert(rc_sub.ad[0].frmt_3.ran_param != NULL && "Memory exhausted");
    rc_sub.ad[0].frmt_3.id = 1; // Insert Indication 1 Handover Control request

    rc_sub.ad[0].frmt_3.ran_param->ran_id=Target_Primary_CELL_ID_8_4_4_1;
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
  // INSERT SERVICE STYLE 3: Connected Mobility Request
  // INDICATION ID: Handover Control Request
  // Action Definition IE Format 3
  // E2SM-RC Indication Header Format 2
  // E2SM-RC Indication Message Format 5
  // Call Process ID Format 1
  int const RC_ran_function = 3;

  for (int i = 0; i < nodes.len; i++) {
    e2_node_connected_xapp_t* n = &nodes.n[i];

    size_t const idx = find_sm_idx(n->rf, n->len_rf, eq_sm, RC_ran_function);
    assert(n->rf[idx].defn.type == RC_RAN_FUNC_DEF_E && "RC is not the received RAN Function");
    // if INSERT Service is supported by E2 node, send SUBSCRIPTION message
    if (n->rf[idx].defn.rc.report != NULL) {
      // Generate RC SUBSCRIPTION message
      rc_sub_data_t rc_sub = gen_rc_insert_sub_msg(n->rf[idx].defn.rc.insert);

      hndl[i] = insert_sm_xapp_api(&n->id, RC_ran_function, &rc_sub, sm_cb_rc_insert_handover_control);
      assert(hndl[i].success == true);
      
      free_rc_sub_data(&rc_sub);
    }
  }
  ////////////
  // END RC
  ////////////

  sleep(5);

  for (int i = 0; i < nodes.len; ++i) {
    // Remove the handle previously returned
    if (hndl[i].success == true)
      rm_insert_sm_xapp_api(hndl[i].u.handle);
  }
  free(hndl);

  // Stop the xApp
  while (try_stop_xapp_api() == false)
    usleep(1000);

  printf("Test xApp run SUCCESSFULLY!\n");
}
