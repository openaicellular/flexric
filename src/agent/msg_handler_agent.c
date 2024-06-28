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
 *
*/

#include "msg_handler_agent.h"
#include "lib/ind_event.h"
#include "lib/pending_events.h"
#include "sm/sm_agent.h"
#include "util/alg_ds/alg/alg.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"
#include "util/compare.h"

#include <assert.h>
#include <stdio.h>

static
bool check_valid_msg_type(e2_msg_type_t msg_type )
{
  return msg_type == RIC_SUBSCRIPTION_REQUEST
      || msg_type == RIC_SUBSCRIPTION_DELETE_REQUEST
      || msg_type == RIC_CONTROL_REQUEST
      || msg_type == E2AP_ERROR_INDICATION
      || msg_type == E2_SETUP_RESPONSE
      || msg_type == E2_SETUP_FAILURE
      || msg_type == E2AP_RESET_REQUEST
      || msg_type == E2AP_RESET_RESPONSE
      || msg_type == RIC_SERVICE_UPDATE_ACKNOWLEDGE
      || msg_type == RIC_SERVICE_UPDATE_FAILURE
      || msg_type == RIC_SERVICE_QUERY
      || msg_type == E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE
      || msg_type == E2_NODE_CONFIGURATION_UPDATE_FAILURE
      || msg_type == E2_CONNECTION_UPDATE;
}

static inline
bool not_aperiodic_ind_event(int fd)
{
  assert(fd > -1);

  // 0 value used for aperiodic indication events
  return fd != 0;
}

static
bool stop_ind_event(e2_agent_t* ag, ric_gen_id_t id)
{
  assert(ag != NULL);
  ind_event_t tmp = {.ric_id = id, .sm = NULL, .action_id =0 };

  // Fix this! bi_map should liberate the memory itself 
  void* start_r = assoc_rb_tree_front(&ag->ind_event.right);
  void* end_r = assoc_rb_tree_end(&ag->ind_event.right);
  void* it_r = find_if_rb_tree(&ag->ind_event.right, start_r, end_r, &tmp, eq_ind_event); 
  if(it_r == end_r){
    printf("[E2 AGENT]: RAN_FUNC_ID %d RIC_REQ_ID %d not found. Spuriously occurs when abruptly closing the xApp\n", id.ran_func_id, id.ric_req_id);
    return false;;
  }

  assert(it_r != end_r);
  ind_event_t* ind_ev = assoc_rb_tree_key(&ag->ind_event.right, it_r);

  // These 4 lines need refactoring
  if(ind_ev->sm->free_act_def != NULL)
    ind_ev->sm->free_act_def(ind_ev->sm, ind_ev->act_def);
  //
  if(ind_ev->type == APERIODIC_SUBSCRIPTION_FLRC)
    ind_ev->free_subs_aperiodic(id.ric_req_id);


  void (*free_ind_event)(void*) = NULL;
  int* fd = bi_map_extract_right(&ag->ind_event, &tmp, sizeof(tmp), free_ind_event);
  assert(*fd > -1);
  //printf("fd value in stopping pending event = %d \n", *fd);
 
  if(not_aperiodic_ind_event(*fd))
    rm_fd_asio_agent(&ag->io, *fd);
  free(fd);

  return true;;
}

void init_handle_msg_agent(size_t len, handle_msg_fp_agent (*handle_msg)[len])
{
  assert(len == NONE_E2_MSG_TYPE);

  memset((*handle_msg), 0, sizeof(handle_msg_fp_agent)*len);

  (*handle_msg)[RIC_SUBSCRIPTION_REQUEST] = e2ap_handle_subscription_request_agent;
  (*handle_msg)[RIC_SUBSCRIPTION_DELETE_REQUEST] =  e2ap_handle_subscription_delete_request_agent;
  (*handle_msg)[RIC_CONTROL_REQUEST] = e2ap_handle_control_request_agent;
  (*handle_msg)[E2AP_ERROR_INDICATION] = e2ap_handle_error_indication_agent;
  (*handle_msg)[E2_SETUP_RESPONSE] =  e2ap_handle_setup_response_agent;
  (*handle_msg)[E2_SETUP_FAILURE] =  e2ap_handle_setup_failure_agent;
  (*handle_msg)[E2AP_RESET_REQUEST] =  e2ap_handle_reset_request_agent;
  (*handle_msg)[E2AP_RESET_RESPONSE] =  e2ap_handle_reset_response_agent;
  (*handle_msg)[RIC_SERVICE_UPDATE_ACKNOWLEDGE] =  e2ap_handle_service_update_ack_agent;
  (*handle_msg)[RIC_SERVICE_UPDATE_FAILURE] =  e2ap_handle_service_update_failure_agent;
  (*handle_msg)[RIC_SERVICE_QUERY] = e2ap_handle_service_query_agent;
  (*handle_msg)[E2_NODE_CONFIGURATION_UPDATE_ACKNOWLEDGE] =  e2ap_handle_node_configuration_update_ack_agent;
  (*handle_msg)[E2_NODE_CONFIGURATION_UPDATE_FAILURE] =  e2ap_handle_node_configuration_update_failure_agent;
  (*handle_msg)[E2_CONNECTION_UPDATE] =  e2ap_handle_connection_update_agent;
}

e2ap_msg_t e2ap_msg_handle_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(msg != NULL);
  const e2_msg_type_t msg_type = msg->type;
  assert(check_valid_msg_type(msg_type) == true);
  assert(ag->handle_msg[ msg_type ] != NULL);
  return ag->handle_msg[msg_type](ag, msg); 
}

static inline
bool supported_ric_subscription_request(ric_subscription_request_t const* sr)
{
  assert(sr != NULL);
  assert(sr->len_action == 1 && "Only one action supported" );
  assert(sr->action->type == RIC_ACT_REPORT && "Only report supported" );
  return true;
}

static
sm_subs_data_t generate_sm_subs_data(ric_subscription_request_t const* sr)
{
  assert(sr != NULL);
  sm_subs_data_t data =  { .event_trigger = sr->event_trigger.buf,
                           .len_et = sr->event_trigger.len,
                           .ric_req_id = sr->ric_id.ric_req_id };

  if(sr->action->definition != NULL){
    data.action_def = sr->action->definition->buf;
    data.len_ad = sr->action->definition->len;
  }

  return data;
}

static
ric_subscription_response_t generate_subscription_response(ric_gen_id_t const* ric_id, uint8_t ric_act_id)
{
  ric_subscription_response_t sr = {
    .ric_id = *ric_id,
    .not_admitted = 0,
    .len_na = 0,
  }; 
  sr.admitted = calloc(1, sizeof(ric_action_admitted_t));
  assert(sr.admitted != NULL && "Memory exahusted");
  sr.admitted->ric_act_id = ric_act_id; 
  sr.len_admitted = 1;

  return sr; 
}

e2ap_msg_t e2ap_handle_subscription_request_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_REQUEST);

  ric_subscription_request_t const* sr = &msg->u_msgs.ric_sub_req;
  assert(supported_ric_subscription_request(sr) == true);

  printf("[E2 AGENT]: RIC_SUBSCRIPTION_REQUEST rx RAN_FUNC_ID %d RIC_REQ_ID %d\n", sr->ric_id.ran_func_id, sr->ric_id.ric_req_id);

  sm_subs_data_t data = generate_sm_subs_data(sr);
  uint16_t const ran_func_id = sr->ric_id.ran_func_id; 
  sm_agent_t* sm = sm_plugin_ag(&ag->plugin, ran_func_id);
  
  //subscribe_timer_t t = sm->proc.on_subscription(sm, &data);
  //assert(t.ms > -2 && "Bug? 0 = create pipe value"); 
  
  sm_ag_if_ans_subs_t const subs = sm->proc.on_subscription(sm, &data);

  // Register the indication event
  ind_event_t ev = {0};
  ev.action_id = sr->action[0].id;
  ev.ric_id = sr->ric_id;
  ev.sm = sm;
  ev.type = subs.type; 

  if(ev.type == PERIODIC_SUBSCRIPTION_FLRC){
    subscribe_timer_t const t = subs.per.t; 
    ev.act_def = t.act_def;
    // Periodic indication message generated i.e., every 5 ms
    assert(t.ms < 10001 && "Subscription for granularity larger than 10 seconds requested? ");
    int fd_timer = create_timer_ms_asio_agent(&ag->io, t.ms, t.ms); 
    lock_guard(&ag->mtx_ind_event);
    bi_map_insert(&ag->ind_event, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev));
  } else if(ev.type == APERIODIC_SUBSCRIPTION_FLRC){
    ev.free_subs_aperiodic = subs.aper.free_aper_subs;
    // Aperiodic indication generated i.e., the RAN will generate it via 
    // void async_event_agent_api(uint32_t ric_req_id, void* ind_data);
    int fd = 0;
    lock_guard(&ag->mtx_ind_event);
    bi_map_insert(&ag->ind_event, &fd, sizeof(int), &ev, sizeof(ev));
  } else {
    assert(0!=0 && "Unknown subscritpion timer value");
  }

  printf("[E2-AGENT]: RIC_SUBSCRIPTION_REQUEST rx\n");

  uint8_t const ric_act_id = sr->action[0].id;
  e2ap_msg_t ans = {.type = RIC_SUBSCRIPTION_RESPONSE, 
                    .u_msgs.ric_sub_resp = generate_subscription_response(&sr->ric_id, ric_act_id) };
  return ans;
}

e2ap_msg_t e2ap_handle_subscription_delete_request_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_SUBSCRIPTION_DELETE_REQUEST);

  const ric_subscription_delete_request_t* sdr = &msg->u_msgs.ric_sub_del_req;

  printf("[E2-AGENT]: RIC_SUBSCRIPTION_DELETE_REQUEST rx RAN_FUNC_ID %d  RIC_REQ_ID %d  \n", sdr->ric_id.ran_func_id, sdr->ric_id.ric_req_id);

  stop_ind_event(ag, sdr->ric_id);
  //bool const found_ind_event = stop_ind_event(ag, sdr->ric_id);
  //if(found_ind_event == false){
  //  return ( e2ap_msg_t ){.type = NONE_E2_MSG_TYPE };
  //}

  ric_subscription_delete_response_t sub_del = {.ric_id = sdr->ric_id };

  e2ap_msg_t ans = {.type = RIC_SUBSCRIPTION_DELETE_RESPONSE};
  ans.u_msgs.ric_sub_del_resp = sub_del; 

  return ans; 
}


static
byte_array_t* ba_from_ctrl_out(sm_ctrl_out_data_t const* data)
{
  assert(data != NULL);
  byte_array_t* ba = NULL; 

  size_t const sz = data->len_out; 
  if(sz > 0){
    ba = malloc(sizeof(byte_array_t ) ) ;
    assert(ba != NULL && "Memory exhausted!");

    ba->len = sz;
    ba->buf = malloc(sz);
    assert(ba->buf != NULL && "Memory exhausted");
    memcpy(ba->buf, data->ctrl_out, sz);
  }

 return ba; 
}

// The purpose of the RIC Control procedure is to initiate or resume a specific functionality in the E2 Node.
e2ap_msg_t e2ap_handle_control_request_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(msg->type == RIC_CONTROL_REQUEST);
 
  ric_control_request_t const* ctrl_req = &msg->u_msgs.ric_ctrl_req;
  assert(ctrl_req->ack_req != NULL && *ctrl_req->ack_req == RIC_CONTROL_REQUEST_ACK );

  sm_ctrl_req_data_t data = {.ctrl_hdr = ctrl_req->hdr.buf,
                          .len_hdr = ctrl_req->hdr.len,
                          .ctrl_msg = ctrl_req->msg.buf,
                          .len_msg = ctrl_req->msg.len}; 


  uint16_t const ran_func_id = ctrl_req->ric_id.ran_func_id; 
  sm_agent_t* sm = sm_plugin_ag(&ag->plugin, ran_func_id);

  sm_ctrl_out_data_t ctrl_ans = sm->proc.on_control(sm, &data);
  defer({ free_sm_ctrl_out_data(&ctrl_ans); } );


  byte_array_t* ba_ctrl_ans = ba_from_ctrl_out(&ctrl_ans);

  ric_control_acknowledge_t ric_ctrl_ack = {.ric_id = ctrl_req->ric_id,
                                            .call_process_id = NULL,
#ifdef E2AP_V1
                                            .status = RIC_CONTROL_STATUS_SUCCESS,
#endif                                            
                                            .control_outcome = ba_ctrl_ans } ;

  printf("[E2-AGENT]: CONTROL ACKNOWLEDGE tx\n");
  e2ap_msg_t ans = {.type = RIC_CONTROL_ACKNOWLEDGE};
  ans.u_msgs.ric_ctrl_ack = ric_ctrl_ack;

  return ans; 
}

e2ap_msg_t e2ap_handle_error_indication_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {0};
  return ans; 
}

static
void stop_pending_event(e2_agent_t* ag, pending_event_t event)
{
  assert(ag != NULL);

  void (*free_pending_event)(void*)=NULL;
  int* fd = bi_map_extract_right(&ag->pending, &event, sizeof(event), free_pending_event);
  assert(*fd > 0);
  //printf("[E2-AGENT]: stopping pending\n");
  //event = %d \n", *fd);
  rm_fd_asio_agent(&ag->io, *fd);
  free(fd);
}


e2ap_msg_t e2ap_handle_setup_response_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_RESPONSE);
  printf("[E2-AGENT]: E2 SETUP RESPONSE rx\n");

  // Stop the timer
  pending_event_t ev = SETUP_REQUEST_PENDING_EVENT;
  stop_pending_event(ag,ev);

#if defined(E2AP_V2) || defined(E2AP_V3)
  assert(ag->trans_id_setup_req > 0 && "Receiving an E2 SETUP-RESPONSE, eventhough not E2 SETUP-REQUEST not sent from this E2 Node" );
  printf("[E2-AGENT]: Transaction ID E2 SETUP-REQUEST %u E2 SETUP-RESPONSE %u \n", --ag->trans_id_setup_req, msg->u_msgs.e2_stp_resp.trans_id);
  ag->trans_id_setup_req = 0;
#endif
  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_setup_failure_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(msg->type == E2_SETUP_FAILURE);

	char const* str[] = { 
   "CAUSE_NOTHING",	/* NO COMPONENTS PRESENT */
	 "CAUSE_RICREQUEST",
	 "CAUSE_RICSERVICE",
	 "CAUSE_TRANSPORT",
	 "CAUSE_PROTOCOL",
	 "CAUSE_MISC"
  };

  const char* str2[6][11] = { 
    {
      "CAUSE_NOTHING",
    },
    {
      "CAUSE_RIC_RAN_FUNCTION_ID_INVALID",
      "CAUSE_RIC_ACTION_NOT_SUPPORTED",
      "CAUSE_RIC_EXCESSIVE_ACTIONS",
      "CAUSE_RIC_DUPLICATE_ACTION",
      "CAUSE_RIC_DUPLICATE_EVENT",
      "CAUSE_RIC_FUNCTION_RESOURCE_LIMIT",
      "CAUSE_RIC_REQUEST_ID_UNKNOWN",
      "CAUSE_RIC_INCONSISTENT_ACTION_SUBSEQUENT_ACTION_SEQUENCE",
      "CAUSE_RIC_CONTROL_MESSAGE_INVALID",
      "CAUSE_RIC_CALL_PROCESS_ID_INVALID",
      "CAUSE_RIC_UNSPECIFIED"	
    }, 
    {
      "CAUSE_RICSERVICE_FUNCTION_NOT_REQUIRED",
      "CAUSE_RICSERVICE_EXCESSIVE_FUNCTIONS",
      "CAUSE_RICSERVICE_RIC_RESOURCE_LIMIT"
    },
    {
      "CAUSE_TRANSPORT_UNSPECIFIED",
      "CAUSE_TRANSPORT_TRANSPORT_RESOURCE_UNAVAILABLE"
    },
    {
      "CAUSE_PROTOCOL_TRANSFER_SYNTAX_ERROR",
      "CAUSE_PROTOCOL_ABSTRACT_SYNTAX_ERROR_REJECT",
      "CAUSE_PROTOCOL_ABSTRACT_SYNTAX_ERROR_IGNORE_AND_NOTIFY",
      "CAUSE_PROTOCOL_MESSAGE_NOT_COMPATIBLE_WITH_RECEIVER_STATE",
      "CAUSE_PROTOCOL_SEMANTIC_ERROR",
      "CAUSE_PROTOCOL_ABSTRACT_SYNTAX_ERROR_FALSELY_CONSTRUCTED_MESSAGE",
      "CAUSE_PROTOCOL_UNSPECIFIED"
    },
    {
      "CAUSE_MISC_CONTROL_PROCESSING_OVERLOAD",
      "CAUSE_MISC_HARDWARE_FAILURE",
      "CAUSE_MISC_OM_INTERVENTION",
      "CAUSE_MISC_UNSPECIFIED"
    }
  };

  e2_setup_failure_t const* e2_stp_fail = &msg->u_msgs.e2_stp_fail;
	int const present = e2_stp_fail->cause.present;
  assert(present < 6);
  int const idx = e2_stp_fail->cause.ricRequest;
  assert(idx < 11);
  printf("[E2-AGENT]: E2 SETUP FAILURE rx %s %s\n", str[present], str2[present][idx]);

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_reset_request_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_reset_response_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}
  
e2ap_msg_t e2ap_handle_service_update_ack_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_service_update_failure_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_service_query_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_node_configuration_update_ack_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_node_configuration_update_failure_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

e2ap_msg_t e2ap_handle_connection_update_agent(e2_agent_t* ag, const e2ap_msg_t* msg)
{
  assert(ag != NULL);
  assert(msg != NULL);
  assert(0!=0 && "Not implemented");

  e2ap_msg_t ans = {.type = NONE_E2_MSG_TYPE};
  return ans; 
}

