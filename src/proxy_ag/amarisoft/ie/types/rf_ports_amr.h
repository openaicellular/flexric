#ifndef RF_PORTS_AMR_MIR_H
#define RF_PORTS_AMR_MIR_H

#include "channel_dl_amr.h"

typedef struct {

  channel_dl_amr_t channel_dl;

  // Mandatory. Uplink frequency shift in Hz 
  float ul_freq_shift;

  // Mandatory. Sample rate in Hz
  float sample_rate;

} rf_ports_amr_t;

void free_rf_ports_amr(rf_ports_amr_t* src);

rf_ports_amr_t parse_rf_ports_amr(void* it);





#endif
