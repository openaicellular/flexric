#include <assert.h>
#include <cjson/cJSON.h>
#include <stdio.h>

#include "enc_ccc_ric_event_trigger_frm_1.h"
#include "../enc_ccc_common_json/enc_node_level_ran_conf_struct_report_et_ad.h"

cJSON * enc_ccc_event_trigger_frm_1(const e2sm_ccc_ev_trg_frm_1_t * ev_trg)
{
  assert(ev_trg != NULL);

  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);

  cJSON *event_trigger_name = cJSON_CreateObject();
  assert(event_trigger_name != NULL);

  cJSON *node_level_conf_list_json = cJSON_CreateArray();
  assert(node_level_conf_list_json != NULL);

  

  // Mandatory
  // List of Node-level Configuration Structures - [1..256]
  assert(ev_trg->node_level_conf_list_len >= 1 && ev_trg->node_level_conf_list_len <= 256);
  
  for (size_t i = 0; i<ev_trg->node_level_conf_list_len; i++)
  {
    cJSON *node_level_conf_item_json = enc_ccc_node_level_ran_conf_struct_report_et_ad(&ev_trg->node_level_ran_conf_struct_list[i]);
    cJSON_AddItemToArray(node_level_conf_list_json, node_level_conf_item_json);
  }


  cJSON_AddItemToObject(event_trigger_name, "listOfNodeLevelConfigurationStructuresForEventTrigger", node_level_conf_list_json);
  cJSON_AddItemToObject(object, "E2SM-CCC-EventTriggerDefinition-Format1", event_trigger_name);


  return object;
}
