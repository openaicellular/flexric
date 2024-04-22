#include "rc_pend_msg.h"
#include <assert.h>

rc_pend_msg_t init_rc_pend_msg(rc_msgs_amr_t* msg)
{
  assert(msg != NULL);

  rc_pend_msg_t dst = {.ues = &msg->ues, .cfg = &msg->cfg };

  size_t const num_msgs = 2;
  dst.latch = init_latch_cv(num_msgs);

  return dst;
}

void free_rc_pend_msg(rc_pend_msg_t* r)
{
  assert(r != NULL);
  // Non-owning ptr
  // msg_stats_amr_t *stats;
  // Non-owning ptr
  // msg_ue_get_t *ues;

  free_latch_cv(&r->latch);
}

void notify_part_filled_rp(rc_pend_msg_t* r)
{
  assert(r != NULL);

  count_down_latch_cv(&r->latch);
}

void wait_untill_filled_rp(rc_pend_msg_t* r)
{
  assert(r != NULL);

  wait_latch_cv(&r->latch);
}


