#include <libwebsockets.h>

#include "util/alg_ds/alg/find.h"
#include "ran_msg_hdlr.h"
#include "ran_if.h"
#include "util/alg_ds/alg/defer.h"
#include "proxy_agent.h"
#include "ringbuffer.h"
#include "ws_io_ran.h"

static int aperiodic_api_cnt = 0;

static
bool cmp_msg_id(const void* value, const void* n)
{
  int* v = (int*)value;
  int* it = (int*)n;
//  printf("v %d it %d \n", *v, *it);
  fflush(stdout);
  return *v == *it;
}

static
ws_ioloop_event_t* get_value_left_tree(bi_map_t* sent_msg_list, ran_msghdr_t msghdr){
  bml_iter_t const f = bi_map_front_left(sent_msg_list);
  bml_iter_t const l = bi_map_end_left(sent_msg_list);

  assert(l.it != f.it && "Zero size");
//  printf("Tree size = %zu \n", bi_map_size(sent_msg_list) );
//  fflush(stdout);
  assert(bi_map_size(sent_msg_list) != 0);
  bml_iter_t const bml_it = find_if_bi_map_left(sent_msg_list, f, l, &msghdr.msg_id, cmp_msg_id);
  assert(l.it != bml_it.it && "Key not found");
  return (ws_ioloop_event_t *)bi_map_value_left(sent_msg_list, bml_it);
}

static next_msg_t handle_ran_indication_stats(proxy_agent_t * proxy, ran_msg_t msg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list)
{
  (void)sent_msg_list;
  (void)msghdr;

  lwsl_info("[WS] Received indication data\n");

  next_msg_t ret_msg = {. type_id = RAN_E2_NONE };
  if (msghdr.msg_id != 0){
    ret_msg.type_id = RAN_E2_WRT_FWD;
    ws_ioloop_event_t *loop_event_p = get_value_left_tree(sent_msg_list, msghdr);
    assert (loop_event_p != NULL && "key not found in the io_loop datastructure. Should not happen.");
    if (loop_event_p->msg_type == E2_WRITE_SUBSCRIPTION_EVENT){
      ret_msg.e2wrt_msg.ric_req_id = loop_event_p->wr_subs_ev.ric_req_id;
      ret_msg.e2wrt_msg.rc = loop_event_p->wr_subs_ev.rc;
      ret_msg.e2wrt_msg.msg_id = msghdr.msg_id;
    } else {
      assert(0 != 0 && "error extract RC ric_req_id or not properly remove RC ric_req_id from RAN interface");
    }
    aperiodic_api_cnt += 1;
  }
  ran_ind_t ind = get_ringbuffer_data();
  ind.ran_config = proxy->ranConfig;
  if (get_IndTimer_sts() == false)
    set_IndTimer_sts(true);
  if (proxy->ran_if.ser->decode_indication_stats(&msg, &ind)  == true)
     put_ringbuffer_data(ind);
  else 
    lwsl_err("[WS] Error parsing json message for indication message from RAN. Discarding msg\n");
  return ret_msg;
}

static next_msg_t handle_ran_indication_ue(proxy_agent_t * proxy, ran_msg_t msg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list) 
{
  (void)msghdr;
  (void)sent_msg_list;

  lwsl_info("[WS] Received indication data\n");

  next_msg_t ret_msg = {.type_id = RAN_E2_NONE};
  if (msghdr.msg_id != 0){
    ret_msg.type_id = RAN_E2_WRT_FWD;
    ws_ioloop_event_t *loop_event_p = get_value_left_tree(sent_msg_list, msghdr);
    assert (loop_event_p != NULL && "key not found in the io_loop datastructure. Should not happen.");
    if (loop_event_p->msg_type == E2_WRITE_SUBSCRIPTION_EVENT){
      ret_msg.e2wrt_msg.ric_req_id = loop_event_p->wr_subs_ev.ric_req_id;
      ret_msg.e2wrt_msg.rc = loop_event_p->wr_subs_ev.rc;
      ret_msg.e2wrt_msg.msg_id = msghdr.msg_id;
    } else {
      assert(0 != 0 && "error extract RC ric_req_id or not properly remove RC ric_req_id from RAN interface");
    }
    aperiodic_api_cnt += 1;
  }
  ran_ind_t ind = get_ringbuffer_data();
  ind.ran_config = proxy->ranConfig;
  if (get_IndTimer_sts() == false)
    set_IndTimer_sts(true);
  if (proxy->ran_if.ser->decode_indication_ue_get(&msg, &ind)  == true)
     put_ringbuffer_data(ind);
  else 
    lwsl_err("[WS] Error parsing json message for indication message from RAN. Discarding msg\n");
  return ret_msg;
}

static next_msg_t handle_ran_indication_config_get(proxy_agent_t * proxy, ran_msg_t msg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list)
{
    (void)msghdr;
    (void)sent_msg_list;

    lwsl_info("[WS] Received indication data\n");

    next_msg_t ret_msg = {.type_id = RAN_E2_NONE};
    if (msghdr.msg_id != 0){
      ret_msg.type_id = RAN_E2_WRT_FWD;
      ws_ioloop_event_t *loop_event_p = get_value_left_tree(sent_msg_list, msghdr);
      assert (loop_event_p != NULL && "key not found in the io_loop datastructure. Should not happen.");
      if (loop_event_p->msg_type == E2_WRITE_SUBSCRIPTION_EVENT){
        ret_msg.e2wrt_msg.ric_req_id = loop_event_p->wr_subs_ev.ric_req_id;
        ret_msg.e2wrt_msg.rc = loop_event_p->wr_subs_ev.rc;
        ret_msg.e2wrt_msg.msg_id = msghdr.msg_id;
      } else {
        assert(0 != 0 && "error extract RC ric_req_id or not properly remove RC ric_req_id from RAN interface");
      }
      aperiodic_api_cnt += 1;
    }
    ran_ind_t ind = get_ringbuffer_data();
    if (get_IndTimer_sts() == false)
        set_IndTimer_sts(true);
    if (proxy->ran_if.ser->decode_indication_config_get(&msg, &ind)  == true)
        put_ringbuffer_data(ind);
    else
        lwsl_err("[WS] Error parsing json message for indication message from RAN. Discarding msg\n");
    return ret_msg;
}

static next_msg_t handle_ran_unimplemented(proxy_agent_t * proxy, ran_msg_t ranmsg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list) 
{
  (void)ranmsg;
  (void)proxy;
  (void)msghdr;
  (void)sent_msg_list;

  assert (0!=0 && "Not implemented\n");
}

/* 
 * Decode the CTRL reply received from the RAN and create a message to be sent back to E2 interface. 
 * The content of the reply dependes on SM, but it is usually just a yes/no
 */
static next_msg_t handle_ran_ctrl(proxy_agent_t * proxy, ran_msg_t ranmsg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list)
{
  (void)ranmsg;

  next_msg_t ret_msg = { .type_id = RAN_E2_CTRL_FWD};
  ws_ioloop_event_t *loop_event_p = bi_map_extract_left(sent_msg_list, &msghdr.msg_id, sizeof(int), NULL);
  // For some reason, need to free cause this cause memory leak.
  defer({free(loop_event_p);};);
  assert (loop_event_p != NULL && "key not found in the io_loop datastructure. Should not happen.");

  if (proxy->ran_if.ser->decode_ctrl(&msghdr, &ret_msg.ctrl_msg, loop_event_p) == false) 
  {
    lwsl_err("Error decoding json message received from RAN for CTRL command. Discarding msg\n");
    ret_msg.type_id = RAN_E2_NONE;
  }
  return ret_msg;
}

static next_msg_t handle_ran_ready(proxy_agent_t * proxy, ran_msg_t ranmsg, ran_msghdr_t msghdr,  bi_map_t *sent_msg_list)
{
  (void)ranmsg;
  (void)sent_msg_list;
  (void)msghdr;

  next_msg_t ret_msg; 
  lwsl_info("RAN is in READY state. You can send your commands!\n");
  
  if (get_E2setup_sts() == false) {
    ret_msg.type_id = ASK_RAN_SETUP;
    const int msg_id = 0;//  msg_id = 0 as we do not use it for this type of message the io_loop bi-map
    ret_msg.e2setup_msg.p = proxy->ran_if.ser->encode_e2setup(msg_id); 
  } else {
    ret_msg.type_id = RAN_E2_NONE;
    lwsl_warn("Asking again for a E2 setup is not implemented. Request discarded\n");
  }
  
  return ret_msg;
}
static next_msg_t handle_ran_config(proxy_agent_t * proxy, ran_msg_t ranmsg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list)
{
  (void)ranmsg;
  (void)msghdr;
  (void)sent_msg_list;

  set_E2setup_sts(true);
  next_msg_t ret_msg = { .type_id = RAN_E2_CONFIG_FWD};
  ran_ind_t ind = get_ringbuffer_data();
  if (proxy->ran_if.ser->decode_e2setup(&ranmsg, &proxy->ranConfig) == false){
    lwsl_err("Error parsing json message for setup request from RAN. Discarding msg\n");
    ret_msg.type_id = RAN_E2_NONE;
  }
  ret_msg.config_msg.global_e2_node_id = cp_global_e2_node_id(&proxy->ranConfig.global_e2_node_id);
  ind.global_e2_node_id = cp_global_e2_node_id(&ret_msg.config_msg.global_e2_node_id);
  put_ringbuffer_data(ind);
  return ret_msg;
}

// guess the type of message received from the endpoint
static bool ws_associate_valid_msg_type(ran_msghdr_t *hdr) 
{
  // XXX: now you have in hdr datastructure the 'hdr.msg_id' that you can leverage to extract the sm_id and 
  // typeid directly from the io_loop bi_map without the heuristic below.
  bool ret = true;

  if (!strcmp(hdr->type, "ready")){
      hdr->typeid = RAN_READY;
  } else if (!strcmp(hdr->type, "register")){
      hdr->typeid = RAN_REGISTRATION_ACK;
  } else if (!strcmp(hdr->type,"authenticate")){
      hdr->typeid = RAN_AUTHENTICATE_ACK;
  } else if (!strcmp(hdr->type, "config_get")){
      if (get_proxy_agent()->ranConfig.len_nr_cell){
          hdr->typeid = RAN_INDICATION_CONFGET_RECV;
      } else {
          hdr->typeid = RAN_CONFIG_RECV;
      }
  } else if (!strcmp(hdr->type, "stats")){
      hdr->typeid = RAN_INDICATION_STATS_RECV;
  } else if (!strcmp(hdr->type, "ue_get")){
      hdr->typeid = RAN_INDICATION_UEGET_RECV;
  } else if (!strcmp(hdr->type, "config_set")){
      hdr->typeid = RAN_CTRL_ACK;
  } else if (!strcmp(hdr->type, "handover")){
      hdr->typeid = RAN_CTRL_ACK;
  } else if (!strcmp(hdr->type, "cell_gain")){
      hdr->typeid = RAN_CTRL_ACK;
  } else{
      ret = false;
  }

  return ret;
 }

/* 
 * Table with function pointers to correlate messages received (ranmsg) with actions to be performed as a 
 * consequence (return value)
 */
typedef next_msg_t (*handle_msg_ran)(proxy_agent_t * pa, ran_msg_t rowmsg, ran_msghdr_t msghdr, bi_map_t *sent_msg_list);
static handle_msg_ran handle_msg[8] = {
  // Note that the remaining unspecified seats in this array are initialized automatically to NULL(0)
  [RAN_READY]                   = &handle_ran_ready,
  [RAN_REGISTRATION_ACK]        = &handle_ran_unimplemented,
  [RAN_AUTHENTICATE_ACK]        = &handle_ran_unimplemented,
  [RAN_CONFIG_RECV]             = &handle_ran_config,
  [RAN_INDICATION_UEGET_RECV]   = &handle_ran_indication_ue,
  [RAN_INDICATION_STATS_RECV]   = &handle_ran_indication_stats,
  [RAN_INDICATION_CONFGET_RECV] = &handle_ran_indication_config_get,
  [RAN_CTRL_ACK]                = &handle_ran_ctrl
};

next_msg_t ran_msg_handle(const char *buf, size_t len, bi_map_t *sent_msg_list)
{
  // XXX: improve encapsulation of 'proxy_agent' variable, i.e. maybe passing directly in the ran_msg_handle signature
  next_msg_t ret = {.type_id = RAN_E2_NONE};
  ran_msg_t ran_rowmsg = { .buf = buf, .len = len};
 
  ran_ser_abs_t *serializer = get_proxy_agent()->ran_if.ser;
  ran_msghdr_t msghdr = serializer->get_ran_msghdr(&ran_rowmsg);
  defer({serializer->free_ran_msghdr(msghdr); }; );
      
  lwsl_debug("RAN hdr decoded: (type=%s, message_id=%d)\n",  msghdr.type, msghdr.msg_id);
  if (ws_associate_valid_msg_type(&msghdr) == false) 
  {
    lwsl_err("discarding unexpected received msg type: (%s)\n",  msghdr.type);
    return ret;
  }
  assert(handle_msg [msghdr.typeid] != NULL);
  return handle_msg[msghdr.typeid](get_proxy_agent(), ran_rowmsg, msghdr, sent_msg_list); 
}

bool is_get_aperiodic_event(void){
  // Get 3 API from indication
  if (aperiodic_api_cnt == 2){
    aperiodic_api_cnt = 0;
    return true;
  }
  return false;
}