#include "dec_config_get.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "util/alg_ds/alg/defer.h"
#include "parse_cjson.h"


void dec_config_get_amr(const char* in, config_get_amr_t* out)
{
  assert(in != NULL);
  assert(out != NULL);

  cJSON *json = cJSON_Parse(in);
  defer({ cJSON_Delete(json); });
  if (json == NULL) {
    const char *error_ptr = cJSON_GetErrorPtr();
    assert(error_ptr != NULL);
    fprintf(stderr, "Error before: %s\n", error_ptr);
  }
  assert(json != NULL && "Error parsing the input json");

  // Message ID. Mandatory 
  out->msg_id = parse_int(json, "message_id"); 

  // Version. Mandatory
  out->version = parse_string(json, "version"); 

  // Type. Mandatory
  out->type = parse_string(json, "type"); 

  // Name. Mandatory
  out->name = parse_string(json, "name"); 

  // License ID. Mandatory
  out->license_id = parse_string(json, "license_id"); 

  // License User. Mandatory
  out->license_user = parse_string(json, "license_user"); 

  // Log. Mandatory
  out->logs = parse_logs_amr(json); 

  // Optional
  // bool* locked;
 
  // TAI. Mandatory
  out->tai = parse_float(json, "tai");  

  // 4G Optional
  // global_enb_id_amr_t* global_enb_id; 
  
  // 5G Optional. We make it mandatory as we only support 5G
  
  ans_cjson_t ans = find_object(json, "global_gnb_id");
  out->global_gnb_id = parse_global_gnb_id_amr((void*)ans.it);

  // 4G mandatory 
  // cells
  
  // 4G NB-IoT 
  // nb_cells 
 
  // 5G nr_cells 
  ans = find_object(json, "nr_cells");
  out->arr_nr_cells = parse_arr_nr_cells_amr((void*)ans.it);

  // Mandatory
  out->arr_rx_channels = parse_arr_rx_channels_amr(json); 
 
  // Mandatory
  out->arr_tx_channels = parse_arr_tx_channels_amr(json); 

  // Mandatory
  out->arr_rf_ports = parse_arr_rf_ports_amr(json);  

  // Mandatory to all messages?
  
  out->utc = parse_float(json, "utc");  
}


