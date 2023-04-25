#include <assert.h>
#include <cjson/cJSON_Utils.h>

#include "enc_ccc_ric_action_def_frm_2.h"
#include "../enc_ccc_common_json/enc_cell_global_id.h"
#include "../enc_ccc_common_json/enc_cell_level_ran_conf_struct_report_et_ad.h"

cJSON * enc_ccc_action_def_frm_2(const e2sm_ccc_action_def_frm_2_t * act_def_frm_2)
{
  assert(act_def_frm_2 != NULL);
  
  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);

  cJSON *act_def_name = cJSON_CreateObject();
  assert(act_def_name != NULL);

  cJSON *cells_list_json = cJSON_CreateArray();
  assert(cells_list_json != NULL);

  // Mandatory
  // List of Cell Configurations To Be Reported  -  [1..1024]
  assert(act_def_frm_2->cell_level_conf_list_len >= 1 && act_def_frm_2->cell_level_conf_list_len <= 1024);

  for (size_t i = 0; i<act_def_frm_2->cell_level_conf_list_len; i++)
  {
    cJSON *cells_item_json = cJSON_CreateObject();
    assert(cells_item_json != NULL);

    // Optional
    // Cell Global ID
    // 9.3.6
    if (act_def_frm_2->cell_level_conf_list_frm_2[i].cell_global_id != NULL)
    {
      cJSON *cell_global_id_json = enc_ccc_cell_global_id_json(act_def_frm_2->cell_level_conf_list_frm_2[i].cell_global_id);
      cJSON_AddItemToObject(cells_item_json, "cellGlobalId", cell_global_id_json);
    }

    // Mandatory
    // List of Cell-level RAN Configuration Structures  -  [1..1024]
    assert(act_def_frm_2->cell_level_conf_list_frm_2[i].node_level_conf_list_len >= 1 && act_def_frm_2->cell_level_conf_list_frm_2[i].node_level_conf_list_len <= 1024);

    cJSON *cell_act_def_frm_2_list = cJSON_CreateArray();
    assert(cell_act_def_frm_2_list != NULL);

    for (size_t j = 0; j<act_def_frm_2->cell_level_conf_list_frm_2[i].node_level_conf_list_len; j++)
    {
      cJSON *cell_level_item = cJSON_CreateObject();
      assert(cell_level_item != NULL);

      // Mandatory
      // Report Type
      // 9.3.9
      switch (act_def_frm_2->cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].report_type)
      {
      case ALL_REPORT_TYPE_CCC:
        cJSON_AddItemToObject(cell_level_item, "reportType", cJSON_CreateString("all"));
        break;

      case CHANGE_REPORT_TYPE_CCC:
        cJSON_AddItemToObject(cell_level_item, "reportType", cJSON_CreateString("change"));
        break;
    
      default:
        assert("Unknown CCC Report Type");
      }

      // Mandatory
      // Cell-Level RAN Configuration Structures
      // 8.2.2
      cJSON *cell_level_conf_item_json = enc_ccc_cell_level_ran_conf_struct_report_et_ad(&act_def_frm_2->cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].cell_level_ran_conf_struct);

      cJSONUtils_MergePatch(cell_level_item, cell_level_conf_item_json);
      cJSON_Delete(cell_level_conf_item_json);
      cJSON_AddItemToArray(cell_act_def_frm_2_list, cell_level_item);
    }
    cJSON_AddItemToObject(cells_item_json, "listOfCellLevelRANConfigurationStructuresForADF", cell_act_def_frm_2_list);
    cJSON_AddItemToArray(cells_list_json, cells_item_json);

  }

  cJSON_AddItemToObject(act_def_name, "listOfCellConfigurationsToBeReportedForADF", cells_list_json);
  cJSON_AddItemToObject(object, "E2SM-CCC-ActionDefinitionFormat2", act_def_name);

  return object;
}
