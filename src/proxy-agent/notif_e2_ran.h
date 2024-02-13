#ifndef NOTIF_E2_RAN_H
#define NOTIF_E2_RAN_H
/* 
 * This interface provides a single-thread internal notification between E2 and RAN interface.
 * 
 * The notification system provides a bidirection communication between RAN and E2 interfaces for sending custom commands. 
 * The information to be transported can be modeled as a message ServiceModel-specific that is sent at irregular time but with a 
 * low frequency. The RAN and E2 interface will need to read this message in the asynchronous I/O module and detect which 
 * operation the message came from and decide what to do after. Note that the messages does not need to 'travel' among the two interfaces
 * as interfaces are part of the same process.
 * The creation, free and extraction of an event from E2 I/O loop is already implemented by e2_agent.c:pend_event() so will not be provided by this API.
 */
#include "agent/e2_agent.h"
#include "agent/e2_agent_api.h"
#include "lib/ind_event.h"
#include "lib/pending_events.h"

#include "e2_sm_agent.h"

typedef struct e2_agent_s e2_agent_t; // forward declaration to avoid circular dependencies

typedef enum notif_event_t
{
  // RAN->E2
  RAN_SETUP_REQUEST_PENDING_EVENT = SETUP_REQUEST_PENDING_EVENT,
  // E2->RAN 
  E2_ADD_SUBSCRIPTION_TIMER_EVENT, 
  E2_REMOVE_SUBSCRIPTION_TIMER_EVENT,
  E2_CTRL_EVENT,
  E2_WRITE_SUBSCRIPTION_EVENT,
  E2_REMOVE_RC_SUBSCRIPTION_EVENT,
} notif_event_t;

typedef struct e2_unsubscribe_event_t {
  int sm_id;  
} e2_unsubscribe_event_t;

typedef struct e2_subscribe_event_t {
  int          sm_id;      
  long int     time_ms;
  ric_gen_id_t ric_id; 
  uint8_t      ric_action_id;
  void* act_def;
} e2_subscribe_event_t;

typedef struct e2_wr_sub_event_t {
  uint32_t ric_req_id;
  e2sm_rc_event_trigger_t et;
  e2sm_rc_action_def_t ad; // Support 1 ad
} e2_wr_sub_event_t;

// types of events managed by I/O ran loop notification signal
typedef struct notif_e2_ran_event_t {
  union {
    e2_unsubscribe_event_t  unsubs_ev;
    e2_subscribe_event_t    subs_ev;
    ctrl_ev_t               ctrl_ev;
    ctrl_ev_reply_t         ctrl_ev_reply;
    e2_wr_sub_event_t       wr_subs_ev;
  }; // anonymous union discriminated by `type` field
  notif_event_t type;
} notif_e2_ran_event_t;

/* -- RAN I/O module functionalities -- */

void notif_init_ran(ran_if_t *ran_if);
void notif_free_ran(ran_if_t *ran_if);

/* 
 * Extract an event from the RAN I/O container. This function is meant to be called iteratively and remove the 
 * messages from the container until returns NULL. A returned NULL signals that all the messages have been read
 * from the container.
 */
notif_e2_ran_event_t *notif_get_ran_event(ran_if_t *ran_if);
int cmp_notif_event(void const* pend_v1, void const* pend_v2);
char * notif_strevent(enum notif_event_t type);

/* handle the RAN I/O loop notification messages coming from E2 */
void ran_notif_msg_handle(ran_if_t *ran_if, e2_agent_t *e2_if, const notif_e2_ran_event_t *notif_event, int msg_id) ;

/* -- E2AP functionalities --- */
void fwd_e2_ran_wr_sub_ev(ran_if_t *ran_if, wr_rc_sub_data_t* wr_rc_sub_data);
void fwd_e2_ran_subscription_timer(ran_if_t *ran_if, ind_event_t ev, long interval_ms) ;
void fwd_e2_ran_remove_subscription_timer(ran_if_t *ran_if, ric_gen_id_t ric_id);
void fwd_e2_ran_ctrl (ran_if_t *ran_if, ctrl_ev_t in);
void fwd_ran_e2_setup_request(e2_agent_t *e2_if, global_e2_node_id_t ge2ni);
void fwd_ran_e2_ric_subscription_response(e2_agent_t *e2_if, e2_subscribe_event_t reply);
void fwd_ran_e2_ctrl_reply (e2_agent_t *e2_if, ctrl_ev_reply_t reply);

e2_setup_request_t generate_setup_request_from_ran(e2_agent_t* ag, global_e2_node_id_t ge2ni);
#endif