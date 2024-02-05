#define PROXY_AGENT

#include <stdio.h>
#include <string.h>
#include <libwebsockets.h>

#include "agent/e2_agent_api.h"
#include "e2_sm_agent.h"
#include "proxy_agent.h"
#include "notif_e2_ran.h"
#include "lib/async_event.h"

#include "lib/e2ap/ric_control_ack_wrapper.h"
#include "lib/e2ap/e2ap_msg_free_wrapper.h"
#include "lib/correlation_events.h"
#include "util/alg_ds/alg/defer.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"
#include "sm/mac_sm/mac_sm_id.h"

static notif_e2_ran_event_t static_notif_e2_ran_event; 

/* 
 * Send a notification to the RAN
 * 
 * For the storage that enable communication of RAN asio with outside world, I use a thread safe queue (`tsnq_t`) to store a 
 * generic structure of pending messages (aka ran event container). Its storage `io_ran_notif_ds` is already declared in 
 * ran_if.h and its initialization will be performed by this API's via notif_init_ran(). 
 * 
 * For the signalling mechanism, I insert the event into the ran event container: when you receive 
 * the specific signal in the asio loop of events of RAN interface, read the `io_ran_notif_ds` to understand 
 * which SM and request generated it and act as a consequence  (i.e. starting or stopping a timer for the indication event)
 * 
 */
static void 
notif_send_ran_event(ran_if_t *ran_if, notif_e2_ran_event_t *ev)
{ 
  push_tsnq(&ran_if->io_ran_notif_ds, ev, sizeof(notif_e2_ran_event_t));

  ran_if->io->send_notif(); 
}

/*
 * Send a notification to E2 loop
 * 
 * For E2 interface, the message will not use any storage from I/O loop as the message is acted upon directly from the calling 
 * function and sent directly to SCTP endpoint. Instead, we will use the datastrcuture `e2_agent_t->pending` of I/O loop to 
 * remember that we sent the message and wait for a reply withing a certain time for those messages that need a reply from RIC. 
 * The list of those messages is specified in pending_event_t data structure
 */
static void 
notif_send_e2_event(e2_agent_t *e2_if, notif_e2_ran_event_t *ev)
{
  pending_event_t pending_event = ev->type;
  long const wait_ms = 3000;
  int fd_timer = create_timer_ms_asio_agent(&e2_if->io, wait_ms, wait_ms); 
  lock_guard(&e2_if->pend_mtx);     
  bi_map_insert(&e2_if->pending, &fd_timer, sizeof(fd_timer), &pending_event, sizeof(pending_event)); 
}

/*
 * XXX: copied from e2_agent.c and changed with a new input to adapt to proxy agent. 
 * Investigate if we can patch the original code removing the static attribute instead of copying it here the content as workaround.
 */
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

/* 
 * Creates the message `E2 setup request` from RAN
 */
#ifdef E2AP_V1
e2_setup_request_t 
generate_setup_request_from_ran(e2_agent_t* ag, global_e2_node_id_t ge2ni)
{
  assert(ag != NULL);

  const size_t all_rf = assoc_size(&ag->plugin.sm_ds);
  assert(all_rf > 0 && "No RAN function/service model registered. \
  Check if the Service Models are located at shared library paths, default location is /usr/local/lib/flexric/");
  
  size_t whitelisted_rf = 0;
  void* it = assoc_front(&ag->plugin.sm_ds);
  for(size_t i = 0; i < all_rf; ++i){
    sm_agent_t* sm = assoc_value( &ag->plugin.sm_ds, it);
    if (is_sm_whitelisted(sm->info.id()))
      whitelisted_rf++;
    it = assoc_next(&ag->plugin.sm_ds, it);
  }
  ran_function_t* ran_func = calloc(whitelisted_rf, sizeof(*ran_func));
  assert(ran_func != NULL);

  e2_setup_request_t sr = {
    .id = ge2ni,
    .ran_func_item = ran_func,
    .len_rf = whitelisted_rf,
    .comp_conf_update = NULL,
    .len_ccu = 0
  };

  it = assoc_front(&ag->plugin.sm_ds);
  size_t whitelisted_idx = 0;
  for(size_t i = 0; i < all_rf; ++i){
    sm_agent_t* sm = assoc_value( &ag->plugin.sm_ds, it);
    assert(sm->info.id() == *(uint16_t*)assoc_key(&ag->plugin.sm_ds, it) && "RAN function mismatch");
    
    if (!is_sm_whitelisted(sm->info.id()))
    {
      it = assoc_next(&ag->plugin.sm_ds, it);
      continue;
    }

    sm_e2_setup_data_t def = sm->proc.on_e2_setup(sm);
    // Pass memory ownership
    ran_func[whitelisted_idx].defn.len = def.len_rfd;
    ran_func[whitelisted_idx].defn.buf = def.ran_fun_def;

    ran_func[whitelisted_idx].id = sm->info.id();
    ran_func[whitelisted_idx].rev = sm->info.rev();

    ran_func[whitelisted_idx].oid = calloc(1, sizeof(byte_array_t));
    assert(ran_func[whitelisted_idx].oid != NULL && "Memory exhausted");
    *ran_func[whitelisted_idx].oid = cp_str_to_ba(sm->info.oid());

    whitelisted_idx++;

    it = assoc_next(&ag->plugin.sm_ds, it);
  }
  assert(it == assoc_end(&ag->plugin.sm_ds) && "Length mismatch");

  return sr;
}
#elif defined(E2AP_V2) || defined (E2AP_V3)
e2_setup_request_t generate_setup_request_from_ran(e2_agent_t* ag, global_e2_node_id_t ge2ni)
{
  assert(ag != NULL);

  const size_t len_rf = assoc_size(&ag->plugin.sm_ds);
  assert(len_rf > 0 && "No RAN function/service model registered. Check if the Service Models are located at shared library paths, default location is /usr/local/flexric/");

  ran_function_t* ran_func = calloc(len_rf, sizeof(*ran_func));
  assert(ran_func != NULL);

  // ToDO: Transaction ID needs to be considered within the pending messages
  e2_setup_request_t sr = {
    .trans_id = ag->trans_id_setup_req++,
    .id = ge2ni,
    .ran_func_item = ran_func,
    .len_rf = len_rf,
  };

  void* it = assoc_front(&ag->plugin.sm_ds);
  for(size_t i = 0; i < len_rf; ++i){
    sm_agent_t* sm = assoc_value(&ag->plugin.sm_ds, it);
    assert(sm->info.id() == *(uint16_t*)assoc_key(&ag->plugin.sm_ds, it) && "RAN function mismatch");

    sm_e2_setup_data_t def = sm->proc.on_e2_setup(sm);
    // Pass memory ownership
    ran_func[i].defn.len = def.len_rfd;
    ran_func[i].defn.buf = def.ran_fun_def;

    ran_func[i].id = sm->info.id();
    ran_func[i].rev = sm->info.rev();
    ran_func[i].oid = cp_str_to_ba(sm->info.oid());
    it = assoc_next(&ag->plugin.sm_ds ,it);
  }
  assert(it == assoc_end(&ag->plugin.sm_ds) && "Length mismatch");

  // E2 Node Component Configuration Addition List
  arr_node_component_config_add_t arr = {0};
  ag->read_setup_ran(&arr);
  // Move ownership
  sr.len_cca = arr.len_cca;
  sr.comp_conf_add = arr.cca;

 return sr;
}
#else
static_assert(0!=0, "Unknown E2AP version");
#endif

// ------------------------E2AP  API ---------------------------------------------------
static void* create_notif_e2_ran_event(void* it)
{
  if(it == NULL)
    return NULL;

  memcpy(&static_notif_e2_ran_event, it, sizeof(notif_e2_ran_event_t));

  return &static_notif_e2_ran_event;
}
static void free_notif_e2_ran_event(void* it)
{
  assert(it != NULL);
  // there is nothing to do
}

notif_e2_ran_event_t * notif_get_ran_event(ran_if_t *ran_if)
{
  return  wait_and_pop_tsnq(&ran_if->io_ran_notif_ds, create_notif_e2_ran_event);
}

void notif_init_ran(ran_if_t *ran_if)
{
  init_tsnq(&ran_if->io_ran_notif_ds, sizeof(notif_e2_ran_event_t));
}

void notif_free_ran(ran_if_t *ran_if)
{
  free_tsnq(&ran_if->io_ran_notif_ds, free_notif_e2_ran_event);
}

void fwd_e2_ran_subscription_timer(ran_if_t *ran_if, ind_event_t ev, long interval_ms)
{

  notif_e2_ran_event_t msg = {
    .type = E2_ADD_SUBSCRIPTION_TIMER_EVENT,
    .subs_ev.time_ms = interval_ms ,
    .subs_ev.sm_id = ev.sm->info.id(),
    .subs_ev.ric_action_id = ev.action_id,
    .subs_ev.ric_id = ev.ric_id,
    .subs_ev.act_def = ev.act_def
  };
  
  notif_send_ran_event(ran_if, &msg);
}

void fwd_e2_ran_wr_sub_ev(ran_if_t *ran_if, uint32_t ric_req_id)
{

  notif_e2_ran_event_t msg = {
      .type = E2_WRITE_SUBSCRIPTION_EVENT,
      .wr_subs_ev.ric_req_id = ric_req_id,
  };

  notif_send_ran_event(ran_if, &msg);
}

void fwd_e2_ran_remove_subscription_timer(ran_if_t *ran_if, ric_gen_id_t ric_id) 
{
  printf("Removing subscription request for SM %d\n", ric_id.ran_func_id);
  
  notif_e2_ran_event_t msg = {
    .type = E2_REMOVE_SUBSCRIPTION_TIMER_EVENT,
    .unsubs_ev.sm_id = ric_id.ran_func_id,
  };

  notif_send_ran_event(ran_if, &msg);
}

void fwd_ran_e2_setup_request(e2_agent_t *e2_if, global_e2_node_id_t ge2ni) 
{
  lwsl_info("Sending E2 Setup request from ran id %d. timeout of 3 seconds.\n", ge2ni.nb_id.nb_id);
  e2_if->global_e2_node_id = ge2ni;
  e2_setup_request_t sr = generate_setup_request_from_ran(e2_if, ge2ni); 
  defer({ e2ap_free_setup_request(&sr);  } );
  byte_array_t ba = e2ap_enc_setup_request_ag(&e2_if->ap, &sr); 
  defer({free_byte_array(ba); } ); 
  e2ap_send_bytes_agent(&e2_if->ep, ba);

  notif_e2_ran_event_t ev;
  ev.type = RAN_SETUP_REQUEST_PENDING_EVENT;
  notif_send_e2_event(e2_if, &ev);
}

void fwd_ran_e2_ric_subscription_response(e2_agent_t *e2_if, e2_subscribe_event_t reply)
{
  lwsl_info("[E2 -> RIC]: sending RIC_SUBSCRIPTION_RESPONSE\n");

  ind_event_t ev = { .action_id = reply.ric_action_id,
                     .ric_id = reply.ric_id,
                     .sm = sm_plugin_ag(&e2_if->plugin, reply.sm_id),
                     .act_def = reply.act_def};
  int fd_timer = create_timer_ms_asio_agent(&e2_if->io, reply.time_ms, reply.time_ms); 
  assert(pthread_mutex_lock(&e2_if->mtx_ind_event) == 0);  
  bi_map_insert(&e2_if->ind_event, &fd_timer, sizeof(fd_timer), &ev, sizeof(ev));
  assert(pthread_mutex_unlock(&e2_if->mtx_ind_event) == 0);  
  ric_subscription_response_t sr = generate_subscription_response(&ev.ric_id, ev.action_id); 
  defer({ e2ap_free_subscription_response(&sr);  } );
  byte_array_t ba = e2ap_enc_subscription_response_ag(&e2_if->ap, &sr); 
  defer({free_byte_array(ba); } ); 
  e2ap_send_bytes_agent(&e2_if->ep, ba);
  /* 
   * Note that we do not need to create a message in notif queue like in fwd_ran_e2_setup_request()
   * as this message does not need ack. Instead it goes directly to SCTP endpoint.
   */
}

// XXX: copied from agent/msg_handler_agent.c
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

void fwd_ran_e2_ctrl_reply (e2_agent_t *e2_if, ctrl_ev_reply_t reply)
{
  lwsl_info("[E2 -> RIC]: sending RIC_CONTROL_RESPONSE\n");
  pending_event_t ev = CONTROL_REQUEST_AGENT_PENDING_EVENT;
  lock_guard(&e2_if->pend_mtx);
  int* fd = bi_map_extract_right(&e2_if->pending, &ev, sizeof(ev), NULL);
  assert(*fd > 0);
  int fdcorr = *fd;
  rm_fd_asio_agent(&e2_if->io, *fd);
  free(fd);
  
  correlation_event_t *corr;
  lock_guard(&e2_if->corr_mtx);
  corr = bi_map_extract_left(&e2_if->correlation, &fdcorr, sizeof(int), NULL);
  assert(corr != NULL);
  defer({free(corr); } ); 

  // XXX: handle the 3 cases for status to be reported to ric:  RIC_CONTROL_STATUS_SUCCESS,  RIC_CONTROL_STATUS_REJECTED,  RIC_CONTROL_STATUS_FAILED
  // for now we handle just one SUCCESS/FAILURE
  assert(reply.ans.type == CTRL_OUTCOME_SM_AG_IF_ANS_V0);
#ifdef E2AP_V1
  ric_control_status_t ret_sts;
  if (reply.ans.ctrl_out.type == MAC_AGENT_IF_CTRL_ANS_V0)
    ret_sts = (reply.ans.ctrl_out.mac.ans == MAC_CTRL_OUT_OK) ? RIC_CONTROL_STATUS_SUCCESS: RIC_CONTROL_STATUS_FAILED;
  else
    assert (0!=0 && "unsupported service model for CTRL procedure\n");
#endif
  sm_ctrl_out_data_t ctrl_ans = {.len_out = 0, .ctrl_out =NULL};
  
  byte_array_t* ba_ctrl_ans = ba_from_ctrl_out(&ctrl_ans);
  ric_control_acknowledge_t ctrl_ack = {    
    .ric_id = corr->ric_id,
    .call_process_id = NULL,
#ifdef E2AP_V1
    .status = ret_sts,
#endif
    .control_outcome = ba_ctrl_ans 
    } ;
  byte_array_t ba = e2ap_enc_control_acknowledge_ag(&e2_if->ap, &ctrl_ack); 

  // send the reply via SCTP protocol
  defer({free_byte_array(ba); } ); 
  e2ap_send_bytes_agent(&e2_if->ep, ba); 
  /* 
   * Note that we do not need to create a message in notif queue like in fwd_ran_e2_setup_request()
   * as this message does not need ack. Instead it goes directly to SCTP endpoint.
   */
}

void fwd_e2_ran_ctrl (ran_if_t *ran_if, ctrl_ev_t in)
{
  notif_e2_ran_event_t msg = {
    .type = E2_CTRL_EVENT,
    .ctrl_ev = in
  };
  
  notif_send_ran_event(ran_if, &msg);
}

int cmp_notif_event(void const* pend_v1, void const* pend_v2)
{
  assert(pend_v1 != NULL);
  assert(pend_v2 != NULL);

  notif_event_t* ev1 = (notif_event_t*)pend_v1; 
  notif_event_t* ev2 = (notif_event_t*)pend_v2; 

  if(*ev1 < *ev2) return 1;
  if(*ev1 == *ev2) return 0;
  return -1;
}

char * notif_strevent(enum notif_event_t type) 
{
  switch (type) 
  {
    case RAN_SETUP_REQUEST_PENDING_EVENT:
      return "RAN setup";
    case E2_ADD_SUBSCRIPTION_TIMER_EVENT:
      return "E2_ADD_SUBSCRIPTION_TIMER_EVENT";
    case E2_REMOVE_SUBSCRIPTION_TIMER_EVENT:
      return "E2_REMOVE_SUBSCRIPTION_TIMER_EVENT";
    case E2_CTRL_EVENT:
      return "E2_CTRL_EVENT";
    default:
      return "bug"; 
  }
}

/* ------------------------------------------------------------------------------------------------------ */
static void ran_handle_notif_ctrl(ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id)
{
    (void)e2_if;

    const char *p = ran_if->ser->encode_ctrl(msg_id, notif_event->ctrl_ev.req);
    ws_ioloop_event_t ev = {
      .msg_type       = E2_CTRL_EVENT, 
      .ctrl_ev.ric_inst_id = notif_event->ctrl_ev.ric_id.ric_inst_id,
      .ctrl_ev.ran_func_id = notif_event->ctrl_ev.ric_id.ran_func_id,
      .ctrl_ev.ric_req_id  = notif_event->ctrl_ev.ric_id.ric_req_id
    };
    ran_if->io->write_to_ran(&ev, p, strlen(p) + 1);
}

static void ran_handle_notif_write_sub(ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id)
{
  (void)e2_if;

  const char *p = ran_if->ser->encode_indication(msg_id, SM_RC_ID, (double)get_proxy_agent()->conf.io_ran_conf.timer/1000);
  ws_ioloop_event_t ev = {
      .msg_type               = E2_WRITE_SUBSCRIPTION_EVENT,
      .wr_subs_ev.ric_req_id  = notif_event->wr_subs_ev.ric_req_id,
  };
  ran_if->io->write_to_ran(&ev, p, strlen(p) + 1);
}

static void ran_handle_notif_subsrmtimer (ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id)
{
  (void)ran_if;
  (void)e2_if;
  (void)msg_id;

  ran_io_rmtimer(notif_event->unsubs_ev.sm_id);
  /* 
   * no need to send a reply back to the E2 interface as we went for a 'fire and forget' approach in calling routine
   * e2ap_handle_subscription_delete_request_agent()
   */
}

static void ran_handle_notif_subsaddtimer (ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id)
{
  (void)msg_id;
  if (notif_event->subs_ev.time_ms > 0){
    ran_io_addtimer(notif_event->subs_ev.sm_id, notif_event->subs_ev.time_ms);
    ind_event_t *ev = calloc(1, sizeof(ind_event_t));
    ev->sm = sm_plugin_ag(&e2_if->plugin, notif_event->subs_ev.sm_id);
    ev->action_id = notif_event->subs_ev.ric_action_id;
    ev->ric_id = notif_event->subs_ev.ric_id;
    ev->act_def = notif_event->subs_ev.act_def;
    bi_map_insert(&ran_if->ind_event, &notif_event->subs_ev.sm_id, sizeof(notif_event->subs_ev.sm_id), ev, sizeof(ind_event_t));
  }
  fwd_ran_e2_ric_subscription_response(e2_if, notif_event->subs_ev);
}

// Note that the remaining seats in array 'handle_notif' are initialized automatically to NULL(0)
typedef void (*handle_msg_notif)(ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id);
static handle_msg_notif handle_notif[5] = {
    [E2_ADD_SUBSCRIPTION_TIMER_EVENT]      = &ran_handle_notif_subsaddtimer,
    [E2_REMOVE_SUBSCRIPTION_TIMER_EVENT]   = &ran_handle_notif_subsrmtimer,
    [E2_CTRL_EVENT]                        = &ran_handle_notif_ctrl,
    [E2_WRITE_SUBSCRIPTION_EVENT]          = &ran_handle_notif_write_sub,
};


void ran_notif_msg_handle(ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id) 
{
  assert(handle_notif[notif_event->type] != NULL);
  return handle_notif[notif_event->type](ran_if, e2_if, notif_event, msg_id); 
}