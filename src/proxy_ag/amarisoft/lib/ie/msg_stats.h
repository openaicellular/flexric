#ifndef MESSAGE_STATS_AMARISOFT_MIR_H
#define MESSAGE_STATS_AMARISOFT_MIR_H 

#include "../types/cpu_amr.h"
#include "../types/counter_msgs_amr.h"
#include "../types/arr_cell_amr.h"
#include "../types/arr_rf_ports_ms_amr.h"

typedef struct{
  // Mandatory in our version
  int msg_id;

  // Constant over process lifetime.
  char* instance_id;

  // Each member name defines a type and its value cpu load in % of one core.
  cpu_amr_t cpu;

  // Counters  
  cnt_msgs_amr_t counter;

  // Cells
  arr_cell_amr_t cells;

  // Optional bits/sec 
  float* gtp_tx_bitrate;

  // Optional bits/sec
  float* gtp_rx_bitrate;

  // Each member name is the RF port ID and each value is an object representing the TX-RX latency statistics 
  arr_rf_ports_ms_amr_t arr_rf_port; 

  // Set if samples has been set to true in request
  // samples

  // Mandatory. Time elapsed in seconds since the last call to the stats API
  float duration;

} msg_stats_amr_t;

void free_msg_stats_amr(msg_stats_amr_t* src);

#endif
