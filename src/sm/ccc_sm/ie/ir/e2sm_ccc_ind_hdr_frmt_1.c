#include "e2sm_ccc_ind_hdr_frmt_1.h"

#include <assert.h>

void free_e2sm_ccc_ind_hdr_frmt_1(e2sm_ccc_ind_hdr_frmt_1_t* src)
{
  assert(src != NULL);
  assert(src->ind_reason != IND_REASON_PERIODIC
        || src->ind_reason != IND_REASON_UPON_CHANGE
        || src->ind_reason != IND_REASON_UPON_SUB);

  free_byte_array(src->event_time);
}

bool eq_e2sm_ccc_ind_hdr_frmt_1(e2sm_ccc_ind_hdr_frmt_1_t const* m0,e2sm_ccc_ind_hdr_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->ind_reason != m1->ind_reason)
    return false;

  if(eq_byte_array(&m0->event_time, &m1->event_time) == false)
    return false;

  return true;
}

e2sm_ccc_ind_hdr_frmt_1_t cp_e2sm_ccc_ind_hdr_frmt_1(e2sm_ccc_ind_hdr_frmt_1_t const* src)
{
  assert(src != NULL);
  assert(src->ind_reason != IND_REASON_PERIODIC
         || src->ind_reason != IND_REASON_UPON_CHANGE
         || src->ind_reason != IND_REASON_UPON_SUB);

  e2sm_ccc_ind_hdr_frmt_1_t dst = {.ind_reason = src->ind_reason};

  dst.event_time = copy_byte_array(src->event_time);

  return dst;
}

