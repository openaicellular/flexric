#ifndef ARR_QOS_FLOWS_UE_LST_MIR_H
#define ARR_QOS_FLOWS_UE_LST_MIR_H 

#include <stdlib.h>

#include "qos_flows_ue_lst_amr.h" 

typedef struct{
  size_t sz;
  qos_flows_ue_lst_amr_t* qos_flows;
} arr_qos_flows_ue_lst_t;

void free_arr_qos_flows_ue_lst(arr_qos_flows_ue_lst_t* src);

arr_qos_flows_ue_lst_t parse_arr_qos_flows_ue_lst(void* it);


#endif
