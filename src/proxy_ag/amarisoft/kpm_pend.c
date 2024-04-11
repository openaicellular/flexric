#include "kpm_pend.h"

#include <assert.h>

kpm_pend_t init_kpm_pend(msg_stats_amr_t *out_stats,  msg_ue_get_t *out_ues)
{
  assert(out_stats != NULL);
  assert(out_ues != NULL);

  kpm_pend_t dst = {.stats = out_stats, .ues = out_ues };

  size_t const num_msgs = 2;
  dst.latch = init_latch_cv(num_msgs);

  return dst;
}

void free_kpm_pend(kpm_pend_t* k)
{
  assert(k != NULL);
  // Non-owning ptr
  // msg_stats_amr_t *stats;
  // Non-owning ptr
  // msg_ue_get_t *ues;

  free_latch_cv(&k->latch);
}

void notify_part_filled_kp(kpm_pend_t* k)
{
  assert(k != NULL);

  count_down_latch_cv(&k->latch);
}

void wait_untill_filled_kp(kpm_pend_t* k)
{
  assert(k != NULL);

  wait_latch_cv(&k->latch);
}

