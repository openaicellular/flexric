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
