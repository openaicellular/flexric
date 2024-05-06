#ifndef RC_SM_PEND_MSG_H
#define RC_SM_PEND_MSG_H 

#include <stdbool.h>
#include <pthread.h>

#include "io_ran/rc_msgs_amr.h"
#include "../../util/alg_ds/ds/latch_cv/latch_cv.h"

// All ptrs are non-owning
typedef struct{
  // Non-owning ptr
  rc_msgs_amr_t* msg;

  // Data needed to notify
  // that msgs are ready 
  latch_cv_t latch;
} rc_pend_msg_t;

rc_pend_msg_t init_rc_pend_msg(rc_msgs_amr_t* msg);

void free_rc_pend_msg(rc_pend_msg_t* k);

void notify_part_filled_rp(rc_pend_msg_t* k);

void wait_untill_filled_rp(rc_pend_msg_t* k);

#endif


