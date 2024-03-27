#ifndef RX_CHANNELS_AMR_MIR_H
#define RX_CHANNELS_AMR_MIR_H


typedef struct{
  // Mandatory. RF reception gain, in dB
  float gain;

  // Mandatory. RF reception frequency, in MHz 
  float freq;

  // Mandatory? Optional? Bug? 2023-12-15
  int port;

} rx_channels_amr_t;

void free_rx_channels_amr( rx_channels_amr_t* src );

rx_channels_amr_t parse_rx_channels_amr(void* it);

#endif
