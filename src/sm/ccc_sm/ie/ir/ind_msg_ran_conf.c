#include "ind_msg_ran_conf.h"

#include <assert.h>

void free_ind_msg_ran_conf(ind_msg_ran_conf_t* src)
{
  assert(src != NULL);
  assert(src->change_type == CHANGE_TYPE_ADDITION ||
         src->change_type == CHANGE_TYPE_DELETION ||
         src->change_type == CHANGE_TYPE_MODIFICATION ||
         src->change_type == CHANGE_TYPE_NONE);

  free_byte_array(src->ran_conf_name);

  free_byte_array(src->vals_attributes);

  // Optional
  if (src->old_vals_attributes.buf != NULL)
    free_byte_array(src->old_vals_attributes);
}

bool eq_ind_msg_ran_conf(ind_msg_ran_conf_t const* m0, ind_msg_ran_conf_t  const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->change_type != m1->change_type)
    return false;

  if(eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name) == false)
    return false;

  if(eq_byte_array(&m0->vals_attributes, &m1->vals_attributes) == false)
    return false;

  // Optional
  if(m0->old_vals_attributes.buf != NULL && m1->old_vals_attributes.buf != NULL)
    if(eq_byte_array(&m0->old_vals_attributes, &m1->old_vals_attributes) == false)
      return false;

  return true;
}

ind_msg_ran_conf_t cp_ind_msg_ran_conf(ind_msg_ran_conf_t const* src)
{
  assert(src != NULL);
  assert(src->change_type == CHANGE_TYPE_ADDITION ||
         src->change_type == CHANGE_TYPE_DELETION ||
         src->change_type == CHANGE_TYPE_MODIFICATION ||
         src->change_type == CHANGE_TYPE_NONE);

  ind_msg_ran_conf_t dst = {.change_type = src->change_type};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  dst.vals_attributes = copy_byte_array(src->vals_attributes);

  if (src->old_vals_attributes.buf != NULL)
    dst.old_vals_attributes = copy_byte_array(src->old_vals_attributes);

  return dst;
}



