#ifndef TX_CHANNELS_AMR_MIR_H
#define TX_CHANNELS_AMR_MIR_H

typedef struct{

  // Mandatory RF transmission gain, in dB.
  float gain; 

  // Mandatory RF transmission frequency, in MHz. 
  float freq;

  // Mandatory. RF port index.
  int port;

} tx_channels_amr_t;

void free_tx_channels_amr(tx_channels_amr_t* src );


#endif
