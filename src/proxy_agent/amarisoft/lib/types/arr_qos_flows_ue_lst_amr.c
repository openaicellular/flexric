#include "arr_qos_flows_ue_lst_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_arr_qos_flows_ue_lst(arr_qos_flows_ue_lst_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz; ++i){
    free_qos_flows_ue_lst_amr(&src->qos_flows[i]);
  }
  free(src->qos_flows);
}


