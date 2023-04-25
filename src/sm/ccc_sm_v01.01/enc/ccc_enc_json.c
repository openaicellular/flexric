#include <assert.h>
#include <stdio.h>
#include <cjson/cJSON.h>

#include "ccc_enc_json.h"
#include "enc_ccc_json/enc_ccc_ric_event_trigger_frm_1.h"
#include "enc_ccc_json/enc_ccc_ric_event_trigger_frm_2.h"
#include "enc_ccc_json/enc_ccc_ric_event_trigger_frm_3.h"

#include "enc_ccc_json/enc_ccc_ric_action_def_frm_1.h"
#include "enc_ccc_json/enc_ccc_ric_action_def_frm_2.h"


/* Encoding Event Trigger IE
 * Direction of the message: RIC --> E2 Node
 */
byte_array_t ccc_enc_event_trigger_json(e2sm_ccc_event_trigger_t const* event_trigger)
{
  assert(event_trigger != NULL);
  
  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);
    
  cJSON *event_trigger_json = cJSON_CreateObject();
  assert(event_trigger_json != NULL);

  cJSON *event_trigger_format_json = NULL;

  
  switch (event_trigger->type)
  {
  case FORMAT_1_EVENT_TRIGGER_CCC:
    event_trigger_format_json = enc_ccc_event_trigger_frm_1(&event_trigger->frm_1);
    break;

  case FORMAT_2_EVENT_TRIGGER_CCC:
    event_trigger_format_json = enc_ccc_event_trigger_frm_2(&event_trigger->frm_2);
    break;

  case FORMAT_3_EVENT_TRIGGER_CCC:
    event_trigger_format_json = enc_ccc_event_trigger_frm_3(&event_trigger->frm_3);
    break;
  
  default:
    assert("Non valid CCC RIC Event Trigger Format");
  }

  cJSON_AddItemToObject(event_trigger_json, "eventTriggerDefinitionFormat", event_trigger_format_json);
  cJSON_AddItemToObject(object, "RICEventTriggerDefinition", event_trigger_json);


  char *string_json = cJSON_Print(object);
  printf("%s\n", string_json);


  byte_array_t ba = {.len = strlen(string_json), .buf = (uint8_t *)string_json};

  cJSON_Delete(object);
  
  return ba;
}


/* Encoding Action Definition IE
 * Direction of the message: RIC --> E2 Node
 */
byte_array_t ccc_enc_action_def_json(e2sm_ccc_action_def_t const* act_def)
{
  assert(act_def != NULL);
  
  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);
    
  cJSON *act_def_json = cJSON_CreateObject();
  assert(act_def_json != NULL);

  cJSON *act_def_format_json = NULL;

  switch (act_def->report_style_type)
  {
  case STYLE_1_RIC_SERVICE_REPORT_CCC:
    assert(act_def->type == FORMAT_1_ACTION_DEFINITION_CCC);
    act_def_format_json = enc_ccc_action_def_frm_1(&act_def->frm_1);
    cJSON_AddItemToObject(act_def_json, "ricStyleType", cJSON_CreateNumber(1));
    break;

  case STYLE_2_RIC_SERVICE_REPORT_CCC:
    assert(act_def->type == FORMAT_2_ACTION_DEFINITION_CCC);
    act_def_format_json = enc_ccc_action_def_frm_2(&act_def->frm_2);
    cJSON_AddItemToObject(act_def_json, "ricStyleType", cJSON_CreateNumber(2));
    break;
  
  default:
    assert("Unknown CCC RIC Service Type");
  }

  cJSON_AddItemToObject(act_def_json, "actionDefinitionFormat", act_def_format_json);
  cJSON_AddItemToObject(object, "RICActionDefinition", act_def_json);


  char *string_json = cJSON_Print(object);
  printf("%s\n", string_json);


  byte_array_t ba = {.len = strlen(string_json), .buf = (uint8_t *)string_json};

  cJSON_Delete(object);
  
  return ba;
}
