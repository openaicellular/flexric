#ifndef PENDING_EVENT_PROXY_MIR_H
#define PENDING_EVENT_PROXY_MIR_H 

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h" 
#include <pthread.h>
#include "../../lib/pending_events.h"

typedef struct{
  assoc_rb_tree_t tree; // key: int (msg_id) ; value pending_event_t
  pthread_mutex_t mtx;
} pend_ev_prox_t;

void init_pend_ev_prox(pend_ev_prox_t *src);

void free_pend_ev_prox(pend_ev_prox_t* src); 

void add_pend_ev_prox(pend_ev_prox_t* p, int msg_id, pending_event_t ev);

void rm_pend_ev_prox(pend_ev_prox_t* src, int msg_id);

#endif
