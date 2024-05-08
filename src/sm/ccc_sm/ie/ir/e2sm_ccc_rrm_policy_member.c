#include "e2sm_ccc_rrm_policy_member.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_rrm_policy_member(e2sm_ccc_rrm_policy_member_t* src)
{
  assert(src != NULL);

  // Mandatory
  free_e2sm_plmn(&src->plmn_id);

  // Optional
  if (src->s_nssai != NULL){
    free_s_nssai_e2sm(src->s_nssai);
    free(src->s_nssai);
  }
}

bool eq_e2sm_ccc_rrm_policy_member(e2sm_ccc_rrm_policy_member_t const* m0, e2sm_ccc_rrm_policy_member_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  if(!eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id))
    return false;

  // Optional
  if (m0->s_nssai != NULL && m1->s_nssai != NULL)
    if(!eq_s_nssai_e2sm(m0->s_nssai, m1->s_nssai))
      return false;

  return true;
}


e2sm_ccc_rrm_policy_member_t cp_e2sm_ccc_rrm_policy_member(e2sm_ccc_rrm_policy_member_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_rrm_policy_member_t dst = {0};

  // Mandatory
  dst.plmn_id = cp_e2sm_plmn(&src->plmn_id);

  // Optional
  if (src->s_nssai != NULL){
    dst.s_nssai = calloc(1, sizeof(s_nssai_e2sm_t));
    assert(dst.s_nssai!= NULL);
    *dst.s_nssai = cp_s_nssai_e2sm(src->s_nssai);
  }
  return dst;
}



