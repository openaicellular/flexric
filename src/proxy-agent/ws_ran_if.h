/*
 * Async IO communication layer for Websocket interface (WS) that communicates with the RAN
 * It will handle: 
 * - connection looping managing events and read/write from/to RAN 
 * - timer setting and handling
 */
#ifndef WS_RAN_IF_H
#define WS_RAN_IF_H

#include <libwebsockets.h>
#include "proxy_agent_conf.h"
#include "../lib/ind_event.h"
#include "../lib/async_event.h"

/* 
 * internal data structure that the user can use in the main loop callback (loop_callback) to pass some data (i.e.
 * to correlate with something that has been sent) among the various events (i.e. TIMER)
 * see https://github.com/warmcat/libwebsockets/blob/main/minimal-examples-lowlevel/ws-client/minimal-ws-client-echo/protocol_lws_minimal_client_echo.c#L33
 * if you want to use a lws_ring as data type instead of a plain async_event
 */
struct ws_async_event_t 
{
  union {
    ind_event_t* i_ev;
    //pending_event_t* p_ev;
  }; // various types of events
  async_event_e type;       // for selecting the union element
  int           req_id;     // request id: monotonically increasing to avoid collision.
  char          jsonmsg[1024]; // message to be sent to RAN in encoded form already
  int           timer_id;
  long          timer_ms;
};

// Websocket functions that implement the ran_if.h
struct lws_context * ws_init(struct proxy_conf_t conf);
void ws_conn(struct lws_context *ctx);
int ws_poll(struct lws_context *ctx);
void ws_free(struct lws_context *ctx);

// timer setting
// XXX: this is to circumventing issue on notification from E2 interface to WS async management
struct ws_async_event_t static_user;
#endif