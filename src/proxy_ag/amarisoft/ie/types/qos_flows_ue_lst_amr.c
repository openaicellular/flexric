#include "qos_flows_ue_lst_amr.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

qos_flows_ue_lst_amr_t parse_qos_flows_ue_lst_amr(void* it)
{
  assert(it != NULL);

  qos_flows_ue_lst_amr_t dst = {0};

  dst.pdu_session_id = parse_int(it, "pdu_session_id") ;
  dst.dl_total_bytes = parse_int(it, "dl_total_bytes") ;
  dst.ul_total_bytes = parse_int(it, "ul_total_bytes") ;
  dst.sst = parse_int(it, "sst"); 
  dst.sd = parse_int(it, "sd") ;

  return dst;
}

