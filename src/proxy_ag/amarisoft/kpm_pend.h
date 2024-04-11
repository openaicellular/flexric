#ifndef KPM_SM_PEND_H
#define KPM_SM_PEND_H 

#include <stdbool.h>
#include <pthread.h>

#include "ie/msg_stats.h"
#include "ie/msg_ue_get.h"

#include "../../util/alg_ds/ds/latch_cv/latch_cv.h"

// All ptrs are non-owning
typedef struct{
  // Non-owning ptr
  msg_stats_amr_t *stats;
  // Non-owning ptr
  msg_ue_get_t *ues;

  // Data needed to notify
  // that msgs are ready 
  latch_cv_t latch;
} kpm_pend_t;

kpm_pend_t init_kpm_pend(msg_stats_amr_t *out_stats,  msg_ue_get_t *out_ues);

void free_kpm_pend(kpm_pend_t* k);

void notify_part_filled_kp(kpm_pend_t* k);

void wait_untill_filled_kp(kpm_pend_t* k);

#endif
