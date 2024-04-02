#ifndef LAYERS_AMR_MIR_H
#define LAYERS_AMR_MIR_H 

#include <stdbool.h>

typedef struct{

  // Mandatory layer name e.g., PHY
  char* layer_name; 
  // Mandatory. Level
  char* level;
  // Mandatory. Max_size 
  int max_size;
  // Mandatory. key
  bool key;
  // Mandatory. crypto
  bool crypto;
  // Mandatory. Payload
  bool payload;
  // Optional. rep
  //
  // Optional. csi
  //
  // Optional. ntn
  //
  // Optional. signal

} layers_amr_t;

void free_layers_amr(layers_amr_t* src);

layers_amr_t parse_layers_amr(void* it);

#endif
