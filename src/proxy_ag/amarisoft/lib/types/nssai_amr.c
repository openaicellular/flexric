#include "nssai_amr.h"

#include <assert.h>
#include <stdlib.h>

void free_nssai_amr(nssai_amr_t* src)
{
  assert(src != NULL);
  // Mandatory. Slice Srvice Type [1-255] 
  // uint8_t sst;

  // Optional. Slice Differentiator [0-0xFFFFFE]
  free(src->sd);
}


