#include <assert.h>
#include <stdio.h>

#include "enc_cell_global_id.h"

cJSON * enc_ccc_cell_global_id_json(const cell_global_id_t * cell_global_id)
{
  cJSON *cell_global_id_json = cJSON_CreateObject();
  assert(cell_global_id_json != NULL);
      
  cJSON *plmn_id_json = cJSON_CreateObject();
  assert(plmn_id_json != NULL);
      
  char str[1024];
  
  switch (cell_global_id->type)
  {
  case NR_CGI_RAT_TYPE:
    sprintf(str, "%u", cell_global_id->nr_cgi.plmn_id.mcc);
    cJSON_AddItemToObject(plmn_id_json, "mcc", cJSON_CreateString(str));

    sprintf(str, "%u", cell_global_id->nr_cgi.plmn_id.mnc);
    cJSON_AddItemToObject(plmn_id_json, "mnc", cJSON_CreateString(str));

    cJSON_AddItemToObject(cell_global_id_json, "plmnIdentity", plmn_id_json);

    sprintf(str, "%lu", cell_global_id->nr_cgi.nr_cell_id);
    cJSON_AddItemToObject(cell_global_id_json, "nRCellIdentity", cJSON_CreateString(str));

    break;

  case EUTRA_CGI_RAT_TYPE:
    sprintf(str, "%u", cell_global_id->eutra.plmn_id.mcc);
    cJSON_AddItemToObject(plmn_id_json, "mcc", cJSON_CreateString(str));

    sprintf(str, "%u", cell_global_id->eutra.plmn_id.mnc);
    cJSON_AddItemToObject(plmn_id_json, "mnc", cJSON_CreateString(str));

    cJSON_AddItemToObject(cell_global_id_json, "plmnIdentity", plmn_id_json);

    sprintf(str, "%u", cell_global_id->eutra.eutra_cell_id);
    cJSON_AddItemToObject(cell_global_id_json, "eUTRACellIdentity", cJSON_CreateString(str));

    break;
      
  default:
    assert(false && "Unknown Cell Global ID Type");
  }

  return cell_global_id_json;
}
