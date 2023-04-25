#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "enc_ccc_ric_event_trigger_frm_2.h"
#include "../enc_ccc_common_json/enc_cell_global_id.h"
#include "../enc_ccc_common_json/enc_cell_level_ran_conf_struct_report_et_ad.h"

cJSON * enc_ccc_event_trigger_frm_2(const e2sm_ccc_ev_trg_frm_2_t * ev_trg)
{
  assert(ev_trg != NULL);
  

  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);

  cJSON *event_trigger_name = cJSON_CreateObject();
  assert(event_trigger_name != NULL);

  cJSON *cells_list_json = cJSON_CreateArray();
  assert(cells_list_json != NULL);
    
  // Mandatory
  // List of Cell-level Configuration Structures  -  [1..1024]
  assert(ev_trg->cell_level_conf_list_len >= 1 && ev_trg->cell_level_conf_list_len <= 1024);

  for (size_t i = 0; i<ev_trg->cell_level_conf_list_len; i++)
  {
    cJSON *cells_item_json = cJSON_CreateObject();
    assert(cells_item_json != NULL);

    // Optional
    // Cell Global ID
    // 9.3.6
    if (ev_trg->cell_level_conf_list[i].cell_global_id != NULL)
    {
      cJSON *cell_global_id_json = enc_ccc_cell_global_id_json(ev_trg->cell_level_conf_list[i].cell_global_id);
      cJSON_AddItemToObject(cells_item_json, "cellGlobalId", cell_global_id_json);
    }
    
    // Mandatory
    // List of RAN Configuration Structures  -  [1..1024]
    assert(ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len >=1 && ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len <= 1024);

    cJSON *cell_level_ran_conf_list_json = cJSON_CreateArray();
    assert(cell_level_ran_conf_list_json != NULL);

    for (size_t j = 0; j<ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len; j++)
    {
      cJSON *cell_level_conf_item_json = enc_ccc_cell_level_ran_conf_struct_report_et_ad(&ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j]);
      cJSON_AddItemToArray(cell_level_ran_conf_list_json, cell_level_conf_item_json);
    }
    cJSON_AddItemToObject(cells_item_json, "listOfRANConfigurationStructuresForEventTrigger", cell_level_ran_conf_list_json);

    cJSON_AddItemToArray(cells_list_json, cells_item_json);
  }

  cJSON_AddItemToObject(event_trigger_name, "listOfCellLevelConfigurationStructuresForEventTrigger", cells_list_json);
  cJSON_AddItemToObject(object, "E2SM-CCC-EventTriggerDefinition-Format2", event_trigger_name);

  return object;
}
