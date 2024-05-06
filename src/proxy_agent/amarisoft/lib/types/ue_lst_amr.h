#ifndef UE_LIST_AMARISOFT_MIR_H
#define UE_LIST_AMARISOFT_MIR_H 

#include "arr_qos_flows_ue_lst_amr.h"
#include "arr_cells_ue_lst_amr.h"


typedef struct{

  // Optional. Present for LTE or NB-IoT UEs.
  // int enb_ue_id

  // Mandatory for 5G. RAN UE id. Present for NR UEs.
  int ran_ue_id;

  // Optional integer. AMF UE id. It is present when the UE-
  // associated logical NG-connection is setup.
  int amf_ue_id;

  // Mandatory
  int rnti; 

  // Mandatory
  // Array of Objects
  arr_cells_ue_lst_amr_t arr_cells;

  // Mandatory. 
  // Array of objects
  arr_qos_flows_ue_lst_t qos_flows;

} ue_lst_amr_t;

void free_ue_lst_amr(ue_lst_amr_t* src);


#endif

