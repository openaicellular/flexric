#ifndef KPM_SM_PEND_MSG_H
#define KPM_SM_PEND_MSG_H 

#include <stdbool.h>
#include <pthread.h>

#include "io_ran/kpm_msgs_amr.h"

#include "../../util/alg_ds/ds/latch_cv/latch_cv.h"

// All ptrs are non-owning
typedef struct{
  // Non-owning ptr
  kpm_msgs_amr_t* msg; 
  // Data needed to notify
  // that msgs are ready 
  latch_cv_t latch;
} kpm_pend_msg_t;

kpm_pend_msg_t init_kpm_pend_msg(kpm_msgs_amr_t* msg);

void free_kpm_pend_msg(kpm_pend_msg_t* k);

void notify_part_filled_kp(kpm_pend_msg_t* k);

void wait_untill_filled_kp(kpm_pend_msg_t* k);

#endif
