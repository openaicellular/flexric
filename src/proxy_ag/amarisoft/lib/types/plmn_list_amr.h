#ifndef PLMN_LIST_AMR_MIR_H
#define PLMN_LIST_AMR_MIR_H 

#include "nssai_amr.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct{
  // Mandatory.
  // Array of strings. List of PLMNs for this TAC
  char** plmn_ids;
  size_t sz_plmn_ids;

  // Mandatory. Reserved flag.
  bool reserved;

  // Mandatory. Tracking Area Code
  int tac;

  // Optional. RAN Area Code.
  int* ranac;

  // Array of objects
  nssai_amr_t* nssai;
  size_t sz_nssai_amr;

} plmn_list_amr_t;

void free_plmn_list_amr( plmn_list_amr_t* src);


#endif
