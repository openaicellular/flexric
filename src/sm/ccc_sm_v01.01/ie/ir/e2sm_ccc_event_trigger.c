#include <assert.h>

#include "e2sm_ccc_event_trigger.h"

void free_ccc_event_trigger_def(e2sm_ccc_event_trigger_t *src)
{
  assert(src != NULL);

  switch (src->type)
  {
  case FORMAT_1_EVENT_TRIGGER_CCC:
    free_ccc_event_trigger_frm_1(&src->frm_1);
    break;

  case FORMAT_2_EVENT_TRIGGER_CCC:
    free_ccc_event_trigger_frm_2(&src->frm_2);
    break;

  case FORMAT_3_EVENT_TRIGGER_CCC:
    free_ccc_event_trigger_frm_3(&src->frm_3);
    break;
    
  default:
    assert(false && "Unknown CCC Event Trigger Definition Type");
  }    
}

bool eq_ccc_event_trigger_def(e2sm_ccc_event_trigger_t const* m0,  e2sm_ccc_event_trigger_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->type != m1->type)
    return false;

  switch (m0->type)
  {
  case FORMAT_1_EVENT_TRIGGER_CCC:
    if (eq_ccc_event_trigger_frm_1(&m0->frm_1, &m1->frm_1) != true)
      return false;
    break;

  case FORMAT_2_EVENT_TRIGGER_CCC:
    if (eq_ccc_event_trigger_frm_2(&m0->frm_2, &m1->frm_2) != true)
      return false;
    break;

  case FORMAT_3_EVENT_TRIGGER_CCC:
    if (eq_ccc_event_trigger_frm_3(&m0->frm_3, &m1->frm_3) != true)
      return false;
    break;
  
  default:
    assert(false && "Unknown CCC Event Trigger Definition Type");
  }
  
  return true;
}
