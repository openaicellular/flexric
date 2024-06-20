#include "e2sm_ccc_partition_lst.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

static
void free_e2sm_ccc_partition_lst_item(e2sm_ccc_partition_lst_item_t * src)
{
  assert(src != NULL);

//  src->p_number_of_rbs
//  src->p_offset_to_point_A
  // Mandatory
  free_e2sm_ccc_partition_flow_lst(&src->partition_flow_lst);
}

static
bool eq_e2sm_ccc_partition_lst_item(e2sm_ccc_partition_lst_item_t const* m0, e2sm_ccc_partition_lst_item_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  if (m0->p_offset_to_point_A != m1->p_offset_to_point_A)
    return false;

  // Mandatory
  if (m0->p_number_of_rbs != m1->p_number_of_rbs)
    return false;

  // Mandatory
  if(!eq_e2sm_ccc_partition_flow_lst(&m0->partition_flow_lst, &m1->partition_flow_lst))
      return false;

  return true;
}

static
e2sm_ccc_partition_lst_item_t cp_e2sm_ccc_partition_lst_item(e2sm_ccc_partition_lst_item_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_partition_lst_item_t dst = {0};

  // Mandatory
  dst.p_number_of_rbs = src->p_number_of_rbs;

  // Mandatory
  dst.p_offset_to_point_A = src->p_offset_to_point_A;

  // Mandatory
  dst.partition_flow_lst = cp_e2sm_ccc_partition_flow_lst(&src->partition_flow_lst);

  return dst;
}

void free_e2sm_ccc_partition_lst(e2sm_ccc_partition_lst_t* src)
{
  assert(src != NULL);

  // [1..128]
  assert(src->sz_partition_lst_item > 0 && src->sz_partition_lst_item< 129);
  for (size_t i = 0; i < src->sz_partition_lst_item; i++){
    free_e2sm_ccc_partition_lst_item(&src->partition_lst_item[i]);
  }
  free(src->partition_lst_item);
}

bool eq_e2sm_ccc_partition_lst(e2sm_ccc_partition_lst_t const* m0, e2sm_ccc_partition_lst_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // [1..128]
  assert(m0->sz_partition_lst_item > 0 && m1->sz_partition_lst_item < 129);
  assert(m0->sz_partition_lst_item > 0 && m1->sz_partition_lst_item < 129);
  if(m0->sz_partition_lst_item != m1->sz_partition_lst_item){
    return false;
  }

  for (size_t i = 0; i < m0->sz_partition_lst_item; i++){
    if(!eq_e2sm_ccc_partition_lst_item(&m0->partition_lst_item[i], &m1->partition_lst_item[i])){
      return false;
    }
  }

  return true;
}


e2sm_ccc_partition_lst_t cp_e2sm_ccc_partition_lst(e2sm_ccc_partition_lst_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_partition_lst_t dst = {0};

  // [1..128]
  assert(src->sz_partition_lst_item > 0 && src->sz_partition_lst_item < 129);
  dst.sz_partition_lst_item = src->sz_partition_lst_item;
  dst.partition_lst_item = calloc(src->sz_partition_lst_item, sizeof(e2sm_ccc_partition_lst_item_t));
  assert(dst.partition_lst_item!= NULL);
  for (size_t i = 0; i < dst.sz_partition_lst_item; i++){
    dst.partition_lst_item[i] = cp_e2sm_ccc_partition_lst_item(&src->partition_lst_item[i]);
  }

  return dst;
}



