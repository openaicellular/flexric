#include "ran_param_def.h"

#include <assert.h>
#include <stdlib.h>

void free_ran_param_def(ran_param_def_t* src)
{
  assert(src != NULL);
  assert(src->type == LIST_RAN_PARAMETER_DEF_TYPE || src->type == STRUCTURE_RAN_PARAMETER_DEF_TYPE);
  
  switch (src->type)
  {
  case LIST_RAN_PARAMETER_DEF_TYPE:
    free_ran_param_type(src->lst);
    free(src->lst);
    break;

  case STRUCTURE_RAN_PARAMETER_DEF_TYPE:
    free_ran_param_type(src->strct);
    free(src->strct);
    break;

  default:
    assert(false && "Unknown RAN Parameter Type");
  }

}

bool eq_ran_param_def(ran_param_def_t const* m0, ran_param_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type) return false;

  switch (m0->type)
  {
  case LIST_RAN_PARAMETER_DEF_TYPE:
    if(eq_ran_param_type(m0->lst, m1->lst) == false)
      return false;

    break;

  case STRUCTURE_RAN_PARAMETER_DEF_TYPE:
    if(eq_ran_param_type(m0->strct, m1->strct) == false)
      return false;

    break;

  default:
    assert(false && "Unknown RAN Parameter Type");
  }

  return true;
}

ran_param_def_t cp_ran_param_def(ran_param_def_t const* src)
{
  assert(src != NULL);
  assert(src->type == LIST_RAN_PARAMETER_DEF_TYPE || src->type == STRUCTURE_RAN_PARAMETER_DEF_TYPE);

  ran_param_def_t dst = {.type = src->type}; 

  if(dst.type == LIST_RAN_PARAMETER_DEF_TYPE){
    dst.lst = calloc(1, sizeof(ran_param_type_t));
    assert(dst.lst != NULL && "Memory exhausted");
    *dst.lst = cp_ran_param_type(src->lst);
  } else if(dst.type == STRUCTURE_RAN_PARAMETER_DEF_TYPE){
    dst.strct = calloc(1, sizeof(ran_param_type_t));
    assert(dst.strct != NULL && "Memory exhausted");
    *dst.strct = cp_ran_param_type(src->strct);
  } else {
    assert(0!=0 && "Unknown type");
  }

  return dst;
}

