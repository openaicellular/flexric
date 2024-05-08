#include "e2sm_ccc_ev_trg_frmt_3.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_ccc_ev_trg_frmt_3(e2sm_ccc_ev_trg_frmt_3_t* src)
{
  assert(src != NULL);
  assert(src->period > 0 && src->period < 4294967295);
}

bool eq_e2sm_ccc_ev_trg_frmt_3(e2sm_ccc_ev_trg_frmt_3_t const* m0, e2sm_ccc_ev_trg_frmt_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  assert(m0->period > 0 && m0->period < 4294967295);
  assert(m1->period > 0 && m1->period < 4294967295);
  if(m0->period != m1->period)
    return false;

  return true;
}

e2sm_ccc_ev_trg_frmt_3_t cp_e2sm_ccc_ev_trg_frmt_3(e2sm_ccc_ev_trg_frmt_3_t const* src)
{
  assert(src != NULL);
  assert(src->period > 0 && src->period < 4294967295);
  e2sm_ccc_ev_trg_frmt_3_t dst = {.period = src->period};

  return dst;
}

