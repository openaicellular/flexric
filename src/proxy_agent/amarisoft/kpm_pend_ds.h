#ifndef KPM_PENDING_DATA_STRUCTURE_H
#define KPM_PENDING_DATA_STRUCTURE_H 

#include "../../util/alg_ds/ds/assoc_container/assoc_generic.h"
#include "kpm_pend_msg.h"
#include <pthread.h>

typedef struct{
  // key: msg_id, val: kpm_sm_pend_t*
  assoc_rb_tree_t t;  
  pthread_mutex_t mtx;
} kpm_pend_ds_t;

void init_kpm_pend_ds(kpm_pend_ds_t* src);

void free_kpm_pend_ds(kpm_pend_ds_t* src);

// Removes the msg_id from the RB tree
kpm_pend_msg_t* extract_kpm_pend_ds(kpm_pend_ds_t* src, int msg_id);

// msg_id->key : kpm_pend_t->value
void add_kpm_pend_ds(kpm_pend_ds_t* src, int msg_id, kpm_pend_msg_t* val);

#endif
