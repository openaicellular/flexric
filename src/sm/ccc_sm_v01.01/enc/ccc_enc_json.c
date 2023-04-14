#include <assert.h>
#include <stdio.h>
#include <cjson/cJSON.h>

#include "ccc_enc_json.h"
#include "enc_ccc_json/enc_ccc_ric_event_trigger_frm_1.h"


/* Encoding Event Trigger IE
 * Direction of the message: RIC --> E2 Node
 */
byte_array_t ccc_enc_event_trigger_json(e2sm_ccc_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);

  char *string_json = NULL;
  cJSON *event_trigger_json = cJSON_CreateObject();
  cJSON *event_trigger_format_json = NULL;

  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);


  switch (event_trigger->type)
  {
  case FORMAT_1_EVENT_TRIGGER_CCC:
    event_trigger_format_json = enc_ccc_event_trigger_frm_1(&event_trigger->frm_1);
    break;

//   case FORMAT_2_EVENT_TRIGGER_CCC:
//     event_trigger_format_json = enc_ccc_event_trigger_frm_2(&event_trigger->frm_2);
//     break;

//   case FORMAT_3_EVENT_TRIGGER_CCC:
//     event_trigger_format_json = enc_ccc_event_trigger_frm_3(&event_trigger->frm_3);
//     break;
  
  default:
    assert("Non valid CCC RIC Event Trigger Format");
  }

  cJSON_AddItemToObject(event_trigger_json, "eventTriggerDefinitionFormat", event_trigger_format_json);
  cJSON_AddItemToObject(object, "RICEventTriggerDefinition", event_trigger_json);

  string_json = cJSON_Print(object);
  printf("%s\n", string_json);


  byte_array_t ba = {.len = strlen(string_json)};
  ba.buf = calloc(ba.len + 1, sizeof(char));
  assert(ba.buf != NULL);
  memcpy(ba.buf, string_json, ba.len);


  return ba;
}
