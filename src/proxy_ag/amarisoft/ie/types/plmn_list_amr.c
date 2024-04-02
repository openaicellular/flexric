#include "plmn_list_amr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

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

plmn_list_amr_t parse_plmn_list_amr(void* it)
{
  assert(it != NULL);

  plmn_list_amr_t dst = {0}; 
 
  // Mandatory.
  // Array of strings. List of PLMNs for this TAC
  ans_cjson_t ans = find_object(it, "plmn_ids");  
  
  dst.sz_plmn_ids = cJSON_GetArraySize(ans.it);
  assert(dst.sz_plmn_ids > 0);
  
  dst.plmn_ids = calloc(dst.sz_plmn_ids, sizeof(char*));
  assert(dst.plmn_ids != NULL && "Memory exhausted");

  for(size_t i = 0; i< dst.sz_plmn_ids; ++i){
    cJSON * tmp = cJSON_GetArrayItem(ans.it, i);
    dst.plmn_ids[i] = strdup(tmp->valuestring);
  }

  // Mandatory. Reserved flag.
  dst.reserved = parse_bool(it, "reserved"); 

  // Mandatory. Tracking Area Code
  dst.tac = parse_int(it, "tac");

  // Optional. RAN Area Code.
  // int* ranac;

  // Array of objects. Mandatory? Optional?
  ans_cjson_t ans_nssai = find_object(it, "nssai");  
  dst.sz_nssai_amr = cJSON_GetArraySize(ans_nssai.it);
  assert(dst.sz_nssai_amr > 0 && "Not clear froms amari specs..." );
  
  dst.nssai = calloc(dst.sz_nssai_amr, sizeof(nssai_amr_t));
  assert(dst.nssai != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_nssai_amr; ++i){
    cJSON * tmp = cJSON_GetArrayItem(ans_nssai.it, i);
    dst.nssai[i] = parse_nssai_amr(tmp);
  }

  return dst;
}


