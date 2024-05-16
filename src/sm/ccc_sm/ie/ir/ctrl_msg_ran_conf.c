#include "ctrl_msg_ran_conf.h"

#include <assert.h>

void free_ctrl_msg_ran_conf(ctrl_msg_ran_conf_t* src)
{
  assert(src != NULL);

  free_byte_array(src->ran_conf_name);

  free_values_of_attributes(&src->vals_attributes);

  free_values_of_attributes(&src->old_vals_attributes);
}

bool eq_ctrl_msg_ran_conf(ctrl_msg_ran_conf_t const* m0, ctrl_msg_ran_conf_t  const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name) == false)
    return false;

  if(eq_values_of_attributes(&m0->vals_attributes, &m1->vals_attributes) == false)
    return false;

  if(eq_values_of_attributes(&m0->old_vals_attributes, &m1->old_vals_attributes) == false)
    return false;

  return true;
}

ctrl_msg_ran_conf_t cp_ctrl_msg_ran_conf(ctrl_msg_ran_conf_t const* src)
{
  assert(src != NULL);

  ctrl_msg_ran_conf_t dst = {0};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  dst.vals_attributes = cp_values_of_attributes(&src->vals_attributes);

  dst.old_vals_attributes = cp_values_of_attributes(&src->old_vals_attributes);

  return dst;
}



