/*
 * generic transport API for connecting agent to RAN
 */
#ifndef RAN_IF_H
#define RAN_IF_H

// TODO: provide an extern declaration for ctx_t that has to be implemented by the transport (i.e. ws) to avoid including ws_ran_if.h
// and decouple this generic interface to the WS implementation
#include "ws_ran_if.h"
#include "util/alg_ds/ds/assoc_container/bimap.h"

typedef struct lws_context ctx_t;

typedef struct ran_if_t
{
  ctx_t * (*init)(struct proxy_conf_t conf);
  void (*conn)(ctx_t *ctx);
  int (*poll)(ctx_t *ctx);
  void (*free)(ctx_t *ctx);
  
  ctx_t *ctx;      // websocket context. 
                   // XXX-REFACTOR: lws->context = ctx. You may probably remove the need of ctx in this interface and just bring around the interface the lws.
                   // XXX-REFACTOR: signature of conn()/poll/free do not need anymore the ctx as lws is now part of the same data structure.
  struct lws *lws; // websocket connection instance (WSI)
  
  // Registered Indication events
  bi_map_t ind_event; // key1:timer_id, key2:ind_event_t 
  
  // Pending events
  // bi_map_t pending; // left: fd, right: pending_event_t 
  struct ws_async_event_t  *user; // pointer to global variable holding the data to be shared in the loop function. This is valued at init time.
} ran_if_t;

#endif