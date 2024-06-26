#include "param_report_def.h"

#include <assert.h>
#include <stdlib.h>

void free_param_report_def(param_report_def_t* src)
{
  assert(src != NULL);
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // uint32_t ran_param_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  if (src->ran_param_def != NULL)
  {
    free_ran_param_def(src->ran_param_def);
    free(src->ran_param_def);
  }

}

bool eq_param_report_def(param_report_def_t const* m0, param_report_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(m0->ran_param_id > 0); 
  assert(m1->ran_param_id > 0); 
  if(m0->ran_param_id != m1->ran_param_id)
    return false;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  if (m0->ran_param_def != NULL || m1->ran_param_def != NULL) {
    if(m0->ran_param_def == NULL || m1->ran_param_def == NULL)
      return false;
    
    if(eq_ran_param_def(m0->ran_param_def, m1->ran_param_def) == false)
      return false;
  }

  return true;
}

param_report_def_t cp_param_report_def(param_report_def_t const* src)
{
  assert(src != NULL);
  param_report_def_t dst = {0};
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // [1 - 4294967295]
  assert(src->ran_param_id > 0);
  dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  if(src->ran_param_def != NULL ){ 
    dst.ran_param_def = calloc(1, sizeof(ran_param_def_t));
    assert(dst.ran_param_def != NULL && "Memory exhausted");
    *dst.ran_param_def = cp_ran_param_def(src->ran_param_def);
  }

  return dst;
}

