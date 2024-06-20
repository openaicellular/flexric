#ifndef CCC_SM_PEND_MSG_H
#define CCC_SM_PEND_MSG_H

#include <stdbool.h>
#include <pthread.h>

#include "io_ran/ccc_msgs_amr.h"
#include "../../util/alg_ds/ds/latch_cv/latch_cv.h"

// All ptrs are non-owning
typedef struct{
  // Non-owning ptr
  ccc_msgs_amr_t* msg;

  // Data needed to notify
  // that msgs are ready 
  latch_cv_t latch;
} ccc_pend_msg_t;

ccc_pend_msg_t init_ccc_pend_msg(ccc_msgs_amr_t* msg);

void free_ccc_pend_msg(ccc_pend_msg_t* c);

void notify_part_filled_cccp(ccc_pend_msg_t* c);

void wait_untill_filled_cccp(ccc_pend_msg_t* c);

#endif


