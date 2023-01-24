/* cf: timeout docs from lws: https://libwebsockets.org/lws-api-doc-main/html/lws-timeout-timer_8h_source.html*/
#define PROXY_AGENT
#include "../agent/e2_agent_api.h"
#include "ringbuffer.h"
#include "notif_e2_ws.h"
#include "ran_if.h"
#include <libwebsockets.h>

static pthread_t thrd_agent;

/* Two approaches are envisioned for forwarding the E2 subscription timer to the WS interface:
 * 1. create the timer in the ASIO module of proxy_agent RAN interface,
 * 2. send a message notification to the WS interface (i.e. pushing a message with ID=E2_SUBSCRIPTION_REQ that 
 *    will contain the info about the timer and then process it in the ws loop to the pending queue),
 * We choose the first approach in this implementation.
 */
void fwd_e2_ws_subscription_timer(ran_if_t *ran_if, ind_event_t ev, long initial_ms, long interval_ms) 
{
  (void) initial_ms;
  
  /* 
   * Create the timer on WS interface and pass its identifying values to the WS event loop datastructure 'user'.
   * Setting of the timer and the user data associated should be atomic. I believe we have no multithreading issue here, 
   * so for the moment we skip using a mutex like in lock_guard(&ric->pend_mtx);
   * The timer provided by LWS api, can be delayed/overwritten by another timer that can arrive in the meanwhile. This is a side effect
   * that you need to manage this in such a way that you can handle multiple timers at the same time (i.e. for various SMs requests).
   */
  int timer_id = ev.sm->ran_func_id;

  lwsl_user("[WS]: set indication timer (id=%d, %ld ms)\n", timer_id, interval_ms);
  ran_if->user->timer_ms = interval_ms; 
  ran_if->user->timer_id = timer_id; 
  lws_set_timer_usecs(ran_if->lws, interval_ms * 1000); 

  /* 
   * Insert the event into the ran event container. When you receive the alarm signal in the main loop of events of WS interface, 
   * read the ind_event to understand which SM and request generated it.
   */
  bi_map_insert(&ran_if->ind_event, &timer_id, sizeof(timer_id), &ev, sizeof(ind_event_t));
}

/* 
 * This is a basic implementation that removes the timer for all the SMs at WS interface
 * XXX: handle timers per SMs at WS interface. For the moment there is just one timer.
 * Quoted from the E2AP specification
 * " At reception of the RIC SUBSCRIPTION DELETE REQUEST message the target E2 Node shall:
 *  - Determine the target function using the information in the RAN Function ID IE and delete the corresponding RIC
 *    EVENT trigger using information in the RIC Request ID IE.
 *  - If one or more subsequent actions were included in the previously received RIC Subscription, then the target
 *  function shall delete the required actions along with the corresponding RIC Request ID IE.
 */
void fwd_e2_ws_remove_subscription_timer(ran_if_t *ran_if, ric_gen_id_t ric_id) 
{
  lwsl_info("Removing subscription request for SM %d\n", ric_id.ran_func_id);
  int timer_id = ric_id.ran_func_id;
  bi_map_extract_left(&ran_if->ind_event, &timer_id, sizeof(timer_id));

  lws_set_timer_usecs(ran_if->lws, LWS_SET_TIMER_USEC_CANCEL);

  ran_if->ind_timer_ready = false;
}

static inline void* static_start_agent(void* a)
{
  e2_start_agent(a);
  return NULL;
}

void fwd_ws_e2_setup_request(proxy_agent_t *p, global_e2_node_id_t ge2ni) 
{
  sm_io_ag_t io = {.read = e2_read_RAN, .write = e2_write_RAN};
  const int e2ap_server_port = 36421;
  char* server_ip_str = get_near_ric_ip(&p->conf.e2args);

  p->e2_if = e2_init_agent(server_ip_str, e2ap_server_port, ge2ni, io, &p->conf.e2args);
  p->e2_if->ran_if = &p->ran_if;// this is a trick to avoid modifing the e2_init_agent() signature

  free(server_ip_str);

  const int rc = pthread_create(&thrd_agent, NULL, static_start_agent, p->e2_if);
  assert(rc == 0);
}

pthread_t get_e2_agent_thread_id(void){return thrd_agent;}
