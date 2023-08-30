#include "kpm_ric_event_trigger_def.h"

#include <assert.h>
#include <stdlib.h>

void free_kpm_event_trigger_def(kpm_event_trigger_def_t* src)
{
  // No memory allocated in the heap
  (void)src;
}

bool eq_kpm_event_trigger_def( kpm_event_trigger_def_t const* m0,  kpm_event_trigger_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0->type != m1->type)
    return false;

  switch (m0->type)
  {
  case FORMAT_1_RIC_EVENT_TRIGGER:
    if (eq_kpm_event_trigger_def_frm_1(&m0->kpm_ric_event_trigger_format_1, &m1->kpm_ric_event_trigger_format_1) != true)
      return false;
    break;
  
  default:
    assert(false && "Unknown Event Trigger Format Type");
  }

  return true;
}


kpm_event_trigger_def_t cp_kpm_event_trigger_def(const kpm_event_trigger_def_t * src)
{
  assert(src != NULL);

  kpm_event_trigger_def_t dst = {0};

  switch (src->type)
  {
  case FORMAT_1_RIC_EVENT_TRIGGER:
    dst.kpm_ric_event_trigger_format_1 = cp_kpm_event_trigger_def_frm_1(&src->kpm_ric_event_trigger_format_1);
    break;
  
  default:
    assert(false && "Unknown Event Trigger Format Type");
  }

  return dst;
}
