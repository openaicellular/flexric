#include "dec_msg_stats.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "util/alg_ds/alg/defer.h"
#include "parse_cjson.h"

void dec_msg_stats_amr(const char* in, msg_stats_amr_t* out)
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

  // Mandatory. Constant over process lifetime.
  out->instance_id = parse_string(json, "instance_id"); 

  // Each member name defines a type and its value cpu load in % of one core.
  out->cpu = parse_cpu_amr(json); 

  // Counters  
  out->counter = parse_cnt_msgs_amr(json); 

  // Cells
  out->cells = parse_arr_cell_amr(json); 

  // Optional bits/sec 
  ans_cjson_t ans = find_object(json, "gtp_tx_bitrate");  
  if(ans.it != NULL){
    out->gtp_tx_bitrate = calloc(1, sizeof(float));
    assert(out->gtp_tx_bitrate != NULL && "Memory exhausted" );
    *out->gtp_tx_bitrate = ans.it->valuedouble;
  }

  // Optional bits/sec
  ans = find_object(json, "gtp_rx_bitrate");  
  if(ans.it != NULL){
    out->gtp_rx_bitrate = calloc(1, sizeof(float));
    assert(out->gtp_rx_bitrate != NULL && "Memory exhausted" );
    *out->gtp_rx_bitrate = ans.it->valuedouble;
  }

  // Each member name is the RF port ID and each value is an object representing the TX-RX latency statistics 
  //arr_rf_ports_ms_amr_t rf_port; 
  out->arr_rf_port = parse_arr_rf_ports_ms_amr(json);

  // Set if samples has been set to true in request
  // samples

  // Mandatory. Time elapsed in seconds since the last call to the stats API
  out->duration = parse_float(json, "duration"); 


  assert(0 != 0 && "Debugging point");

}

