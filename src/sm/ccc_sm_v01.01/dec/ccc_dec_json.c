#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>

#include "ccc_dec_json.h"
#include "dec_ccc_json/dec_ccc_ric_event_trigger_frm_1.h"
#include "dec_ccc_json/dec_ccc_ric_event_trigger_frm_2.h"
#include "dec_ccc_json/dec_ccc_ric_event_trigger_frm_3.h"

#include "dec_ccc_json/dec_ccc_ric_action_def_frm_1.h"
#include "dec_ccc_json/dec_ccc_ric_action_def_frm_2.h"

/* Decoding Event Trigger IE
 * Direction of the message: RIC --> E2 Node
 */
e2sm_ccc_event_trigger_t ccc_dec_event_trigger_json(size_t len, const uint8_t ev_trg_json[len])
{
  assert(len > 0);
  assert(ev_trg_json != NULL);
  
  e2sm_ccc_event_trigger_t ev_trg = {0};

  char *string_json = (char *)ev_trg_json;
  assert(string_json != NULL);

  cJSON *object = cJSON_Parse(string_json);
  assert(object != NULL);
  const cJSON *message = cJSON_GetObjectItem(object, "RICEventTriggerDefinition");
  assert(message != NULL);
  const cJSON *format = cJSON_GetObjectItem(message, "eventTriggerDefinitionFormat");
  assert(format != NULL);

  const cJSON *format_name = NULL;

  if (cJSON_GetObjectItem(format, "E2SM-CCC-EventTriggerDefinition-Format1") != NULL)
  {
    format_name = cJSON_GetObjectItem(format, "E2SM-CCC-EventTriggerDefinition-Format1");
    ev_trg.type = FORMAT_1_EVENT_TRIGGER_CCC;
    ev_trg.frm_1 = dec_ccc_event_trigger_frm_1(format_name);
  }
  else if (cJSON_GetObjectItem(format, "E2SM-CCC-EventTriggerDefinition-Format2") != NULL)
  {
    format_name = cJSON_GetObjectItem(format, "E2SM-CCC-EventTriggerDefinition-Format2");
    ev_trg.type = FORMAT_2_EVENT_TRIGGER_CCC;
    ev_trg.frm_2 = dec_ccc_event_trigger_frm_2(format_name);
  }
  else if (cJSON_GetObjectItem(format, "E2SM-CCC-EventTriggerDefinition-Format3") != NULL)
  {
    format_name = cJSON_GetObjectItem(format, "E2SM-CCC-EventTriggerDefinition-Format3");
    ev_trg.type = FORMAT_3_EVENT_TRIGGER_CCC;
    ev_trg.frm_3 = dec_ccc_event_trigger_frm_3(format_name);
  }
  else
  {
    assert("Unknown CCC RIC Event Trigger Format Type");
  }
  
  cJSON_Delete(object);
  
  return ev_trg;
}


e2sm_ccc_action_def_t ccc_dec_action_def_json(size_t len, uint8_t const act_def_json[len])
{
  assert(len > 0);
  assert(act_def_json != NULL);
  
  e2sm_ccc_action_def_t act_def = {0};

  char *string_json = (char *)act_def_json;
  assert(string_json != NULL);

  cJSON *object = cJSON_Parse(string_json);
  assert(object != NULL);
  const cJSON *message = cJSON_GetObjectItem(object, "RICActionDefinition");
  assert(message != NULL);
  const cJSON *ric_style_type_json = cJSON_GetObjectItem(message, "ricStyleType");
  assert(ric_style_type_json != NULL);
  const cJSON *format = cJSON_GetObjectItem(message, "actionDefinitionFormat");
  assert(format != NULL);

  const cJSON *format_name = NULL;

  if (cJSON_GetObjectItem(format, "E2SM-CCC-ActionDefinitionFormat1") != NULL)
  {
    assert(ric_style_type_json->valueint == 1);
    act_def.report_style_type = STYLE_1_RIC_SERVICE_REPORT_CCC;
    act_def.type = FORMAT_1_ACTION_DEFINITION_CCC;
    format_name = cJSON_GetObjectItem(format, "E2SM-CCC-ActionDefinitionFormat1");
    act_def.frm_1 = dec_ccc_action_def_frm_1(format_name);
  }
  else if (cJSON_GetObjectItem(format, "E2SM-CCC-ActionDefinitionFormat2") != NULL)
  {
    assert(ric_style_type_json->valueint == 2);
    act_def.report_style_type = STYLE_2_RIC_SERVICE_REPORT_CCC;
    act_def.type = FORMAT_2_ACTION_DEFINITION_CCC;
    format_name = cJSON_GetObjectItem(format, "E2SM-CCC-ActionDefinitionFormat2");
    act_def.frm_2 = dec_ccc_action_def_frm_2(format_name);
  }
  else
  {
    assert("Unknown CCC RIC Action Definition Format Type");
  }

  cJSON_Delete(object);

  return act_def;
}
