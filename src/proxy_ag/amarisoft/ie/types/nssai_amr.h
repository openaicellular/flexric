#ifndef NSSAI_AMR_MIR_H
#define NSSAI_AMR_MIR_H 

#include <stdint.h>

typedef struct{
  // Mandatory. Slice Srvice Type [1-255] 
  uint8_t sst;

  // Optional. Slice Differentiator [0-0xFFFFFE]
  uint32_t* sd;

} nssai_amr_t;

nssai_amr_t parse_nssai_amr(void* it);

#endif
