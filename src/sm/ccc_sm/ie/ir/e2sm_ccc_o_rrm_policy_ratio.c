#include "e2sm_ccc_o_rrm_policy_ratio.h"

#include <assert.h>
#include <string.h>

void free_e2sm_ccc_o_rrm_policy_ratio(e2sm_ccc_o_rrm_policy_ratio_t* src)
{
  assert(src != NULL);

//  src->rrm_policy_max_ratio;
//  src->rrm_policy_min_ratio;
//  src->rrm_policy_dedicated_ratio;

  // [1..65536]
  assert(src->sz_rrm_policy_member_lst > 0 && src->sz_rrm_policy_member_lst < 65537);
  for (size_t i = 0; i < src->sz_rrm_policy_member_lst; ++i){
    free_e2sm_ccc_rrm_policy_member(&src->rrm_policy_member_lst[i]);
  }

  free(src->rrm_policy_member_lst);
}

bool eq_e2sm_ccc_o_rrm_policy_ratio(e2sm_ccc_o_rrm_policy_ratio_t const* m0, e2sm_ccc_o_rrm_policy_ratio_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->resource_type != m1->resource_type)
    return false;

  if(m0->rrm_policy_max_ratio != m1->rrm_policy_max_ratio)
    return false;

  if(m0->rrm_policy_min_ratio != m1->rrm_policy_min_ratio)
    return false;

  if(m0->rrm_policy_dedicated_ratio != m1->rrm_policy_dedicated_ratio)
    return false;

  // [1..65536]
  assert(m0->sz_rrm_policy_member_lst > 0 && m0->sz_rrm_policy_member_lst < 65537);
  assert(m1->sz_rrm_policy_member_lst > 0 && m1->sz_rrm_policy_member_lst < 65537);
  for(size_t i = 0; i < m0->sz_rrm_policy_member_lst; ++i){
    if (!eq_e2sm_ccc_rrm_policy_member(&m0->rrm_policy_member_lst[i], &m1->rrm_policy_member_lst[i])){
      return false;
    }
  }

  return true;
}


e2sm_ccc_o_rrm_policy_ratio_t cp_e2sm_ccc_o_rrm_policy_ratio(e2sm_ccc_o_rrm_policy_ratio_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_rrm_policy_ratio_t dst = {0};

  dst.resource_type = src->resource_type;
  dst.rrm_policy_max_ratio = src->rrm_policy_max_ratio;
  dst.rrm_policy_min_ratio = src->rrm_policy_min_ratio;
  dst.rrm_policy_dedicated_ratio = src->rrm_policy_dedicated_ratio;

  // [1..65536]
  assert(src->sz_rrm_policy_member_lst > 0 && src->sz_rrm_policy_member_lst < 65537);
  dst.sz_rrm_policy_member_lst = src->sz_rrm_policy_member_lst;
  dst.rrm_policy_member_lst = calloc(src->sz_rrm_policy_member_lst, sizeof(e2sm_ccc_rrm_policy_member_t));
  assert(dst.rrm_policy_member_lst != NULL);
  for (size_t i = 0; i < src->sz_rrm_policy_member_lst; i++){
    dst.rrm_policy_member_lst[i] = cp_e2sm_ccc_rrm_policy_member(&src->rrm_policy_member_lst[i]);
  }

  return dst;
}



