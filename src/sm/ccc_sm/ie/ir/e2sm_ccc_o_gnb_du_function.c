#include "e2sm_ccc_o_gnb_du_function.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_o_gnb_du_function(e2sm_ccc_o_gnb_du_function_t* src)
{
  assert(src != NULL);

//  src->gnb_du_id;
//  src->gnb_id;
//  src->gnb_id_len;

  if(src->gnb_du_name != NULL)
    free(src->gnb_du_name);
}

bool eq_e2sm_ccc_o_gnb_du_function(e2sm_ccc_o_gnb_du_function_t const* m0, e2sm_ccc_o_gnb_du_function_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->gnb_du_id != m1->gnb_du_id)
    return false;

  if(m0->gnb_du_name != NULL && m1->gnb_du_name != NULL)
    if (strcmp(m0->gnb_du_name, m1->gnb_du_name) != 0)
      return false;

  if (m0->gnb_id != m1->gnb_id)
    return false;

  if (m0->gnb_id_len != m1->gnb_id_len)
    return false;

  return true;
}


e2sm_ccc_o_gnb_du_function_t cp_e2sm_ccc_o_gnb_du_function(e2sm_ccc_o_gnb_du_function_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_gnb_du_function_t dst = {0};

  dst.gnb_du_id = src->gnb_du_id;
  if (src->gnb_du_name != NULL){
    dst.gnb_du_name = strdup(src->gnb_du_name);
    assert(dst.gnb_du_name != NULL);
  }
  dst.gnb_id = src->gnb_id;
  dst.gnb_id_len = src->gnb_id_len;

  return dst;
}



