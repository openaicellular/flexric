#include "ue_lst_amr.h"
#include <assert.h>
#include <stdlib.h>

void free_ue_lst_amr(ue_lst_amr_t* src)
{
  assert(src != NULL);

  // Optional. Present for LTE or NB-IoT UEs.
  // int enb_ue_id

  // Mandatory for 5G. RAN UE id. Present for NR UEs.
  // int ran_ue_id;

  // Optional integer. AMF UE id. It is present when the UE-
  // associated logical NG-connection is setup.
  // int amf_ue_id;

  // Mandatory
  // int rnti; 

  // Mandatory
  // Array of Objects
  free_arr_cells_ue_lst_amr(&src->arr_cells);

  // Mandatory. 
  // Array of objects
  free_arr_qos_flows_ue_lst(&src->qos_flows);
}


