#include "ncell_list_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

void free_ncell_list_amr(ncell_list_amr_t* src)
{
  assert(src != NULL);

  // Mandatory. "nr" or "eutra"
  free(src->rat);

  // Optional. 
  free(src->dl_earfcn);

  // Optional
  free(src->n_id_cell);
  
  // Optional
  if(src->ecgi != NULL){
    free_ecgi_amr(src->ecgi);
  }

  // Optional 
  free(src->ssb_nr_arfcn);

  // Optional
  free(src->n_id_nrcell);

  // Optional
  if(src->ncgi != NULL) 
    free_ncgi_amr(src->ncgi);

  // Mandatory
  //bool handover_target;

  // Mandatory
  //bool cell_redirect_target;

  // Optional. Only applicable to E-UTRA cells.
  free(src->eps_fallback_target);

  // Optional. Only applicable to E-UTRA cells.
  free(src->emergency_fallback_target);





}





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

