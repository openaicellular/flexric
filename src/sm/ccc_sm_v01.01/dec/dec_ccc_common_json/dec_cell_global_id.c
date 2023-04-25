#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "dec_cell_global_id.h"

cell_global_id_t dec_ccc_cell_global_id_json(const cJSON * cell_global_id_json)
{
  assert(cell_global_id_json != NULL);

  cell_global_id_t cell_global_id = {0};

  const cJSON *plmn_id_json = cJSON_GetObjectItem(cell_global_id_json, "plmnIdentity");
  const cJSON *mcc_json = cJSON_GetObjectItem(plmn_id_json, "mcc");
  const cJSON *mnc_json = cJSON_GetObjectItem(plmn_id_json, "mnc");
  
  const cJSON *nr_cell_id_json = cJSON_GetObjectItem(cell_global_id_json, "nRCellIdentity");
  if (cJSON_IsString(nr_cell_id_json))
  {
    cell_global_id.type = NR_CGI_RAT_TYPE;
    sscanf(nr_cell_id_json->valuestring, "%lu", &cell_global_id.nr_cgi.nr_cell_id);

    sscanf(mcc_json->valuestring, "%hu", &cell_global_id.nr_cgi.plmn_id.mcc);
    sscanf(mnc_json->valuestring, "%hu", &cell_global_id.nr_cgi.plmn_id.mnc);
        
    cell_global_id.nr_cgi.plmn_id.mnc_digit_len = strlen(mnc_json->valuestring);
  }
      
  const cJSON *eutra_cell_id_json = cJSON_GetObjectItem(cell_global_id_json, "eUTRACellIdentity");
  if (cJSON_IsString(eutra_cell_id_json))
  {
    cell_global_id.type = EUTRA_CGI_RAT_TYPE;
        
    sscanf(eutra_cell_id_json->valuestring, "%u", &cell_global_id.eutra.eutra_cell_id);

    sscanf(mcc_json->valuestring, "%hu", &cell_global_id.eutra.plmn_id.mcc);
    sscanf(mnc_json->valuestring, "%hu", &cell_global_id.eutra.plmn_id.mnc);

    cell_global_id.eutra.plmn_id.mnc_digit_len = strlen(mnc_json->valuestring);
  }


  return cell_global_id;
}
