#include <assert.h>

#include <libwebsockets.h>

#include "util/alg_ds/alg/defer.h"
#include "util/alg_ds/ds/lock_guard/lock_guard.h"

#include "e2_sm_agent.h"
#include "io_ran.h"
#include "ran_if.h"
#include "notif_e2_ran.h"
#include "proxy_agent.h"

io_ran_abs_t ws_ran_io; // forward declaration

ws_ioloop_t static_user; // XXX: probably you can bring inside the implementation instead of exposing it

/* XXX: 'global_sul' 
 * global variable storage needed at connection time by 'lws_sul_schedule', but not used for now.
 * To investigate if you can put null in the calling function and get rid of this variable
 */
lws_sorted_usec_list_t global_sul; 

bool ran_open_success = false;

static io_ran_abs_t *io_ran_instance = NULL;
#define LIMITS_TIMER_SM_MAX_NUM 10 // max number of indication timers supported. One timer is associated to one Service Model

struct io_ran_priv_data_t {
  /* 
   * XXX: context and instance. 
   * Remember you can use `lws_get_context(const struct lws *wsi)` to get the lws_context from wsi
   * For some (obscure) reasons this association does not work. A workaround has been implemented in `io_ran_ws_async_loop()`
   * until I find the root cause for the error.
   */
  struct lws *ws_instance; // needs to be multithreading for the notification functionality that is used by E2 thread
  pthread_mutex_t wsi_mtx;
  struct lws_context *ctx; 
  int indication_polling_ms;  // fixed value for polling frequency. Initialized when you instanciate the module RAN I/O.
  next_msg_t (*read_cb)(const char *buf, size_t len, bi_map_t *sent_msg);   
  proxy_agent_t *pa;    // back reference to proxy_agent static variable
  
  ws_ioloop_t *user;    // pointer to io_loop storage
  int io_timers[LIMITS_TIMER_SM_MAX_NUM]; 
  atomic_bool timer_set;       // internal flag to signal if there is already a timer set for I/O RAN
  atomic_bool e2_setup_done;   // state machine of processed messages (e2 setup)
  atomic_bool ind_timer_ready; // state machine of processed messages (indication)
};

static int static_msg_id = 0; // lookup key for io_loop event map

struct io_ran_conf_t static_ran_conf;

// forward declaration as perused by p[]
static int io_ran_ws_async_loop(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

static const struct lws_protocols p[] = {     
  {
         "ws-client",            // 1. protocol_name
         &io_ran_ws_async_loop,   // 2. callback for servicing the async loop
         sizeof(struct ws_ioloop_event_t),// 3. per-connection data size (sizeof(param 6))
         0,                      // 4. rx_buffer_size (opt)
         0,                      // 5. id  (opt)
         &static_user,           // 6. pointer to an external data storage having size specified by parameter number 3 that we want  
                                 //    to be accessible from the callback. This external storage will be used as internal notification mechanism
                                 //    to be read by the function that loops on the I/O available. 
         0                       // 7. tx_packet_size (opt)
       },
       {NULL, NULL, 0, 0, 0, NULL, 0}
  };

/* ------------------------------------------------------------------------------------------------------- */
static int cmp_event (void const *key1_abs, void const *key2_abs)
{
  assert(key1_abs != NULL);
  assert(key2_abs != NULL);
  ws_ioloop_event_t* key1 = (ws_ioloop_event_t*)key1_abs;
  ws_ioloop_event_t* key2 = (ws_ioloop_event_t*)key2_abs;
  if (key1->msg_type != key2->msg_type)
    return -1;
    
  switch (key1->msg_type ) 
  {
    case RAN_SETUP_REQUEST_PENDING_EVENT:
      return memcmp (&key1->setup_ev, &key2->setup_ev, sizeof (key2->setup_ev));
    case E2_ADD_SUBSCRIPTION_TIMER_EVENT:
      return memcmp (&key1->subs_ev, &key2->subs_ev, sizeof (key2->subs_ev));
    case E2_REMOVE_SUBSCRIPTION_TIMER_EVENT:
      return memcmp (&key1->unsubs_ev, &key2->unsubs_ev, sizeof (key2->unsubs_ev));
    case E2_CTRL_EVENT:
      return memcmp (&key1->ctrl_ev, &key2->ctrl_ev, sizeof (key2->ctrl_ev));
    default:
      assert (0!=0 && "Unexpected event type\n");
  }
  return -1;
}

static int cmp_int(void const *key1_abs, void const *key2_abs)
{
  assert(key1_abs != NULL);
  assert(key2_abs != NULL);
  
  int *key1 = (int *)key1_abs;
  int *key2 = (int *)key2_abs;

  if(*key1 < *key2) return 1;
  if(*key1 == *key2) return 0;
  return -1;
}

static void free_int (void *key, void *val) 
{
  
  assert(key != NULL);
  assert(val != NULL);

  (void)key;

  int* fd = (int* )val;
  free(fd);
}

static void free_io_event(void *key, void *val)
{
  assert(key != NULL);
  assert(val != NULL);

  (void)key;

  struct io_loop_event_t* ev = (struct io_loop_event_t* )val;
  free(ev);
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
    case LWS_CALLBACK_WSI_DESTROY:
	    return "LWS_CALLBACK_WSI_DESTROY: destroy notification to websocket instance";
    default:
      return "unkn"; 
  }
}

static ran_errcodes_t io_ran_ws_write(const uint8_t *jsonbuf, size_t len)
{
  int ret = ERR_RAN_IO_NONE;

  if (!io_ran_instance || !io_ran_instance->priv_data->ws_instance) {
    lwsl_err("Init of instance of RAN not correctly performed\n");
    return ERR_RAN_INIT_FAILURE;
  }
  int msg_len = len + 1;
  /*
   * caveats: the pointer containing our data must have LWS_PRE valid allocated preamble memory behind it. 
   * You can leave that memory empty, lws_write will use it to handle context
   */
  char buf[LWS_PRE + 200];
  assert (msg_len < 200 && "Hard limit on msg length buffer hit. Programming error. You should enlarge buffer in io_ran_ws_write()\n");

  lws_strncpy(&buf[LWS_PRE], (const char *)jsonbuf, msg_len);
  lwsl_debug("[E2 -> RAN] Sending on wire '%s'\n", &buf[LWS_PRE]);
  if (lws_write(io_ran_instance->priv_data->ws_instance, (unsigned char *)&buf[LWS_PRE], msg_len, LWS_WRITE_TEXT) == -1){
    lwsl_err("Failure sending msg %s\n",(unsigned char *)&buf[LWS_PRE]);
    ret = ERR_RAN_SEND_DATA;
  }
  return ret;
}

static bool ran_io_anytimer(void)
{
  for (size_t i = 0; i< sizeof(io_ran_instance->priv_data->io_timers)/sizeof(int); i++)
    if (io_ran_instance->priv_data->io_timers[i] != 0)
      return true;
    
  return false;
}

static void ran_io_inittimers (void)
{
  memset (&io_ran_instance->priv_data->io_timers, 0, sizeof(io_ran_instance->priv_data->io_timers));
  io_ran_instance->priv_data->timer_set = false;
}

/*
 * We enter into this case when I/O ran timer is expired and needs to be set again. 
 * This is the case of entering into callback timer in I/O loop
 */
static void ran_io_expiredtimer(int sm_id)
{
  for (size_t i = 0; i< sizeof(io_ran_instance->priv_data->io_timers)/sizeof(int); i++)
  {
    if (io_ran_instance->priv_data->io_timers[i] == sm_id)
      break;
    if (io_ran_instance->priv_data->io_timers[i] == 0){
      io_ran_instance->priv_data->io_timers[i] = sm_id;
      break;
    }
  }
  // lock_guard(&io_ran_instance->priv_data->wsi_mtx);
  lws_set_timer_usecs(io_ran_instance->priv_data->ws_instance, io_ran_instance->priv_data->indication_polling_ms * 1000);
  io_ran_instance->priv_data->timer_set = true;
}

static ran_errcodes_t io_ran_ws_send_notif()
{
  assert(pthread_mutex_lock(&io_ran_instance->priv_data->wsi_mtx) == 0);
  
  struct lws_vhost            *host = lws_get_vhost(io_ran_instance->priv_data->ws_instance);
  const struct lws_protocols  *p_p  = &p[0];
  const int timeout_us              = 100;
  int ret = lws_timed_callback_vh_protocol_us(host, 
                                              p_p, 
                                              LWS_CALLBACK_USER, 
                                              timeout_us
                                              );
  assert(pthread_mutex_unlock(&io_ran_instance->priv_data->wsi_mtx) == 0);
  
  return (ret == 0) ? ERR_RAN_IO_NONE : ERR_RAN_GEN_FAILURE;
}

/*
 * This function will try to reach the status 'connection successfully created'. This does not mean that the websocket service is 
 * up and runnning on target host (i.e route to host). Instead you will get that info from the loop callback
 * io_ran_ws_async_loop() function when receiving wakeup with reason LWS_CALLBACK_CLIENT_ESTABLISHED or 
 * LWS_CALLBACK_CLIENT_CONNECTION_ERROR
 */
static void start_connecting_client(lws_sorted_usec_list_t *userlist)
{
  struct lws_client_connect_info i;
  memset(&i, 0, sizeof(i));
  
  i.context         = io_ran_instance->priv_data->ctx; 
  i.address         = static_ran_conf.address;
  i.port            = static_ran_conf.port;
  i.path            = "/";
  i.host            = i.address;
  /* Caveats origin header. 
   * Amarisoft accepts any name you put as origin, i.e 'flexric' could also be a good name, but you need to 
   * put something. Decided to use instead address.
   */
  i.origin          = i.address; 
  i.protocol        = p[0].name;
  // ssl_connection = 0 means we use only unsecured websockets (`ws`) that do not use SSL
  i.ssl_connection  = 0; 
  
  // XXX: userlist is unused for the moment. Its main purpose it to schedule a retry for the connection
  (void)userlist;
  
  lwsl_info("Starts connection to %s\n", i.address);
  io_ran_instance->priv_data->ws_instance = lws_client_connect_via_info(&i);
  if (!io_ran_instance->priv_data->ws_instance)
    lwsl_err("Unable to connect\n");
  else 
    ran_open_success = true; // use this global variable to bypass issue on start_connecting_client() not having a return value

  return;
}

static ran_errcodes_t io_ran_ws_open(int ms)
{
  assert (io_ran_instance != NULL &&  "programming error. IO RAN instance is null\n");
  const int ignoretimeout = 0;
  
  assert(pthread_mutex_lock(&io_ran_instance->priv_data->wsi_mtx) == 0);
  
  if (ran_open_success == false) {
    io_ran_instance->priv_data->indication_polling_ms = ms;
    lws_sul_schedule(io_ran_instance->priv_data->ctx, 
                    0, 
                    &global_sul, 
                    start_connecting_client, 
                    1);
    // try to serve it immediately what has been set by lws_sul_schedule()
    (void)lws_service(io_ran_instance->priv_data->ctx, ignoretimeout);
  } else {
    
    if (lws_service(io_ran_instance->priv_data->ctx, ignoretimeout))
      return ERR_RAN_GEN_FAILURE;
    
  }
  assert (pthread_mutex_unlock(&io_ran_instance->priv_data->wsi_mtx) == 0);
  /* 
   * WARN: workaround to avoid signal race issue from futex (ref. CCC). 
   */
  usleep(1000);
   
  return (ran_open_success == false) ? ERR_RAN_CONN_FAILURE : ERR_RAN_IO_NONE;
}
/*
 * If you have something to send, you call this function for enqueing your message that will be later be written 
 * when the socket becomes writeable. If EV is NULL, it will not insert a reference of the sent message into the io_loop. 
 * This handles the case of indication related messages that have their own map for tracking the messages sent.
 */
static ran_errcodes_t io_ran_enqueue_for_next_write(const ws_ioloop_event_t *ev, const char *buf, size_t len) 
{
  if (buf == NULL) {
    lwsl_err("empty message to be sent\n");
    return ERR_RAN_IO_NONE;
  }
  
  ws_ioloop_t *loop_io_p = (ws_ioloop_t *)io_ran_instance->priv_data->user;
  if (ev != NULL) {
    lwsl_debug("io_loop: inserting event id %d\n", static_msg_id);
    bi_map_insert(&loop_io_p->ev, &static_msg_id, sizeof(int), ev, sizeof(ws_ioloop_event_t));
    static_msg_id++;
  }
  if (len > sizeof(loop_io_p->encmsg)){
    lwsl_err("internal limit attained. Discarded message longer than %ld\n", sizeof(loop_io_p->encmsg));
    return ERR_RAN_SEND_DATA;
  }
  strcpy(loop_io_p->encmsg, buf);
  
  if (lws_callback_on_writable_all_protocol(io_ran_instance->priv_data->ctx, &p[0]) !=0 )
    return ERR_RAN_SEND_DATA;

  return ERR_RAN_IO_NONE;
}

/*
 * Main function that will handle all the events and notifications. 
 * Here you will call the Read and write callbacks that the user will have set in 
 * set_write_cb() and set_fill_cb().
 */
static int io_ran_ws_async_loop(struct lws *wsi,                  // Opaque websocket instance pointer
                                enum lws_callback_reasons reason, // event that woke up this function
                                void *user,                       // internal buffer to exchange low level info among async operations (i.e. 
                                                                  // Read/WRITE)
                                void *in, size_t len              // Pointer to data that may be used for handling some events (i.e. when 
                                                                  // receiving data from the RAN)
                                ) 
{
  notif_e2_ran_event_t *notif_event = NULL;
  ran_if_t *ran_p = &io_ran_instance->priv_data->pa->ran_if;
  
  
  // Caveats: I/O user buffer handling in io_ran_ws_async_loop()
  // handling of function parameters 'user' and `wsi` does not work. Probably something wrong in its initialization of lws_protocol.
  // to bypass this problem, we will point directly to the allocated datastructure
  (void)user;
  (void)wsi;

  struct ws_ioloop_event_t loop_event; 
  ws_ioloop_t *loop_io_userds = &static_user;
    
  io_ran_instance->priv_data->ws_instance = wsi;
  
  switch (reason)
    {
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
      // the remote ws service is not up.
      lwsl_err("Unable to connect to RAN host at %s:%d. Reason: %s\n", 
                static_ran_conf.address, 
                static_ran_conf.port, 
                in ? (char *)in : "(null)");
      lwsl_info("Check that the amarisoft websocket process is up and running at that location.\n");
      goto do_retry;
      break;
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
      // the remote ws service is up.
      lwsl_user("RAN endpoint: connection established\n");
      break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
      lwsl_info("Getting data from the RAN endpoint: %s\n", in ? (char *)in : "(null)");
      
      next_msg_t ans = io_ran_instance->priv_data->read_cb(in, len, &loop_io_userds->ev);
      if (next_msg_is(ans, ASK_RAN_SETUP)){
        loop_event.msg_type = RAN_SETUP_REQUEST_PENDING_EVENT;
        io_ran_enqueue_for_next_write(&loop_event, ans.e2setup_msg.p, sizeof(loop_io_userds->encmsg));
      }
      else if (next_msg_is(ans, RAN_E2_CONFIG_FWD))
      {
        fwd_ran_e2_setup_request(io_ran_instance->priv_data->pa->e2_if, ans.config_msg.global_e2_node_id);
      }
      else if (next_msg_is(ans, RAN_E2_CTRL_FWD)){
        fwd_ran_e2_ctrl_reply(io_ran_instance->priv_data->pa->e2_if, ans.ctrl_msg);
      }
      // default is to send nothing to E2 or discard (programming error)
      break;
    case LWS_CALLBACK_CLIENT_WRITEABLE:
      /*
       * Caveats: When the socket becomes writable, you will get a `LWS_CALLBACK_SERVER_WRITEABLE' event managed by 
       * loop_callback() where you should generate the data to send and then send it with `lws_write()`.
       * Note that you cannot send data using `lws_write()` outside of the LWS_CALLBACK_SERVER_WRITEABLE event.
       * Assumption: you arrive here immediately after io_ran_enqueue_for_next_write()
       */
      if (io_ran_ws_write((uint8_t *)loop_io_userds->encmsg, strlen(loop_io_userds->encmsg)) != ERR_RAN_IO_NONE)
        lwsl_err("Message not sent to RAN endpoint: %s\n", loop_io_userds->encmsg);
      
      break;
    case LWS_CALLBACK_TIMER:
      // by design, the only case we arrive here is when you have to pull info from the RAN at a regular interval set by a timer 
      lwsl_info("[RAN - WS] It is time to ask for indication from the RAN (subscription to a %d ms frequency)\n", 
                ran_p->io->priv_data->indication_polling_ms);
      
      ind_event_t *ev = bi_map_extract_left(&ran_p->ind_event, &loop_io_userds->indication_sm_id, sizeof(loop_io_userds->indication_sm_id), NULL);
      assert ( ev != NULL && "missing indication event from the bi_map: should not happen\n");
      
      ran_io_rmtimer(loop_io_userds->indication_sm_id);

      const char *p = ran_p->ser->encode_indication(0, ev->sm->info.id(), (double)ws_ran_io.priv_data->indication_polling_ms/1000);
      
      io_ran_enqueue_for_next_write(NULL, p, sizeof(loop_io_userds->encmsg));
        
      // Set timer again for the same event
      ran_io_expiredtimer(loop_io_userds->indication_sm_id);
      bi_map_insert(&ran_p->ind_event, &loop_io_userds->indication_sm_id, sizeof(loop_io_userds->indication_sm_id), ev, sizeof(ind_event_t));
      free(ev);

      break;
    case LWS_CALLBACK_USER:
      // by design, the only case we arrive here is to read messages pushed from E2 on the notification queue
      notif_event = notif_get_ran_event(&io_ran_instance->priv_data->pa->ran_if);
      lwsl_user("I/O event loop: received event (type=%s)\n", notif_strevent(notif_event->type));
      ran_notif_msg_handle(ran_p, io_ran_instance->priv_data->pa->e2_if, notif_event, static_msg_id);

      if (notif_event->type == E2_ADD_SUBSCRIPTION_TIMER_EVENT)
        loop_io_userds->indication_sm_id = notif_event->subs_ev.sm_id; 
      
      break;
    case LWS_CALLBACK_CLIENT_CLOSED:
      lwsl_err("Client connection closed\n");
      goto do_retry;
    case LWS_CALLBACK_PROTOCOL_DESTROY:
      lwsl_user("Received request from RAN to close all: %s\n\n", ws_strerror(reason));
      ran_open_success = false;
      break;
    default:
      lwsl_user("WARN: callback event %d (%s) is not handled, ignoring it.\n", reason, ws_strerror(reason));
      break;
    }
  return 0;

  do_retry:
    /*
    * XXX-TODO: retry the connection to keep it nailed up
    *
    * Caveats (from docs):"If you set retry.conceal_count to be LWS_RETRY_CONCEAL_ALWAYS,
    * it will never give up and keep retrying at the last backoff delay plus the random jitter amount"
    */
    lwsl_user("Retry connection to WebSocket server is not implemented. Poised to close all.\n");
    ran_open_success = false;
    return 0;
}

static void io_ran_ws_free(void)
{
  lws_context_destroy(io_ran_instance->priv_data->ctx);
  io_ran_instance->priv_data->ctx = NULL;
  bi_map_free(&static_user.ev);
  (void)pthread_mutex_destroy(&io_ran_instance->priv_data->wsi_mtx);
}

io_ran_abs_t ws_ran_io = {  

  .open         = &io_ran_ws_open,

  .write_to_ran = &io_ran_enqueue_for_next_write,

  .send_notif   = &io_ran_ws_send_notif,
  
  .destroy      = &io_ran_ws_free,

  .priv_data    = NULL

};
/* ------------------------------------------ API ------------------------------------------------------------- */
io_ran_abs_t *io_ran_get_instance(struct io_ran_conf_t conf,
                                  next_msg_t (*set_read_cb)(const char *buf, size_t len, bi_map_t *sent_msg))
{
  assert (io_ran_instance == NULL  && "programming error. just one RAN instance for WS is allowed\n");
  
  io_ran_instance = &ws_ran_io;
  memcpy(&static_ran_conf, &conf, sizeof(conf) );
  lws_set_log_level(conf.logl, NULL);
  lwsl_user("WS interface: performing initialization of I/O module\n");

  // static const struct lws_protocols p[] = {     
  //     {
  //       "ws-client",            // 1. protocol_name
  //       &io_ran_ws_async_loop,   // 2. callback for servicing the async loop
  //       sizeof(struct ws_ioloop_event_t),// 3. per-connection data size (sizeof(param 6))
  //       0,                      // 4. rx_buffer_size (opt)
  //       0,                      // 5. id  (opt)
  //       &static_user,           // 6. pointer to an external data storage having size specified by parameter number 3 that we want  
  //                               //    to be accessible from the callback. This external storage will be used as internal notification mechanism
  //                               //    to be read by the function that loops on the I/O available. 
  //       0                       // 7. tx_packet_size (opt)
  //     },
  //     {NULL, NULL, 0, 0, 0, NULL, 0}
  // };
  struct lws_context_creation_info info;
  memset(&info, 0, sizeof info);
  info.port                 = CONTEXT_PORT_NO_LISTEN; /* set port like this to just have a websocket client without a server*/
  info.protocols            = p;
  info.fd_limit_per_thread  = 1 + 1 + 1;
  
  io_ran_instance->priv_data = calloc(1, sizeof(*io_ran_instance->priv_data));
  assert(io_ran_instance->priv_data != NULL && "Memory exhausted\n");

  io_ran_instance->priv_data->ctx = lws_create_context(&info);
  assert(io_ran_instance->priv_data->ctx != NULL && "Error creating the context\n");
  io_ran_instance->priv_data->indication_polling_ms = conf.timer;
  io_ran_instance->priv_data->read_cb = set_read_cb;
  io_ran_instance->priv_data->pa = get_proxy_agent();
  io_ran_instance->priv_data->e2_setup_done = false;
  io_ran_instance->priv_data->user = &static_user;
  pthread_mutexattr_t mtx_attr = {0};
  (void)pthread_mutexattr_init(&mtx_attr); 
  (void)pthread_mutexattr_settype(&mtx_attr, PTHREAD_MUTEX_ERRORCHECK);
  assert(pthread_mutex_init(&io_ran_instance->priv_data->wsi_mtx, &mtx_attr) == 0);
  // caveats: we use a bi_map as there is not map implemented in our internal util lib. 
  // The key will be the msg_id integer value, so the cmp_* function for
  // io_loop_event will not be implemented as we do not use.
  bi_map_init(&static_user.ev, sizeof(int), sizeof(struct ws_ioloop_event_t), cmp_int, cmp_event, free_io_event, free_int); 

  set_IndTimer_sts(false);
  ran_io_inittimers();

  return io_ran_instance;
}

/* 
 * Add a timer in RAN I/O module for the subscription to indication messages.
 * This call will track the timers of the various active Service Models subscriptions. Note that we discard the timer values 
 * from E2 subscription as we decide by design to operate our own fixed timer for all SMs set at opening time of I/O module. 
 * We anyway log this design decision for the user.
 */
void ran_io_addtimer(int sm_id, long int e2_subscription_tm)
{
  
  lwsl_info("We were asked for a timer of %ld ms for SM %d. We will set internally for the RAN I/O cycle a timer at %d ms\n", 
            e2_subscription_tm, sm_id, io_ran_instance->priv_data->indication_polling_ms);
  for (size_t i = 0; i< sizeof(io_ran_instance->priv_data->io_timers)/sizeof(int); i++)
  {
    if (io_ran_instance->priv_data->io_timers[i] == sm_id){
      lwsl_info("Timer related to SM %d was already present in the internal list I/O RAN timers. Skipping overwriting\n", sm_id);
      break;
    } else if (io_ran_instance->priv_data->io_timers[i] == 0){
      io_ran_instance->priv_data->io_timers[i] = sm_id;
      break;
    }
  }
  if (!io_ran_instance->priv_data->timer_set){
    lws_set_timer_usecs(io_ran_instance->priv_data->ws_instance, io_ran_instance->priv_data->indication_polling_ms * 1000);
    io_ran_instance->priv_data->timer_set = true;
  }
}

void ran_io_rmtimer(int sm_id)
{
  for (size_t i = 0; i< sizeof(io_ran_instance->priv_data->io_timers)/sizeof(int); i++){
    if (io_ran_instance->priv_data->io_timers[i] == sm_id)
    {
      io_ran_instance->priv_data->io_timers[i] = 0;
      lwsl_info("removing timer for SM %d\n", sm_id);
      break;
    }
  }

  if (ran_io_anytimer() == false) {
    lws_set_timer_usecs(io_ran_instance->priv_data->ws_instance, LWS_SET_TIMER_USEC_CANCEL);
    lwsl_info("Remove I/O RAN timer triggered by SM %d as it was the only timer left among all the SMs\n", sm_id);
    io_ran_instance->priv_data->timer_set = false;
  }
}

void set_E2setup_sts(atomic_bool sts) {io_ran_instance->priv_data->e2_setup_done = sts;}
bool get_E2setup_sts(void) {return io_ran_instance->priv_data->e2_setup_done;};
bool get_IndTimer_sts(void) {return io_ran_instance->priv_data->ind_timer_ready;};
void set_IndTimer_sts(atomic_bool sts) {io_ran_instance->priv_data->ind_timer_ready = sts;}

