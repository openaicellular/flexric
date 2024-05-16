#ifndef CCC_PENDING_DATA_STRUCTURE_H
#define CCC_PENDING_DATA_STRUCTURE_H

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "ccc_pend_msg.h"
#include <pthread.h>

typedef struct{
  // key: msg_id, val: ccc_sm_pend_t*
  assoc_rb_tree_t t;  
  pthread_mutex_t mtx;
} ccc_pend_ds_t;

void init_ccc_pend_ds(ccc_pend_ds_t* src);

void free_ccc_pend_ds(ccc_pend_ds_t* src);

// Removes the msg_id from the RB tree
ccc_pend_msg_t* extract_ccc_pend_ds(ccc_pend_ds_t* src, int msg_id);

// msg_id->key : ccc_pend_t->value
void add_ccc_pend_ds(ccc_pend_ds_t* src, int msg_id, ccc_pend_msg_t* val);

#endif


