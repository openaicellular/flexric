#ifndef CONNECTED_MOBILITY_AMR_MIR_H
#define CONNECTED_MOBILITY_AMR_MIR_H 

#include <stdbool.h>

typedef struct{

  //Mandatory. 
  bool scell_config;

  //Mandatory. 
  bool nr_handover;

  //Mandatory. 
  bool nr_cell_redirect;

  //Mandatory. 
  bool eutra_handover;

  //Mandatory. 
  bool eutra_cell_redirect;

  //Mandatory. 
  bool nr_dc_setup;

  //Mandatory. 
  bool mr_dc_release;

} connected_mobility_amr_t;

void free_connected_mobility_amr(connected_mobility_amr_t* src);

connected_mobility_amr_t parse_connected_mobility_amr(void* it);

#endif

