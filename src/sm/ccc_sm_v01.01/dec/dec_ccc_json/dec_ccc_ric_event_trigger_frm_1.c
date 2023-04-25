#include <assert.h>

#include "dec_ccc_ric_event_trigger_frm_1.h"
#include "../dec_ccc_common_json/dec_node_level_ran_conf_struct_report_et_ad.h"

e2sm_ccc_ev_trg_frm_1_t dec_ccc_event_trigger_frm_1(const cJSON * ev_trg_json)
{
  assert(ev_trg_json != NULL);

  e2sm_ccc_ev_trg_frm_1_t ev_trg_frm_1 = {0};

  const cJSON *node_level_conf_list_json = cJSON_GetObjectItem(ev_trg_json, "listOfNodeLevelConfigurationStructuresForEventTrigger");
  assert(cJSON_IsArray(node_level_conf_list_json));
  
  // Mandatory
  // List of Node-level Configuration Structures  -  [1..256]
  ev_trg_frm_1.node_level_conf_list_len = (size_t)cJSON_GetArraySize(node_level_conf_list_json);
  assert(ev_trg_frm_1.node_level_conf_list_len >= 1 && ev_trg_frm_1.node_level_conf_list_len <= 256);

  ev_trg_frm_1.node_level_ran_conf_struct_list = calloc(ev_trg_frm_1.node_level_conf_list_len, sizeof(node_level_ran_conf_struct_report_et_ad_t));
  assert(ev_trg_frm_1.node_level_ran_conf_struct_list != NULL);

  const cJSON *node_level_conf_item_json;

  size_t i = 0;
  cJSON_ArrayForEach(node_level_conf_item_json, node_level_conf_list_json)
  {
    ev_trg_frm_1.node_level_ran_conf_struct_list[i] = dec_ccc_node_level_ran_conf_struct_report_et_ad(node_level_conf_item_json);
    ++i;
  }


  return ev_trg_frm_1;
}
