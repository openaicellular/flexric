#include "layers_amr.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_layers_amr(layers_amr_t* src)
{
  assert(src != NULL);

  // Mandatory layer name e.g., PHY
  free(src->layer_name); 
  // Mandatory. Level
  free(src->level);
  // Mandatory. Max_size 
  //int max_size;
  // Mandatory. key
  //bool key;
  // Mandatory. crypto
  //bool crypto;
  // Mandatory. Payload
  //bool payload;
  // Optional. rep
  //
  // Optional. csi
  //
  // Optional. ntn
  //
  // Optional. signal
}


