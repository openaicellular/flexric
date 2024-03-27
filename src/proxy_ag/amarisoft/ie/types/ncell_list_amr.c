#include "ncell_list_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

ncell_list_amr_t parse_ncell_list_amr(void* it )
{
  assert(it != NULL);
  ncell_list_amr_t dst = {0};

  // Mandatory. "nr" or "eutra"
  dst.rat = parse_string(it, "rat");

  // Optional. 
  //int* dl_earfcn;

  // Optional
  //int* n_id_cell;
  
  // Optional
  //ecgi_amr_t* ecgi;

  // Optional 
  //int* ssb_nr_arfcn;

  // Optional
  //int* n_id_nrcell;

  // Optional
  //ncgi_amr_t* ncgi;

  // Mandatory
  dst.handover_target = parse_bool(it, "handover_target");

  // Mandatory
  dst.cell_redirect_target = parse_bool(it, "cell_redirect_target");

  // Optional. Only applicable to E-UTRA cells.
  // bool* eps_fallback_target;

  // Optional. Only applicable to E-UTRA cells.
  // bool* emergency_fallback_target;

  return dst;
}

