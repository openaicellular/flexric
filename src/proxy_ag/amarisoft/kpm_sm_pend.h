#ifndef KPM_SM_PEND_H
#define KPM_SM_PEND_H 

#include <stdbool.h>
#include <pthread.h>

#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"

// All ptrs are non-owning
typedef struct{
  // Non-owning ptr
  msg_stats_amr_t *stats;
  // Non-owning ptr
  msg_ue_get_t *ues;

  // Data needed to notify
  // that msgs are ready 
  pthread_cond_t* cond;
  pthread_mutex_t* mtx;
  bool* done;

  bool stats_filled;
  bool ues_filled;
} kpm_sm_pend_t;

#endif
