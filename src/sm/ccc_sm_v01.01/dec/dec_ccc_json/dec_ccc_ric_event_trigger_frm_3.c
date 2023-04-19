#include <assert.h>

#include "dec_ccc_ric_event_trigger_frm_3.h"

e2sm_ccc_ev_trg_frm_3_t dec_ccc_event_trigger_frm_3(const cJSON * ev_trg_json)
{
  assert(ev_trg_json != NULL);

  e2sm_ccc_ev_trg_frm_3_t ev_trg_frm_3 = {0};

  const cJSON *period = cJSON_GetObjectItem(ev_trg_json, "period");
  assert(cJSON_IsNumber(period) && (period->valueint >= 10 && period->valueint <= 4294967295));

  ev_trg_frm_3.report_period = period->valueint;

  return ev_trg_frm_3;
}
