#ifndef WS_IO_RAN_H
#define WS_IO_RAN_H

/* Async implementation of abstract RAN I/O module using websockets as transport */
#include <libwebsockets.h>

#include "util/alg_ds/ds/assoc_container/bimap.h"
#include "lib/ind_event.h"
#include "lib/async_event.h"

#include "notif_e2_ran.h"
#include "e2_sm_agent.h"


struct io_ran_conf_t {
  char                address[128]; /* ip address */
  int                 port;         /* port */
  enum lws_log_levels logl;         /* loglevel*/
  int                 timer;
};

/* 
 * internal I/O loop data structure that the user can use to pass data related to various functionalities 
 * (i.e. indication, json messages, etc..).
  */
typedef struct ws_ioloop_t 
{
  bi_map_t        ev;               // will contain commands in progress towards the RAN endpoint
  char            encmsg[1024];     // message already encoded (in json) to be sent to RAN
  int             indication_sm_id; // Service model ID: to be used just by indication message flow
} ws_ioloop_t;

typedef struct ws_ioloop_event_t 
{
  union {
    e2_unsubscribe_event_t              unsubs_ev;
    e2_subscribe_event_t                subs_ev;
    ric_gen_id_t                        ctrl_ev;
    struct {                        }   setup_ev;
  }; // anonymous union discriminated by `type` field 
  notif_event_t msg_type;
} ws_ioloop_event_t ;

//ws_ioloop_t static_user; // XXX: probably you can bring inside the implementation instead of exposing it

void ran_io_rmtimer(int sm_id);
void ran_io_addtimer(int sm_id, long int e2_subscription_tm);

// access to internals state machine of the RAN interface that keeps track on some messages already processed
// TO-FIX: still some work to do to make it thread safe using atomics.
void set_E2setup_sts(atomic_bool sts);
bool get_E2setup_sts(void);
bool get_IndTimer_sts(void);
void set_IndTimer_sts(atomic_bool sts);

#endif