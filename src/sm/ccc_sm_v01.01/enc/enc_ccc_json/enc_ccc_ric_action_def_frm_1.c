#include <assert.h>
#include <cjson/cJSON_Utils.h>

#include "enc_ccc_ric_action_def_frm_1.h"
#include "../enc_ccc_common_json/enc_node_level_ran_conf_struct_report_et_ad.h"

cJSON * enc_ccc_action_def_frm_1(const e2sm_ccc_action_def_frm_1_t * act_def_frm_1)
{
  assert(act_def_frm_1 != NULL);

  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);

  cJSON *action_def_name = cJSON_CreateObject();
  assert(action_def_name != NULL);

  cJSON *node_level_conf_list_json = cJSON_CreateArray();
  assert(node_level_conf_list_json != NULL);


  // Mandatory
  // List of Node-level Configuration Structures - [1..256]
  assert(act_def_frm_1->node_level_conf_list_len >= 1 && act_def_frm_1->node_level_conf_list_len <= 256);
  
  for (size_t i = 0; i<act_def_frm_1->node_level_conf_list_len; i++)
  {
    cJSON *node_level_item = cJSON_CreateObject();
    assert(node_level_item != NULL);

    // Mandatory
    // Report Type
    // 9.3.9
    switch (act_def_frm_1->node_level_conf_list_frm_1[i].report_type)
    {
    case ALL_REPORT_TYPE_CCC:
        cJSON_AddItemToObject(node_level_item, "reportType", cJSON_CreateString("all"));
        break;

    case CHANGE_REPORT_TYPE_CCC:
        cJSON_AddItemToObject(node_level_item, "reportType", cJSON_CreateString("change"));
        break;
    
    default:
        assert("Unknown CCC Report Type");
    }


    // Mandatory
    // Node-Level RAN Configuration Structures
    // 8.2.1
    cJSON *node_level_conf_item_json = enc_ccc_node_level_ran_conf_struct_report_et_ad(&act_def_frm_1->node_level_conf_list_frm_1[i].node_level_ran_conf_struct);


    cJSONUtils_MergePatch(node_level_item, node_level_conf_item_json);
    cJSON_Delete(node_level_conf_item_json);
    cJSON_AddItemToArray(node_level_conf_list_json, node_level_item);
  }


  cJSON_AddItemToObject(action_def_name, "listOfNodeLevelRANConfigurationStructuresForADF", node_level_conf_list_json);
  cJSON_AddItemToObject(object, "E2SM-CCC-ActionDefinitionFormat1", action_def_name);


  return object;
}
