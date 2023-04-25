#include <assert.h>
#include <stddef.h>

#include "e2sm_ccc_event_trigger_frm_3.h"

void free_ccc_event_trigger_frm_3(e2sm_ccc_ev_trg_frm_3_t *src)
{
  // No memory allocated in the heap
  (void)src;
  
}

bool eq_ccc_event_trigger_frm_3(e2sm_ccc_ev_trg_frm_3_t const* m0,  e2sm_ccc_ev_trg_frm_3_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->report_period != m1->report_period)
    return false;


  return true;
}
