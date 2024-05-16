#include "ccc_pend_msg.h"
#include <assert.h>

ccc_pend_msg_t init_ccc_pend_msg(ccc_msgs_amr_t* msg)
{
  assert(msg != NULL);

  ccc_pend_msg_t dst = {.msg = msg};

  size_t const num_msgs = 2;
  dst.latch = init_latch_cv(num_msgs);

  return dst;
}

void free_ccc_pend_msg(ccc_pend_msg_t* r)
{
  assert(r != NULL);

  free_latch_cv(&r->latch);
}

void notify_part_filled_cccp(ccc_pend_msg_t* r)
{
  assert(r != NULL);

  count_down_latch_cv(&r->latch);
}

void wait_untill_filled_cccp(ccc_pend_msg_t* r)
{
  assert(r != NULL);

  wait_latch_cv(&r->latch);
}


