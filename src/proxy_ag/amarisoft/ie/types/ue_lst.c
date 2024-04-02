#include "ue_lst.h"
#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

ue_lst_t parse_ue_lst(void* it)
{
  assert(it != NULL);

  ue_lst_t dst = {0}; 

  // Mandatory for 5G. RAN UE id. Present for NR UEs.
  dst.ran_ue_id = parse_int(it,"ran_ue_id");

  // Optional integer. AMF UE id. It is present when the UE-
  // associated logical NG-connection is setup.
  dst.amf_ue_id = parse_int(it,"amf_ue_id");

  // Mandatory
  dst.rnti = parse_int(it,"rnti"); 

  // Mandatory
  // Array of Objects
  ans_cjson_t ans = find_object(it, "cells");
  dst.arr_cells = parse_arr_cells_ue_lst_amr((void*)ans.it);

  // Mandatory. 
  // Array of objects
  ans = find_object(it, "qos_flow_list");
  dst.qos_flows = parse_arr_qos_flows_ue_lst((void*)ans.it);

  return dst;
}

