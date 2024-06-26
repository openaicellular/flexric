#include "ran_param_lst_struct.h"
#include "ran_param_def.h"

#include <assert.h>
#include <stdlib.h>


void free_ran_param_lst_struct(ran_param_lst_struct_t* src)
{
  assert(src != NULL);

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  // uint32_t ran_param_id;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  free_byte_array(src->ran_param_name);

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(src->ran_param_def == NULL && "Not implemented");
}

bool eq_ran_param_lst_struct(ran_param_lst_struct_t const* m0, ran_param_lst_struct_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  if (m0->ran_param_id != m1->ran_param_id)
    return false;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  if(eq_byte_array(&m0->ran_param_name, &m1->ran_param_name) == false)
    return false;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  assert(m0->ran_param_def == NULL && "Not implemented");
  assert(m1->ran_param_def == NULL && "Not implemented");

  return true;
}

ran_param_lst_struct_t cp_ran_param_lst_struct(ran_param_lst_struct_t const* src)
{
  assert(src != NULL);

  ran_param_lst_struct_t dst = {0}; 
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  dst.ran_param_id = src->ran_param_id;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  dst.ran_param_name = copy_byte_array(src->ran_param_name);

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  if(src->ran_param_def != NULL){
    dst.ran_param_def = calloc(1, sizeof(ran_param_def_t));
    assert(dst.ran_param_def != NULL && "Memory exhausted");
    *dst.ran_param_def = cp_ran_param_def(src->ran_param_def); 
  }

  return dst;
}

