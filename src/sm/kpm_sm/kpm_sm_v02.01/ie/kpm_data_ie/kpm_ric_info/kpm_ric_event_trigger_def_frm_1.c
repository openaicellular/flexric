#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "kpm_ric_event_trigger_def_frm_1.h"

bool eq_kpm_event_trigger_def_frm_1(kpm_ric_event_trigger_format_1_t const* m0,  kpm_ric_event_trigger_format_1_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->report_period_ms != m1->report_period_ms)
    return false;

  return true;
}

kpm_ric_event_trigger_format_1_t cp_kpm_event_trigger_def_frm_1(const kpm_ric_event_trigger_format_1_t * src)
{
  assert(src != NULL);

  kpm_ric_event_trigger_format_1_t const dst = {.report_period_ms = src->report_period_ms};

  return dst;
}
