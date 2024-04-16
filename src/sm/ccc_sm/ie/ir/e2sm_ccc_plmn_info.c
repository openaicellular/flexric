#include "e2sm_ccc_plmn_info.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_plmn_info(e2sm_ccc_plmn_info_t* src)
{
  assert(src != NULL);

  // Mandatory
  assert(src->plmn_id != NULL);
  free_e2sm_plmn(src->plmn_id);

  // Optional
  if (src->s_nssai.sD != NULL)
    free_s_nssai_e2sm(&src->s_nssai);
}

bool eq_e2sm_ccc_plmn_info(e2sm_ccc_plmn_info_t const* m0, e2sm_ccc_plmn_info_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  assert(m0->plmn_id != NULL);
  assert(m1->plmn_id != NULL);
  if(!eq_e2sm_plmn(m0->plmn_id, m1->plmn_id))
    return false;

  // Optional
  if(!eq_s_nssai_e2sm(&m0->s_nssai, &m1->s_nssai))
    return false;

  return true;
}


e2sm_ccc_plmn_info_t cp_e2sm_ccc_plmn_info(e2sm_ccc_plmn_info_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_plmn_info_t dst = {0};

  // Mandatory
  assert(src->plmn_id != NULL);
  dst.plmn_id = calloc(1, sizeof(e2sm_plmn_t));
  assert(dst.plmn_id != NULL);
  *dst.plmn_id = cp_e2sm_plmn(src->plmn_id);

  // Optional
  if (dst.s_nssai.sD != NULL){
    dst.s_nssai.sD = calloc(1, sizeof(uint32_t));
    assert(dst.s_nssai.sD != NULL);
    *dst.s_nssai.sD = *src->s_nssai.sD;
  }
  dst.s_nssai.sST = src->s_nssai.sST;
  return dst;
}



