#ifndef NCELL_LIST_AMR_MIR_H
#define NCELL_LIST_AMR_MIR_H

#include <stdbool.h>
#include "ncgi_amr.h"
#include "ecgi_amr.h"

typedef struct{

  // Mandatory. "nr" or "eutra"
  char* rat;

  // Optional. 
  int* dl_earfcn;

  // Optional
  int* n_id_cell;
  
  // Optional
  ecgi_amr_t* ecgi;

  // Optional 
  int* ssb_nr_arfcn;

  // Optional
  int* n_id_nrcell;

  // Optional
  ncgi_amr_t* ncgi;

  // Mandatory
  bool handover_target;

  // Mandatory
  bool cell_redirect_target;

  // Optional. Only applicable to E-UTRA cells.
  bool* eps_fallback_target;

  // Optional. Only applicable to E-UTRA cells.
  bool* emergency_fallback_target;

} ncell_list_amr_t;

ncell_list_amr_t parse_ncell_list_amr(void* it );

#endif
