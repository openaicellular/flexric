#include "e2sm_ccc_partition_flow_lst.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static
void free_e2sm_ccc_partition_flow_item(e2sm_ccc_partition_flow_lst_item_t * src)
{
  assert(src != NULL);

  // Mandatory
  free_e2sm_plmn(&src->plmn_id);

  // Mandatory
  free_s_nssai_e2sm(&src->s_nssai);

  // Optional
  if (src->lst_5qi != NULL)
    free_e2sm_ccc_5qi_lst(src->lst_5qi);
  free(src->lst_5qi);
}

static
bool eq_e2sm_ccc_partition_flow_item(e2sm_ccc_partition_flow_lst_item_t const* m0, e2sm_ccc_partition_flow_lst_item_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  if(!eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id))
    return false;

  // Mandatory
  if(!eq_s_nssai_e2sm(&m0->s_nssai, &m1->s_nssai))
    return false;

  // Optional
  if (m0->lst_5qi != NULL && m1->lst_5qi != NULL)
    if(!eq_e2sm_ccc_5qi_lst(m0->lst_5qi, m1->lst_5qi))
      return false;

  return true;
}

static
e2sm_ccc_partition_flow_lst_item_t cp_e2sm_ccc_partition_flow_item(e2sm_ccc_partition_flow_lst_item_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_partition_flow_lst_item_t dst = {0};

  // Mandatory
  dst.plmn_id = cp_e2sm_plmn(&src->plmn_id);

  // Mandatory
  dst.s_nssai.sST = src->s_nssai.sST;
  if (src->s_nssai.sD != NULL){
    dst.s_nssai.sD = calloc(1, sizeof(uint32_t));
    assert(dst.s_nssai.sD != NULL);
    *dst.s_nssai.sD = *src->s_nssai.sD;
  }

  // Optional
  if (src->lst_5qi){
    dst.lst_5qi = calloc(1, sizeof(e2sm_ccc_partition_flow_lst_item_t));
    assert(dst.lst_5qi != NULL);
    *dst.lst_5qi = cp_e2sm_ccc_5qi_lst(src->lst_5qi);
  }

  return dst;
}

void free_e2sm_ccc_partition_flow_lst(e2sm_ccc_partition_flow_lst_t* src)
{
  assert(src != NULL);

  // [1..128]
  assert(src->sz_partition_flow_lst_item > 0 && src->sz_partition_flow_lst_item < 129);
  for (size_t i = 0; i < src->sz_partition_flow_lst_item; i++){
    free_e2sm_ccc_partition_flow_item(&src->partition_flow_lst_item[i]);
  }
  free(src->partition_flow_lst_item);
}

bool eq_e2sm_ccc_partition_flow_lst(e2sm_ccc_partition_flow_lst_t const* m0, e2sm_ccc_partition_flow_lst_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // [1..128]
  assert(m0->sz_partition_flow_lst_item > 0 && m1->sz_partition_flow_lst_item < 129);
  assert(m0->sz_partition_flow_lst_item > 0 && m1->sz_partition_flow_lst_item < 129);
  if(m0->sz_partition_flow_lst_item != m1->sz_partition_flow_lst_item){
    return false;
  }

  for (size_t i = 0; i < m0->sz_partition_flow_lst_item; i++){
    if(!eq_e2sm_ccc_partition_flow_item(&m0->partition_flow_lst_item[i], &m1->partition_flow_lst_item[i])){
      return false;
    }
  }

  return true;
}


e2sm_ccc_partition_flow_lst_t cp_e2sm_ccc_partition_flow_lst(e2sm_ccc_partition_flow_lst_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_partition_flow_lst_t dst = {0};

  // [1..128]
  assert(src->sz_partition_flow_lst_item > 0 && src->sz_partition_flow_lst_item < 129);
  dst.sz_partition_flow_lst_item = src->sz_partition_flow_lst_item;
  dst.partition_flow_lst_item = calloc(src->sz_partition_flow_lst_item, sizeof(e2sm_ccc_partition_flow_lst_item_t));
  assert(dst.partition_flow_lst_item != NULL);
  for (size_t i = 0; i < dst.sz_partition_flow_lst_item; i++){
    dst.partition_flow_lst_item[i] = cp_e2sm_ccc_partition_flow_item(&src->partition_flow_lst_item[i]);
  }


  return dst;
}



