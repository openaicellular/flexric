#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#include "lib/ap/e2ap_types/common/e2ap_global_node_id.h"
#include "util/alg_ds/alg/defer.h"

// proxy agent specific headers
#include "ws_msg_hdlr.h"
#include "e2_sm_agent.h"
#include "msg_json_ws.h"
#include "proxy_agent.h"
#include "notif_e2_ws.h"
#include "ringbuffer.h"


/* check for validity of the type field extracted from json reply .
 * Example of message types: authenticate, register, ready, ue_get, stats, config_get...etc. 
 * See  chapter 9.7 of amarisoft manual for complete list
 *
 */
static bool ws_associate_valid_msg_type(ws_msghdr_t *hdr) 
{
  bool ret = true;

  if (!strcmp(hdr->type, "ready"))
    hdr->typeid = WS_READY;
  else if (!strcmp(hdr->type, "register"))
    hdr->typeid = WS_REGISTRATION_ACK;
  else if (!strcmp(hdr->type,"authenticate"))
    hdr->typeid = WS_AUTHENTICATE_ACK;
  else if (!strcmp(hdr->type, "config_get"))
     hdr->typeid = WS_CONFIG_RECV;
  else if (!strcmp(hdr->type, "stats"))
    hdr->typeid = WS_INDICATION_RECV;
  else 
    ret = false;
  //  XXX: to be implemented the handling of WS_CTRL_ACK (i.e. could be from a config_set reply)
  return ret;
 }

 
/* 
 * Main handler for parsing the messages received on WS interface and act upon (i.e. send a reply for the E2 interface)
 *
 * RETURN:
 * message/action to be sent to E2 interface. If message = WS_NONE, it means that nothing is expected to be done from the calling routine
 */
e2_agent_msg_t ws_msg_handle(proxy_agent_t *p, const ws_msg_t* msg)
{
  e2_agent_msg_t ret_msg = {.type_id = WS_E2_NONE};
  ws_msghdr_t msgdec = ws_json_get_msghdr(msg);
  defer({ws_msghdr_free(msgdec); }; );
  
  lwsl_info("WS msg hdr: (%s, %d)\n",  msgdec.type, msgdec.msg_id);
  if (ws_associate_valid_msg_type(&msgdec) == false) 
  {
    lwsl_err("discarding unexpected received msg type: (%s)\n",  msgdec.type);
    return ret_msg;
  }
  

  // XXX: extract from bimap_pending_t  the correlated message type and service model that originated this request thanks to the msg_id;
  
  // XXX: handle the various messages with callback mechanism to make it cleaner instead of the 'switch' below
  // Cf. e2_agent_msg_t ws_handle_e2setup(near_ric_t* ric, const e2ap_msg_t* msg)
  // decoding error  inside handling routines will managed as printf and discarding the return action with ret_msg.type = WS_NONE
  switch (msgdec.typeid) 
  {
    case WS_READY:
      ret_msg.type_id = WS_E2_SETUP;
      lwsl_info("RAN is ready to receive your commands!\n");
      break;
    case WS_REGISTRATION_ACK:
      lwsl_info("REGISTERED ack\n");
      break;
    case WS_AUTHENTICATE_ACK:
      assert(0!=0 && "Authentication required but not yet implemented!\n");
    case WS_CONFIG_RECV:  
      lwsl_info("Received e2_setup info from RAN. Send it to RIC via E2 interface\n");
      ret_msg.type_id = WS_E2_CONFIG_FWD;
      global_e2_node_id_t id;
      if (ws_json_decode_e2setup(msg, &id) == true) 
        fwd_ws_e2_setup_request(p, id);
      else {
        lwsl_err("Error parsing json message for setup request from RAN. Discarding msg\n");
      }
      break;
    case WS_INDICATION_RECV:
      lwsl_info("[WS] Received indication data\n");
      ws_ind_t ind = {0};
      p->ran_if.ind_timer_ready = true;
      if (ws_json_decode_indication (msg, &ind)  == true)
        put_ringbuffer_data(ind);
      else 
        lwsl_err("[WS] Error parsing json message for indication message from RAN. Discarding msg \n");
      
      ret_msg.type_id = WS_E2_NONE; // no reply to be sent to E2 interface as it will read indipendently the indication data with its own timer
      break;
    case WS_CTRL_ACK:
    default:
      assert(0!=0 && "BUG: unimplemented message type handling!\n");
  }
  return ret_msg;
}

