#include <assert.h>
#include <stdio.h>

#include "dec_ccc_ric_event_trigger_frm_2.h"
#include "../dec_ccc_common_json/dec_cell_global_id.h"
#include "../dec_ccc_common_json/dec_cell_level_ran_conf_struct_report_et_ad.h"

e2sm_ccc_ev_trg_frm_2_t dec_ccc_event_trigger_frm_2(const cJSON * ev_trg_json)
{
  assert(ev_trg_json != NULL);

  e2sm_ccc_ev_trg_frm_2_t ev_trg_frm_2 = {0};

  // Mandatory
  // List of Cell-level Configuration Structures  -  [1..1024]
  const cJSON *cells_list_json = cJSON_GetObjectItem(ev_trg_json, "listOfCellLevelConfigurationStructuresForEventTrigger");
  assert(cJSON_IsArray(cells_list_json));

  ev_trg_frm_2.cell_level_conf_list_len = (size_t)cJSON_GetArraySize(cells_list_json);
  assert(ev_trg_frm_2.cell_level_conf_list_len >= 1 && ev_trg_frm_2.cell_level_conf_list_len <= 1024);

  ev_trg_frm_2.cell_level_conf_list = calloc(ev_trg_frm_2.cell_level_conf_list_len, sizeof(cell_level_conf_list_ev_trg_frm_2_t));
  assert(ev_trg_frm_2.cell_level_conf_list != NULL);

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
      ev_trg_frm_2.cell_level_conf_list[i].cell_global_id = calloc(1, sizeof(cell_global_id_t));
      assert(ev_trg_frm_2.cell_level_conf_list[i].cell_global_id != NULL);
      
      *ev_trg_frm_2.cell_level_conf_list[i].cell_global_id = dec_ccc_cell_global_id_json(cell_global_id_json);
    }

    // Mandatory
    // List of RAN Configuration Structures  -  [1..1024]
    const cJSON *cell_level_ran_conf_list_json = cJSON_GetObjectItem(cells_item_json, "listOfRANConfigurationStructuresForEventTrigger");
    assert(cJSON_IsArray(cell_level_ran_conf_list_json));

    ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len = (size_t)cJSON_GetArraySize(cell_level_ran_conf_list_json);
    assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len >= 1 && ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len <= 1024);

    ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list = calloc(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list_len, sizeof(cell_level_ran_conf_struct_report_et_ad_t));
    assert(ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list != NULL);

    const cJSON *ran_conf_item_json;

    size_t j = 0;
    cJSON_ArrayForEach(ran_conf_item_json, cell_level_ran_conf_list_json)
    {
      ev_trg_frm_2.cell_level_conf_list[i].cell_level_ran_conf_struct_list[j] = dec_ccc_cell_level_ran_conf_struct_report_et_ad(ran_conf_item_json);
      ++j;
    }

    ++i;
  }

  return ev_trg_frm_2;  
}
