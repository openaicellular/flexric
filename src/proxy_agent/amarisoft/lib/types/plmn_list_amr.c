#include "plmn_list_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void free_plmn_list_amr(plmn_list_amr_t* src)
{
  assert(src != NULL);


  // Mandatory.
  // Array of strings. List of PLMNs for this TAC
  for(size_t i = 0; i < src->sz_plmn_ids; ++i){
    free(src->plmn_ids[i]);
  }
  free(src->plmn_ids);

  // Mandatory. Reserved flag.
  // bool reserved;

  // Mandatory. Tracking Area Code
  // int tac;

  // Optional. RAN Area Code.
  free(src->ranac); 

  // Array of objects
  for(size_t i = 0; i < src->sz_nssai_amr; ++i){
    free_nssai_amr(&src->nssai[i]);
  }

  free(src->nssai);
}


