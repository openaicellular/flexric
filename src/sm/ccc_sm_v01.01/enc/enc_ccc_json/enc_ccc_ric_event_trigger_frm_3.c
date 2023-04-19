#include <assert.h>

#include "enc_ccc_ric_event_trigger_frm_3.h"

cJSON * enc_ccc_event_trigger_frm_3(const e2sm_ccc_ev_trg_frm_3_t * ev_trg)
{
  assert(ev_trg != NULL);
  
  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);

  cJSON *event_trigger_name = cJSON_CreateObject();
  assert(event_trigger_name != NULL);

  cJSON *period = cJSON_CreateNumber(ev_trg->report_period);

  cJSON_AddItemToObject(event_trigger_name, "period", period);
  cJSON_AddItemToObject(object, "E2SM-CCC-EventTriggerDefinition-Format3", event_trigger_name);


  return object;
}
