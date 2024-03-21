#include <assert.h>
#include <stdlib.h>

#include "e2sm_ccc_ev_trg_frmt_1.h"

void free_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t* src)
{
  assert(src != NULL);

}

bool eq_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t const* m0, e2sm_ccc_ev_trg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return true;
}

e2sm_ccc_ev_trg_frmt_1_t cp_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ev_trg_frmt_1_t dst = {0}; 

  return dst;
}

