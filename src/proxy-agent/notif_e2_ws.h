
#ifndef NOTIF_E2_WS_H
#define NOTIF_E2_WS_H

#include "ran_if.h"
#include "proxy_agent.h"

#include "../lib/ind_event.h"

/* This interface handles internal notification between E2 and WS interface*/

// 1. direction: E2- > WS

// 1.1 Subscription request

/* set timer for indication message at WS side*/
void fwd_e2_ws_subscription_timer(ran_if_t *ran_if, ind_event_t ev, long initial_ms, long interval_ms);

/* remove timer for indication message event at WS side*/
void fwd_e2_ws_remove_subscription_timer(ran_if_t *ran_if, ric_gen_id_t ric_id);

// 1.2 Control
// TBD 

// 2. direction: WS -> E2

// 2.1 RAN function definition
/* 
 * function is called by the WS interface for creating the E2 Setup request from E2 interface. 
 * Alongside, it will perform the init of E2 interface as well, starting it in another thread and 
 * assigning the proxy_agent handler for E2 interface. the other thread can be accessed via get_e2_agent_thread_id() 
 * E2 Setup request gets from the RAN the global_e2_node_id_t values that will be later filled into the E2 setup 
 * request by static_start_agent() function, while the RAN Function definition will contain all the SMs loaded at run time.
 */
void fwd_ws_e2_setup_request(proxy_agent_t *p, global_e2_node_id_t ge2ni);

pthread_t get_e2_agent_thread_id(void);

// 2.2 Control
// TBD

#endif