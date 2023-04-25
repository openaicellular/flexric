#include <assert.h>
#include <string.h>

#include "dec_ccc_ric_action_def_frm_1.h"
#include "../dec_ccc_common_json/dec_node_level_ran_conf_struct_report_et_ad.h"

e2sm_ccc_action_def_frm_1_t dec_ccc_action_def_frm_1(const cJSON * act_def_json)
{
  assert(act_def_json != NULL);

  e2sm_ccc_action_def_frm_1_t act_def_frm_1 = {0};

  const cJSON *node_level_conf_list_json = cJSON_GetObjectItem(act_def_json, "listOfNodeLevelRANConfigurationStructuresForADF");
  assert(cJSON_IsArray(node_level_conf_list_json));

  // Mandatory
  // List of Node-level Configuration Structures  -  [1..256]
  act_def_frm_1.node_level_conf_list_len = (size_t)cJSON_GetArraySize(node_level_conf_list_json);
  assert(act_def_frm_1.node_level_conf_list_len >= 1 && act_def_frm_1.node_level_conf_list_len <= 256);

  act_def_frm_1.node_level_conf_list_frm_1 = calloc(act_def_frm_1.node_level_conf_list_len, sizeof(node_level_conf_list_act_def_frm_1_t));
  assert(act_def_frm_1.node_level_conf_list_frm_1 != NULL);

  const cJSON *node_level_conf_item_json;

  size_t i = 0;
  cJSON_ArrayForEach(node_level_conf_item_json, node_level_conf_list_json)
  {
    // Mandatory
    // Report Type
    // 9.3.9
    const cJSON * report_type_json = cJSON_GetObjectItem(node_level_conf_item_json, "reportType");

    if (cJSON_IsString(report_type_json) && strcmp(report_type_json->valuestring, "all") == 0)
    {
      act_def_frm_1.node_level_conf_list_frm_1[i].report_type = ALL_REPORT_TYPE_CCC;
    }
    else if (cJSON_IsString(report_type_json) && strcmp(report_type_json->valuestring, "change") == 0)
    {
      act_def_frm_1.node_level_conf_list_frm_1[i].report_type = CHANGE_REPORT_TYPE_CCC;
    }
    else
    {
      assert("Unknown Report type");
    }

    // Mandatory
    // Node-Level RAN Configuration Structures
    // 8.2.1
    act_def_frm_1.node_level_conf_list_frm_1[i].node_level_ran_conf_struct = dec_ccc_node_level_ran_conf_struct_report_et_ad(node_level_conf_item_json);


    ++i;
  }


  return act_def_frm_1;
}
