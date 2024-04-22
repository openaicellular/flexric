#ifndef PENDING_EVENT_PROXY_MIR_H
#define PENDING_EVENT_PROXY_MIR_H 

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h" 

#include "../../util/alg_ds/ds/assoc_container/bimap.h"

#include <pthread.h>
#include "../../lib/pending_events.h"
#include "asio_agent_amr.h"
#include "ws_orig_msg.h"

typedef struct{
  //seq_ring_t arr; //  fd_pending_event_t
  assoc_rb_tree_t tree_msg_id; // key: msg_id; value: fd_pending_event_t
  assoc_rb_tree_t tree_fd; // key: fd; value: fd_pending_event_t

  pthread_mutex_t mtx_mi;
  pthread_mutex_t mtx_fd;
} pend_ev_prox_t;

void init_pend_ev_prox(pend_ev_prox_t *src);

void free_pend_ev_prox(pend_ev_prox_t* src);


void add_pend_ev_prox(pend_ev_prox_t *p, asio_agent_amr_t *io, int msg_id, pending_event_t ev, ws_orig_msg_e orig_msg); 

ws_orig_msg_e rm_pend_ev_prox(pend_ev_prox_t* p, asio_agent_amr_t* io, int msg_id);

// Expected message id or not found 
typedef struct{
  int msg_id;
  bool found;
} exp_msg_id_t;

exp_msg_id_t find_pend_ev_prox(pend_ev_prox_t* p, int fd);


#endif
