#include "ctrl_out_conf_failed.h"

#include <assert.h>
#include <stdlib.h>

void free_ctrl_out_conf_failed(ctrl_out_conf_failed_t * src)
{
  assert(src != NULL);
  assert(src->cause == CAUSE_INCOMPATIBLE_STATE||
         src->cause == CAUSE_JSON_ERROR||
         src->cause == CAUSE_NOT_AVAILABLE||
         src->cause == CAUSE_NOT_SUPPORTED||
         src->cause == CAUSE_SEMANTIC_ERROR||
         src->cause == CAUSE_UNSPECIFIED
  );

  free_byte_array(src->ran_conf_name);

  free_values_of_attributes(&src->old_atr_val);

  free_values_of_attributes(&src->req_atr_val);
}

bool eq_ctrl_out_conf_failed(ctrl_out_conf_failed_t const* m0, ctrl_out_conf_failed_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->cause != m1->cause)
    return false;

  if(!eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name))
    return false;

  if(!eq_values_of_attributes(&m0->old_atr_val, &m1->old_atr_val))
    return false;

  if(!eq_values_of_attributes(&m0->req_atr_val, &m1->req_atr_val))
    return false;

  return true;
}

ctrl_out_conf_failed_t cp_ctrl_out_conf_failed(ctrl_out_conf_failed_t const* src)
{
  assert(src != NULL);
  assert(src->cause == CAUSE_INCOMPATIBLE_STATE||
         src->cause == CAUSE_JSON_ERROR||
         src->cause == CAUSE_NOT_AVAILABLE||
         src->cause == CAUSE_NOT_SUPPORTED||
         src->cause == CAUSE_SEMANTIC_ERROR||
         src->cause == CAUSE_UNSPECIFIED
  );

  ctrl_out_conf_failed_t dst = {.cause = src->cause};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  dst.old_atr_val= cp_values_of_attributes(&src->old_atr_val);

  dst.req_atr_val = cp_values_of_attributes(&src->req_atr_val);

  return dst;
}



