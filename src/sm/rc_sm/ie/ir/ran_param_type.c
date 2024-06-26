#include "ran_param_type.h"
#include <assert.h>
#include <stdlib.h>

void free_ran_param_type(ran_param_type_t* src)
{
  assert(src != NULL);

  for(size_t i = 0; i < src->sz_ran_param; i++) {
    free_ran_param_lst_struct(&src->ran_param[i]);
  }
  free(src->ran_param);
}


bool eq_ran_param_type(ran_param_type_t const* m0, ran_param_type_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->sz_ran_param != m1->sz_ran_param) return false;

  for (size_t i = 0; i < m0->sz_ran_param; i++) {
    if(eq_ran_param_lst_struct(&m0->ran_param[i], &m1->ran_param[i] ) == false)
      return false;
  }

  return true;
}

ran_param_type_t cp_ran_param_type(ran_param_type_t const* src)
{
  assert(src != NULL);

  ran_param_type_t dst = {0}; 
  assert(src->sz_ran_param > 0);

  dst.sz_ran_param = src->sz_ran_param;
  dst.ran_param = calloc(dst.sz_ran_param, sizeof(ran_param_lst_struct_t));
  assert(dst.ran_param != NULL && "Memory exhausted");
  for(size_t i = 0; i < dst.sz_ran_param; ++i){
    dst.ran_param[i] = cp_ran_param_lst_struct(&src->ran_param[i]);
  }

  return dst;
}

