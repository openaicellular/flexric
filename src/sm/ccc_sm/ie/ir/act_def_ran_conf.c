#include "act_def_ran_conf.h"

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void free_act_def_ran_conf(act_def_ran_conf_t* src)
{
  assert(src != NULL);
  // [0 - 66535]
  assert(src->sz_attribute < 66536);

  free_byte_array(src->ran_conf_name);

  if (src->attribute != NULL) {
    for (size_t i = 0; i < src->sz_attribute; i++){
      free_byte_array(src->attribute[i].attribute_name);
    }
    free(src->attribute);
  }
}

bool eq_act_def_ran_conf(act_def_ran_conf_t const* m0, act_def_ran_conf_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->report_type != m1->report_type)
    return false;

  if(!eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name))
    return false;

  for (size_t i = 0; i < m0->sz_attribute; i++){
    if (!eq_byte_array(&m0->attribute[i].attribute_name, &m1->attribute[i].attribute_name))
      return false;
  }

  return true;
}


act_def_ran_conf_t cp_act_def_ran_conf(act_def_ran_conf_t const* src)
{
  assert(src != NULL);
  assert(src->report_type == REPORT_TYPE_ALL || src->report_type == REPORT_TYPE_CHANGE);

  act_def_ran_conf_t dst = {.report_type = src->report_type};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  // [0 - 66535]
  dst.sz_attribute = src->sz_attribute;
  if (src->attribute != NULL){
    dst.attribute = calloc(src->sz_attribute, sizeof(attribute_t));
    assert(dst.attribute != NULL && "Memory exhausted");
    for (size_t i = 0; i < src->sz_attribute; i++){
      dst.attribute[i] = cp_attribute(&src->attribute[i]);
    }
  }

  return dst;
}



