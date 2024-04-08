#ifndef PENDING_EVENT_PROXY_MIR_H
#define PENDING_EVENT_PROXY_MIR_H 

typedef struct{


  pthread_mutex_t mtx;
  assoc_rb_tree_t tree;

} pend_ev_prox_t;




#endif
