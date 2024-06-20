#include "ncell_list_amr.h"
#include <assert.h>
#include <stdlib.h>

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
    free(src->ecgi);
  }

  // Optional 
  free(src->ssb_nr_arfcn);

  // Optional
  free(src->n_id_nrcell);

  // Optional
  if(src->ncgi != NULL){ 
    free_ncgi_amr(src->ncgi);
    free(src->ncgi);
  }

  // Mandatory
  //bool handover_target;

  // Mandatory
  //bool cell_redirect_target;

  // Optional. Only applicable to E-UTRA cells.
  free(src->eps_fallback_target);

  // Optional. Only applicable to E-UTRA cells.
  free(src->emergency_fallback_target);

}


