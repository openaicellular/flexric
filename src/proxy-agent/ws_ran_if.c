/* 
 * websocket implementation of the RAN interface
 */
#include <json-c/json.h>
#include <assert.h>

#include "util/alg_ds/alg/defer.h"
#include "util/alg_ds/alg/alg.h"
#include "util/compare.h"


#include "ws_ran_if.h"
#include "msg_json_ws.h"
#include "e2_sm_agent.h"
#include "ws_msg_hdlr.h"
#include "proxy_agent.h"
#include "ringbuffer.h"


/* Forward static declarations =======================================================================*/

static void start_connecting_client(lws_sorted_usec_list_t *sul);
static int loop_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);
static int ws_send_msg(struct lws *wsi, const char *jsonmsg);
static char * ws_strerror(enum lws_callback_reasons reason);
/* Globals and defines ===============================================================================*/

// temporary solution via global variable until we implement the bimap_t 'pending' in proxy_agent that will store the 
// pending messages to be sent to the RAN
static bool e2_setup_done = false;

/* 
 * List of supported protocols (we have just one) and parameters related to those protocols.
 * We load the list at init time (ws_init) but we bind the specific protocol at connection time (ws_conn). 
 */
static const struct lws_protocols protocols[] = {

    {"amarisoft-ws-client", loop_callback, sizeof(struct ws_async_event_t), 0, 0, &static_user, 0},
    {NULL, NULL, 0, 0, 0, NULL, 0}
};

// XXX: unused global var need by ws_conn
lws_sorted_usec_list_t global_sul; 

/* static functions ====================================================================================*/

/*
 * If you have something to send, you call this function and when the socket becomes writeable, 
 * you will get a `LWS_CALLBACK_SERVER_WRITEABLE' event managed by 
 * loop_callback() where you should generate the data to send and then send it with `lws_write()`.
 * Note that you cannot send data using `lws_write()` outside of the LWS_CALLBACK_SERVER_WRITEABLE event.
 */
static void ws_set_sendmode(void) 
{
  (void)lws_callback_on_writable_all_protocol(ws_get_global_ctx(), &protocols[0]);
}
/*
 * low level routine that send the message to websocket destination
 * RETURN: 0 (success), != 0 (failure)
 */
static int ws_send_msg(struct lws *wsi, const char *jsonmsg)
{
  int ret = 0;
  lwsl_info("[WS -> RAN] Sending %s\n", jsonmsg);
  int msg_len = strlen(jsonmsg) + 1;
  /*
   * caveats: the pointer containing our data must have LWS_PRE valid allocated preamble memory behind it. 
   * You can leave that memory empty, lws_write will use it to handle context
   */
  char buf[LWS_PRE + 150];
  
  assert (msg_len < 150 && "Hard limit on msg length buffer hit. Programming error\n");
  lws_strncpy(&buf[LWS_PRE], jsonmsg, msg_len);
  if (lws_write(wsi, (unsigned char *)&buf[LWS_PRE], msg_len, LWS_WRITE_TEXT) == -1){
    lwsl_err("Failure sending msg %s\n", jsonmsg);
    ret = -1;
  }
  return ret;
}

/*
 * try to reach the status 'connection successfully created'. This does not mean that the websocket service is 
 * up and runnning on target host (i.e route to host). You will get that info instead from the loop_callback() 
 * function when receiving LWS_CALLBACK_CLIENT_ESTABLISHED event or LWS_CALLBACK_CLIENT_CONNECTION_ERROR meaning 
 * that the remote ws service is not up.
 */
static void start_connecting_client(lws_sorted_usec_list_t *userlist)
{
  struct lws_client_connect_info i;
  memset(&i, 0, sizeof(i));
  
  i.context         = ws_get_global_ctx(); // TOFIX: this is a workaround
  i.address         = get_proxy_conf()->address;// TOFIX: this is a workaround
  i.port            = get_proxy_conf()->port;// TOFIX: this is a workaround
  i.path            = "/";
  i.host            = i.address;
  // origin header: Amarisoft accepts any name you put, that is 'flexric' could also be a good name, but you need to put something.
  i.origin          = i.address; 
  i.protocol        = protocols[0].name;
  i.ssl_connection  = 0; // use only unsecured websockets (`ws`) that do not use SSL
  
  // TOFIX: probably to be removed below i.pwsi as it is not valued and we have just one websocket instance.
  (void)userlist;// unused for the moment. Main purpose it to schedule a retry for the connection
  // struct proxy_conf_t  *conf = lws_container_of(userlist, struct proxy_conf_t, userlist);
  // i.pwsi            = &conf->wsi;  

  lwsl_info("Starts connection to %s\n", i.address);
  if (!lws_client_connect_via_info(&i))
  {
    lwsl_err("Unable to connect\n");
    proxy_set_exit();
  }

  return;
}

// TEMPORARY COPY OF CODE from agent/e2_agent.c: <BEGIN>
// to substitute for bi_map_extract_left(&p_agent->ran_if.ind_event, &shared_msg->timer_id, sizeof(shared_msg->timer_id));
// that does not work
static inline void* ind_fd(bi_map_t *in, int fd)
{
  assert(fd > 0);

  void* start_it = assoc_front(&in->left);
  void* end_it = assoc_end(&in->left);

  void* it = find_if(&in->left, start_it, end_it, &fd, eq_fd );
  return it;
}

// 
static inline
bool ind_event_search(bi_map_t *in, int fd, ind_event_t** i_ev)
{
  assert(*i_ev == NULL);
  void* it = ind_fd(in, fd);   
  void* end_it = assoc_end(&in->left); // bi_map_end_left(&ag->ind_event);
  if(it != end_it){
    *i_ev = assoc_value(&in->left, it);
    return true;
  } 
  return false;
}

// TEMPORARY COPY OF CODE: </END>

/* 
 * main callback mechanism for sending/receiving data and events from the RAN
 */
static int loop_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
  proxy_agent_t *p_agent = get_proxy_agent();
  struct ws_async_event_t *shared_msg = p_agent->ran_if.user; 
  
  ws_set_global_lws(wsi);
  
  switch (reason)
  {
  case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
    lwsl_err("Unable to connect to RAN host at %s:%d. Reason: %s\n", p_agent->conf.address, p_agent->conf.port, in ? (char *)in : "(null)");
    lwsl_err("Check that the amarisoft websocket process is up and running at that location.\n");
    ws_conf_print(&p_agent->conf);
    ws_initconf_print_help();
    goto do_retry;
    break;

  case LWS_CALLBACK_CLIENT_RECEIVE:
    
    assert (user != NULL && "should not happen. Loop internal datastructure is null\n");

    lwsl_info("[RAN - WS] Getting data from the RAN: %s\n", in ? (char *)in : "(null)");
    ws_msg_t ws_msg = {.buf = in, .len = len};
    e2_agent_msg_t ans = ws_msg_handle(p_agent, &ws_msg);
    
    if (ans.type_id == WS_E2_CONFIG_FWD) { 
      e2_setup_done = true; 
    } else if (ans.type_id == WS_E2_SETUP) { 
      if (e2_setup_done == false) 
      {
        lwsl_user("Asking to the RAN the content data for an E2 setup\n");
        //XXX:  insert this message in a map queue (key - value store) and get the msg_id related to the key. For the moment we set this to a 
        //      fixed value (2)
        int msg_id = 2;
        const char *p = ws_json_encode_e2setup(msg_id);
        (void)strncpy(shared_msg->jsonmsg, p, sizeof(shared_msg->jsonmsg));
        shared_msg->req_id = msg_id;
        ws_set_sendmode();
      } else {
        lwsl_warn("XXX: Asking again for a E2 setup is not implemented. Request discarded\n");
      }
    } else if (ans.type_id == WS_E2_NONE) { 
      lwsl_info("No further action to be relayed to E2 interface\n"); 
    } else { 
      assert(0!=0 && "Unhandled message from WS interface\n"); 
    }
    break;

  case LWS_CALLBACK_CLIENT_WRITEABLE:
    assert (user != NULL && "should not happen. Loop internal datastructure is null\n");

    lwsl_info("WS: ready to send data to RAN coming from message id %d\n", shared_msg->req_id);
    if (ws_send_msg(wsi, shared_msg->jsonmsg))
        lwsl_err("Failure sending data to Amarisoft\n");
    
    break;

  case LWS_CALLBACK_CLIENT_ESTABLISHED:
    lwsl_user("WS: Connection established\n");
    break;

  case LWS_CALLBACK_TIMER:
    // XXX-INVESTIGATE: for some reasons, the first time I set the timer in fwd_e2_ws_subscription_timer() it takes almost one second to arrive here
    // To circumvent, I put a flag in the global directory when for example you are ready to read indication from E2 interface.
    assert (user != NULL && "should not happen. Loop internal datastructure is null\n");
    ind_event_t *ev = NULL;
    if (ind_event_search(&p_agent->ran_if.ind_event, shared_msg->timer_id, &ev) == false)
      assert ( 0!=0 && "missing indication event from the bi_map: should not happen\n");
    int msg_id = 1;
    const char *p = ws_json_encode_indication(msg_id, ev->sm->ran_func_id);
    (void)strncpy(shared_msg->jsonmsg, p, sizeof(shared_msg->jsonmsg));
    shared_msg->req_id = msg_id;
    
    ws_set_sendmode();
    
    // re-set timer for the same event
    lwsl_info("[WS]: reset indication timer (id=%d, %ld ms)\n", shared_msg->timer_id, shared_msg->timer_ms);
    lws_set_timer_usecs(wsi, shared_msg->timer_ms * 1000);
    // no need to insert again as we did not remove the event
    // bi_map_insert(&p_agent->ran_if.ind_event, &shared_msg->timer_id, sizeof(shared_msg->timer_id), ev, sizeof(ind_event_t));

    break;
  case LWS_CALLBACK_CLIENT_CLOSED:
    goto do_retry;
  case LWS_CALLBACK_PROTOCOL_DESTROY:
    lwsl_user("Received request from RAN to close all: %s\n\n", ws_strerror(reason));
    proxy_set_exit();
    break;
  default:
    lwsl_user("**** WARN: callback event %d (%s) is not handled. IGNORING *****\n", reason, ws_strerror(reason));
    break;
  }

  return 0;

do_retry:
  /*
   * TODO: retry the connection to keep it nailed up
   *
   * We try to conceal any problem for one set of backoff retries and then exit the app. 
   * If you set retry.conceal_count to be LWS_RETRY_CONCEAL_ALWAYS,
   * it will never give up and keep retrying at the last backoff
   * delay plus the random jitter amount.
   */
  lwsl_user("retry connection to ws server: to be implemented\n");
  proxy_set_exit();

  return 0;
}

/* 
 * print a meaningful message for the WS interface message id returned to the user
 */
static char * ws_strerror(enum lws_callback_reasons reason) 
{
  // XXX: handle more cases from the enumeration 'reason'
  switch (reason) 
  {
    case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED: 
      return "A new client has been accepted by the ws server";
    case LWS_CALLBACK_WSI_CREATE:
	    return "LWS WSI: create callback";
    case LWS_CALLBACK_CLIENT_HTTP_BIND_PROTOCOL:
      return "LWS HTTP PROTOCOL: bind status callback";
    case LWS_CALLBACK_CLIENT_HTTP_DROP_PROTOCOL:
      return "LWS HTTP PROTOCOL: drop status callback";
    default:
      return "unkn"; 
  }
}
/* interface implementation ==================================================================*/

struct lws_context *ws_init(struct proxy_conf_t conf)
{
  lws_set_log_level(conf.logl, NULL);
  lwsl_user("WS interface: performing initialization\n");

  struct lws_context_creation_info info;
  memset(&info, 0, sizeof info);
  info.port = CONTEXT_PORT_NO_LISTEN; /* just client, no server */
  info.protocols = protocols;
  info.fd_limit_per_thread = 1 + 1 + 1;

  return lws_create_context(&info);
};

void ws_conn(struct lws_context *ctx)
{
  lws_sul_schedule(ctx, 0, &global_sul, start_connecting_client, 1);
}

/*
 * Event driven loop that services any websocket activity.
 *
 * RETURN: 0 (success), != 0 (failure)
 */
int ws_poll(struct lws_context *ctx)
{
  const int ignoretimeout = 0;
  return lws_service(ctx, ignoretimeout);
}

void ws_free(struct lws_context *ctx)
{ 
  lws_context_destroy(ctx);
  // XXX: free_indication_event(&proxy_agent->ran_if.ind_event);
}