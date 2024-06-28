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


#include <assert.h>
#include <pthread.h>
#include <stdio.h>

#include "endpoint_ric.h"
#include "msg_handler_ric.h"
#include "util/alg_ds/alg/alg.h"
#include "util/compare.h"
#include "util/ngran_types.h"
#include "e2ap_ric.h"
#include "near_ric.h"
#include "e2_node.h"
#include "generate_setup_response.h"
#include "generate_setup_failure.h"
#include "lib/pending_events.h"
#include "lib/pending_event_ric.h"
#include "lib/e2ap/e2ap_msg_free_wrapper.h"
#include "iApps/subscription_ric.h"
#include "iApp/e42_iapp_api.h"

#include "util/alg_ds/ds/lock_guard/lock_guard.h"

static inline
bool check_valid_msg_type(e2_msg_type_t msg_type)
{
  if(msg_type == RIC_SUBSCRIPTION_RESPONSE ||
      msg_type == RIC_SUBSCRIPTION_FAILURE ||
      msg_type == RIC_SUBSCRIPTION_DELETE_RESPONSE ||
      msg_type == RIC_SUBSCRIPTION_DELETE_FAILURE ||
      msg_type == RIC_INDICATION ||
      msg_type == RIC_CONTROL_ACKNOWLEDGE ||
      msg_type == RIC_CONTROL_FAILURE ||
      msg_type == E2AP_ERROR_INDICATION ||
      msg_type == E2_SETUP_REQUEST ||
      msg_type == E2AP_RESET_REQUEST ||
      msg_type == E2AP_RESET_RESPONSE ||
      msg_type == RIC_SERVICE_UPDATE ||
      msg_type == E2_NODE_CONFIGURATION_UPDATE ||
      msg_type == E2_CONNECTION_UPDATE_ACKNOWLEDGE ||
      msg_type == E2_CONNECTION_UPDATE_FAILURE )
    return true;

  return false;
}

static
void stop_pending_event(near_ric_t* ric, pending_event_ric_t* ev )
{
  assert(ric != NULL);
  assert(ev != NULL);

  int rc = pthread_mutex_lock(&ric->pend_mtx);
  assert(rc == 0);
  void (*free_pending_event)(void*) = NULL; 
  int* fd = bi_map_extract_right(&ric->pending, ev, sizeof(*ev), free_pending_event);
  rc = pthread_mutex_unlock(&ric->pend_mtx);
  assert(rc == 0);

//  assert(bi_map_size(&ric->pending) == 0 && "Just one SM supported");
  assert(*fd > 0);
  //printf("fd value in stopping pending event = %d \n", *fd);
  rm_fd_asio_ric(&ric->io, *fd);
  free(fd);
}

e2ap_msg_t e2ap_msg_handle_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  const e2_msg_type_t msg_type = msg->type;
  assert(check_valid_msg_type(msg_type) == true);

  assert(ric->handle_msg[ msg_type ] != NULL);
  return ric->handle_msg[msg_type](ric, msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// E2 -> RIC 
 e2ap_msg_t e2ap_handle_subscription_response_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_RESPONSE);

  ric_subscription_response_t const* resp = &msg->u_msgs.ric_sub_resp;

  pending_event_ric_t ev = {.ev = SUBSCRIPTION_REQUEST_PENDING_EVENT, .id = resp->ric_id }; 
  stop_pending_event(ric, &ev);

  assert(resp->len_na == 0 && "No other case implemented");
  assert(resp->len_admitted == 1 && "No other case implemented");
  assert(resp->admitted[0].ric_act_id == 0 && "No other case implemented");

  // Active Request
//  act_req_t req = {.id = resp->ric_id};

//  int rc_mtx = pthread_mutex_lock(&ric->act_req_mtx);
//  assert(rc_mtx == 0);
//  seq_push_back(&ric->act_req, &req, sizeof(req) );
//  rc_mtx = pthread_mutex_unlock(&ric->act_req_mtx);
//  assert(rc_mtx == 0);

#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

//E2 -> RIC 
 e2ap_msg_t e2ap_handle_subscription_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_FAILURE);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_subscription_delete_response_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_RESPONSE);
  
  ric_subscription_delete_response_t const* resp = &msg->u_msgs.ric_sub_del_resp;

  pending_event_ric_t ev = {.ev = SUBSCRIPTION_DELETE_REQUEST_PENDING_EVENT, .id = resp->ric_id }; 
  stop_pending_event(ric, &ev);

#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif
  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

//E2 -> RIC
 e2ap_msg_t e2ap_handle_subscription_delete_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_FAILURE);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE };
  return ans;
}

static
void publish_ind_msg(near_ric_t* ric,  uint16_t ran_func_id, sm_ag_if_rd_ind_t* d)
{

  // find RIC request ID, pass the data to the assoc. SM
  void* start_it = assoc_front(&ric->pub_sub);
  void* end_it = assoc_end(&ric->pub_sub);

  void* sm_it = find_if(&ric->pub_sub, start_it, end_it, &ran_func_id, eq_ran_func_id);
  assert(sm_it != end_it && "Could not find a RAN function that matches the SM");


  if(sm_it != end_it){
    seq_arr_t* arr = assoc_value(&ric->pub_sub, start_it);  
    void* it = seq_front(arr);
    void* it_end = seq_end(arr);
    while(it != it_end){
      subs_ric_t* sub = (subs_ric_t*)it;
      sub->fp(d);
      it = seq_next(arr, it);
    }
    start_it = assoc_next(&ric->pub_sub, start_it);
  }
 
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_indication_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_INDICATION);

  ric_indication_t const* ric_ind = &msg->u_msgs.ric_ind;

  const uint16_t ran_func_id = ric_ind->ric_id.ran_func_id;  
  sm_ric_t* sm = sm_plugin_ric(&ric->plugin, ran_func_id);

  sm_ind_data_t data = {.ind_hdr = ric_ind->hdr.buf,
                        .len_hdr = ric_ind->hdr.len,
                        .ind_msg = ric_ind->msg.buf,
                        .len_msg = ric_ind->msg.len,
  };
  if(ric_ind->call_process_id != NULL){
    data.call_process_id = ric_ind->call_process_id->buf;
    data.len_cpid = ric_ind->call_process_id->len;
  }

  sm_ag_if_rd_ind_t d = sm->proc.on_indication(sm, &data);
  defer({ sm->alloc.free_ind_data(&d); } );
  assert(d.type == MAC_STATS_V0 || d.type == RLC_STATS_V0 
        || d.type == PDCP_STATS_V0 || d.type == SLICE_STATS_V0 
        || d.type == KPM_STATS_V3_0 || d.type == RAN_CTRL_STATS_V1_03 
        || d.type == GTP_STATS_V0 || d.type == TC_STATS_V0 );

  publish_ind_msg(ric, ran_func_id, &d);

  // Notify the iApp
#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE };
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_control_ack_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_ACKNOWLEDGE);

  ric_control_acknowledge_t const* ack = &msg->u_msgs.ric_ctrl_ack;
#ifdef E2AP_V1 
  assert( ack->status == RIC_CONTROL_STATUS_SUCCESS && "Only success supported ") ;
#endif

  pending_event_ric_t ev = {.ev = CONTROL_REQUEST_PENDING_EVENT, .id = ack->ric_id }; 
  stop_pending_event(ric, &ev);

  printf("[NEAR-RIC]: CONTROL ACKNOWLEDGE rx\n");

#ifndef TEST_AGENT_RIC  
  notify_msg_iapp_api(msg);
#endif

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_control_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_FAILURE);
  assert(0!=0 && "not implemented" );

  e2ap_msg_t ans = {0};
  return ans;
}
  
///////////////////////////////////////////////////////////////////////////////////////////////////
// O-RAN E2APv01.01: Messages for Global Procedures ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// RIC <-> E2 
 e2ap_msg_t e2ap_handle_error_indication_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2AP_ERROR_INDICATION);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_setup_request_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_REQUEST); 

  const e2_setup_request_t* req = &msg->u_msgs.e2_stp_req;

  //  Working E2_SETUP_FAILURE 
  //  e2ap_msg_t ans = {.type = E2_SETUP_FAILURE };
  //  ans.u_msgs.e2_stp_fail = generate_setup_failure(&ric->ap.version.type, ric, req); 

  const e2ap_plmn_t* plmn = &req->id.plmn;
  char* ran_type = get_ngran_name(req->id.type);

#if defined(E2AP_V2) || defined(E2AP_V3)
  if(req->id.type == ngran_gNB && req->len_cca == 2){
    ran_type = "ngran_gNB_CU";
  }
  else if (req->id.type == ngran_gNB && req->len_cca == 3) {
    ran_type = "ngran_gNB_CUCP";
  }
#endif

  if (NODE_IS_MONOLITHIC(req->id.type))
    printf("[E2AP]: E2 SETUP-REQUEST rx from PLMN %3d.%*d Node ID %d RAN type %s\n", plmn->mcc, plmn->mnc_digit_len, plmn->mnc, req->id.nb_id.nb_id, ran_type);
  else
    printf("[E2AP]: E2 SETUP-REQUEST rx from PLMN %3d.%*d Node ID %d RAN type %s ID %ld\n", plmn->mcc, plmn->mnc_digit_len, plmn->mnc, req->id.nb_id.nb_id, ran_type, *req->id.cu_du_id);

  // Add the E2 Node into the iApp
#ifdef E2AP_V1
  add_e2_node_iapp_api_v1((global_e2_node_id_t*)&req->id, req->len_rf, req->ran_func_item);
#else
  add_e2_node_iapp_api((global_e2_node_id_t*)&req->id, req->len_rf, req->ran_func_item, req->len_cca, req->comp_conf_add);
#endif
  e2ap_msg_t ans = {.type = E2_SETUP_RESPONSE };
  ans.u_msgs.e2_stp_resp = generate_setup_response(&ric->ap.version.type, ric, req); 

  e2_node_t n = {0};
  init_e2_node(&n, &req->id, ans.u_msgs.e2_stp_resp.len_acc, ans.u_msgs.e2_stp_resp.accepted); 

  lock_guard(&ric->conn_e2_nodes_mtx);
  seq_push_back(&ric->conn_e2_nodes, &n, sizeof(n));

  return ans;
}

// RIC <-> E2
 e2ap_msg_t e2ap_handle_reset_request_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2AP_RESET_REQUEST);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// RIC <-> E2
 e2ap_msg_t e2ap_handle_reset_response_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2AP_RESET_RESPONSE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}
  
// E2 -> RIC
 e2ap_msg_t e2ap_handle_service_update_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SERVICE_UPDATE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_node_configuration_update_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_NODE_CONFIGURATION_UPDATE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {0};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_connection_update_ack_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE_ACKNOWLEDGE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

// E2 -> RIC
 e2ap_msg_t e2ap_handle_connection_update_failure_ric(near_ric_t* ric, const e2ap_msg_t* msg)
{
  assert(ric != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_CONNECTION_UPDATE_FAILURE);
  assert(0 != 0 && "Not Implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans;
}

