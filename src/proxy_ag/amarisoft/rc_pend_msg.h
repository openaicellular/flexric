#ifndef RC_SM_PEND_MSG_H
#define RC_SM_PEND_MSG_H 

#include <stdbool.h>
#include <pthread.h>

#include "rc_msgs_amr.h"
#include "../../util/alg_ds/ds/latch_cv/latch_cv.h"

// All ptrs are non-owning
typedef struct{
  // Non-owning ptr
  msg_ue_get_t *ues;
  // Non-owning ptr
  config_get_amr_t* cfg;
  // Non-owning ptr
  msg_ho_ans_amr_t* ho; 

  // Data needed to notify
  // that msgs are ready 
  latch_cv_t latch;
} rc_pend_msg_t;

rc_pend_msg_t init_rc_pend_msg(rc_msgs_amr_t* msg);

void free_rc_pend_msg(rc_pend_msg_t* k);

void notify_part_filled_rp(rc_pend_msg_t* k);

void wait_untill_filled_rp(rc_pend_msg_t* k);

#endif


