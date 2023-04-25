#include <assert.h>
#include <string.h>

#include "dec_ccc_ric_action_def_frm_2.h"
#include "../dec_ccc_common_json/dec_cell_global_id.h"
#include "../dec_ccc_common_json/dec_cell_level_ran_conf_struct_report_et_ad.h"

e2sm_ccc_action_def_frm_2_t dec_ccc_action_def_frm_2(const cJSON * act_def_json)
{
  assert(act_def_json != NULL);

  e2sm_ccc_action_def_frm_2_t act_def_frm_2 = {0};
  
  // Mandatory
  // List of Cell Configurations To Be Reported  -  [1..1024]
  const cJSON *cells_list_json = cJSON_GetObjectItem(act_def_json, "listOfCellConfigurationsToBeReportedForADF");
  assert(cJSON_IsArray(cells_list_json));

  act_def_frm_2.cell_level_conf_list_len = (size_t)cJSON_GetArraySize(cells_list_json);
  assert(act_def_frm_2.cell_level_conf_list_len >= 1 && act_def_frm_2.cell_level_conf_list_len <= 1024);

  act_def_frm_2.cell_level_conf_list_frm_2 = calloc(act_def_frm_2.cell_level_conf_list_len, sizeof(cell_configurations_list_t));
  assert(act_def_frm_2.cell_level_conf_list_frm_2 != NULL);

  const cJSON *cells_item_json;

  size_t i = 0;
  cJSON_ArrayForEach(cells_item_json, cells_list_json)
  {
    // Optional
    // Cell Global ID
    // 9.3.6
    const cJSON *cell_global_id_json = cJSON_GetObjectItem(cells_item_json, "cellGlobalId");
    if (cJSON_IsObject(cell_global_id_json))
    {
      act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id = calloc(1, sizeof(cell_global_id_t));
      assert(act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id != NULL);
      
      *act_def_frm_2.cell_level_conf_list_frm_2[i].cell_global_id = dec_ccc_cell_global_id_json(cell_global_id_json);
    }

    // Mandatory
    // List of Cell-level RAN Configuration Structures  -  [1..1024]
    const cJSON *cell_level_list_act_def = cJSON_GetObjectItem(cells_item_json, "listOfCellLevelRANConfigurationStructuresForADF");
    assert(cell_level_list_act_def != NULL);

    act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len = cJSON_GetArraySize(cell_level_list_act_def);
    assert(act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len >= 1 && act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len <= 1024);

    act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2 = calloc(act_def_frm_2.cell_level_conf_list_frm_2[i].node_level_conf_list_len, sizeof(cell_level_conf_list_act_def_frm_2_t));
    assert(act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2 != NULL);

    const cJSON *cell_level_item_act_def;
    
    size_t j = 0;
    cJSON_ArrayForEach(cell_level_item_act_def, cell_level_list_act_def)
    {
      // Mandatory
      // Report Type
      // 9.3.9
      const cJSON * report_type_json = cJSON_GetObjectItem(cell_level_item_act_def, "reportType");

      if (cJSON_IsString(report_type_json) && strcmp(report_type_json->valuestring, "all") == 0)
      {
        act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].report_type = ALL_REPORT_TYPE_CCC;
      }
      else if (cJSON_IsString(report_type_json) && strcmp(report_type_json->valuestring, "change") == 0)
      {
        act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].report_type = CHANGE_REPORT_TYPE_CCC;
      }
      else
      {
        assert("Unknown Report type");
      }

      // Mandatory
      // Cell-Level RAN Configuration Structures
      // 8.2.2
      act_def_frm_2.cell_level_conf_list_frm_2[i].cell_level_conf_list_frm_2[j].cell_level_ran_conf_struct = dec_ccc_cell_level_ran_conf_struct_report_et_ad(cell_level_item_act_def);

      ++j;
    }
    ++i;
  }


  return act_def_frm_2;
}
