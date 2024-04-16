#include "e2sm_ccc_o_gnb_cu_up_function.h"

#include <assert.h>
#include <string.h>

void free_e2sm_ccc_o_gnb_cu_up_function(e2sm_ccc_o_gnb_cu_up_function_t* src)
{
  assert(src != NULL);

//  src->gnb_id;
//  src->gnb_id_len;
//  src->gnb_cu_up_id;

  // [1..65536]
  assert(src->sz_plmn_info_lst > 0 && src->sz_plmn_info_lst < 65537);
  for (size_t i = 0; i < src->sz_plmn_info_lst; ++i){
    free_e2sm_ccc_plmn_info(&src->plmn_info_lst[i]);
  }

  if (src->plmn_info_lst != NULL)
    free(src->plmn_info_lst);
}

bool eq_e2sm_ccc_o_gnb_cu_up_function(e2sm_ccc_o_gnb_cu_up_function_t const* m0, e2sm_ccc_o_gnb_cu_up_function_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->gnb_id != m1->gnb_id)
    return false;

  if(m0->gnb_id_len != m1->gnb_id_len)
    return false;

  if(m0->gnb_cu_up_id != m1->gnb_cu_up_id)
    return false;

  if(m0->sz_plmn_info_lst != m1->sz_plmn_info_lst)
    return false;

  // [1..65536]
  assert(m0->sz_plmn_info_lst > 0 && m0->sz_plmn_info_lst < 65537);
  assert(m1->sz_plmn_info_lst > 0 && m1->sz_plmn_info_lst < 65537);
  for(size_t i = 0; i < m0->sz_plmn_info_lst; ++i){
    if (!eq_e2sm_ccc_plmn_info(&m0->plmn_info_lst[i], &m1->plmn_info_lst[i])){
      return false;
    }
  }

  return true;
}


e2sm_ccc_o_gnb_cu_up_function_t cp_e2sm_ccc_o_gnb_cu_up_function(e2sm_ccc_o_gnb_cu_up_function_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_gnb_cu_up_function_t dst = {0};

  dst.gnb_id = src->gnb_id;
  dst.gnb_id_len = src->gnb_id_len;
  dst.gnb_cu_up_id = src->gnb_cu_up_id;

  // [1..65536]
  assert(src->sz_plmn_info_lst > 0 && src->sz_plmn_info_lst < 65537);
  dst.sz_plmn_info_lst = src->sz_plmn_info_lst;
  dst.plmn_info_lst = calloc(src->sz_plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));
  assert(dst.plmn_info_lst != NULL);
  memcpy(dst.plmn_info_lst, src->plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));

  return dst;
}



