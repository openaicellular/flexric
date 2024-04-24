#include "e2sm_ccc_5qi_lst.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_5qi_lst(e2sm_ccc_5qi_lst_t* src)
{
  assert(src != NULL);

  // [1..128]
  assert(src->sz_lst_5qi > 0 && src->sz_lst_5qi < 129);
//  for (size_t i = 0; i < src->sz_lst_5qi; ++i){
//    free(src->lst_5qi[i]);
//  }

  free(src->lst_5qi);
}

bool eq_e2sm_ccc_5qi_lst(e2sm_ccc_5qi_lst_t const* m0, e2sm_ccc_5qi_lst_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // [1..128]
  assert(m0->sz_lst_5qi > 0 && m0->sz_lst_5qi < 129);
  assert(m1->sz_lst_5qi > 0 && m1->sz_lst_5qi < 129);
  for(size_t i = 0; i < m0->sz_lst_5qi ; ++i) {
    if (m0->lst_5qi[i] != m1->lst_5qi[i]) {
      return false;
    }
  }

  return true;
}


e2sm_ccc_5qi_lst_t cp_e2sm_ccc_5qi_lst(e2sm_ccc_5qi_lst_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_5qi_lst_t dst = {0};

  // [1..128]
  assert(src->sz_lst_5qi > 0 && src->sz_lst_5qi < 129);
  dst.sz_lst_5qi = src->sz_lst_5qi;
  dst.lst_5qi = calloc(src->sz_lst_5qi, sizeof(uint32_t));
  assert(dst.lst_5qi!= NULL);
  memcpy(dst.lst_5qi, src->lst_5qi, sizeof(uint32_t));

  return dst;
}



