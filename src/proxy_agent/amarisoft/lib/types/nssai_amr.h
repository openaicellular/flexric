#ifndef NSSAI_AMR_MIR_H
#define NSSAI_AMR_MIR_H 

#include <stdint.h>

typedef struct{
  // Mandatory. Slice Srvice Type [1-255] 
  uint8_t sst;

  // Optional. Slice Differentiator [0-0xFFFFFE]
  uint32_t* sd;

} nssai_amr_t;

void free_nssai_amr(nssai_amr_t* src);


#endif
