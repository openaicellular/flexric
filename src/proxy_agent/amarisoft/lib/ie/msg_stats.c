#include "msg_stats.h"

#include <assert.h>

void free_msg_stats_amr(msg_stats_amr_t* src)
{
  assert(src != NULL);
  // Mandatory in our version
  // int msg_id;

  // Constant over process lifetime.
  free(src->instance_id);

  // Each member name defines a type and its value cpu load in % of one core.
  free_cpu_amr(&src->cpu);

  // Counters  
  free_cnt_msgs_amr(&src->counter);

  // Cells
  free_arr_cell_amr(&src->cells);

  // Optional bits/sec 
  free(src->gtp_tx_bitrate); 

  // Optional bits/sec
  free(src->gtp_rx_bitrate);

  // Each member name is the RF port ID and each value is an object representing the TX-RX latency statistics 
  free_arr_rf_ports_ms_amr(&src->arr_rf_port); 

  // Set if samples has been set to true in request
  // samples

  // Mandatory. Time elapsed in seconds since the last call to the stats API
  // float duration;
}

