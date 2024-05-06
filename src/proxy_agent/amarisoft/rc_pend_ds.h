#ifndef RC_PENDING_DATA_STRUCTURE_H
#define RC_PENDING_DATA_STRUCTURE_H 

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "rc_pend_msg.h"
#include <pthread.h>

typedef struct{
  // key: msg_id, val: rc_sm_pend_t*
  assoc_rb_tree_t t;  
  pthread_mutex_t mtx;
} rc_pend_ds_t;

void init_rc_pend_ds(rc_pend_ds_t* src);

void free_rc_pend_ds(rc_pend_ds_t* src);

// Removes the msg_id from the RB tree
rc_pend_msg_t* extract_rc_pend_ds(rc_pend_ds_t* src, int msg_id);

// msg_id->key : rc_pend_t->value
void add_rc_pend_ds(rc_pend_ds_t* src, int msg_id, rc_pend_msg_t* val);

#endif


