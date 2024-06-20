#include "kpm_pend_msg.h"

#include <assert.h>

kpm_pend_msg_t init_kpm_pend_msg(kpm_msgs_amr_t* msg)
{
  assert(msg != NULL);

  kpm_pend_msg_t dst = {.msg = msg};

  size_t const num_msgs = 3;
  dst.latch = init_latch_cv(num_msgs);

  return dst;
}

void free_kpm_pend_msg(kpm_pend_msg_t* k)
{
  assert(k != NULL);
  // Non-owning ptr
  // msg_stats_amr_t *stats;
  // Non-owning ptr
  // msg_ue_get_t *ues;

  free_latch_cv(&k->latch);
}

void notify_part_filled_kp(kpm_pend_msg_t* k)
{
  assert(k != NULL);

  count_down_latch_cv(&k->latch);
}

void wait_untill_filled_kp(kpm_pend_msg_t* k)
{
  assert(k != NULL);

  wait_latch_cv(&k->latch);
}

