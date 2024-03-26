#include "ctrl_out_conf_accepted.h"

#include <assert.h>

void free_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t * src)
{
  assert(src != NULL);

  free_byte_array(src->ran_conf_name);

  free_byte_array(src->old_atr_val);

  free_byte_array(src->cur_atr_val);

  if(src->app_timestamp.buf != NULL)
    free_byte_array(src->app_timestamp);
}

bool eq_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t const* m0, ctrl_out_conf_accepted_t const* m1) {
  if (m0 == m1)
    return true;

  if (m0 == NULL || m1 == NULL)
    return false;

  if (!eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name))
    return false;

  if (!eq_byte_array(&m0->old_atr_val, &m1->old_atr_val))
    return false;

  if (!eq_byte_array(&m0->cur_atr_val, &m1->cur_atr_val))
    return false;

  if (m0->app_timestamp.buf != NULL && m1->app_timestamp.buf != NULL)
    if(eq_byte_array(&m0->app_timestamp, &m1->app_timestamp) == false)
      return false;

  return true;
}

ctrl_out_conf_accepted_t cp_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t const* src)
{
  assert(src != NULL);

  ctrl_out_conf_accepted_t  dst = {0};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  dst.old_atr_val= copy_byte_array(src->old_atr_val);

  dst.cur_atr_val= copy_byte_array(src->cur_atr_val);

  if (src->app_timestamp.buf != NULL)
    dst.app_timestamp = copy_byte_array(src->app_timestamp);

  return dst;
}


