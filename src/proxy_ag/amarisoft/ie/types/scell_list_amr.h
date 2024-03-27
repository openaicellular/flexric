#ifndef SCELL_LIST_AMR_MIR_H
#define SCELL_LIST_AMR_MIR_H 

#include <stdbool.h>

typedef struct{

  // Mandatory. Cell identifier 
  int cell_id;

  // Mandatory. Indicates if PUSCH transmission is allowed
  bool ul_allowed;

} scell_list_amr_t;

scell_list_amr_t parse_scell_list_amr(void* it);

#endif

