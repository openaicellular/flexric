#include "ev_trg_ran_conf.h"

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void free_ev_trg_ran_conf(ev_trg_ran_conf_t* src)
{
  assert(src != NULL);

  free_byte_array(src->ran_conf_name);

  // List of Attributes
  // [0 - 66535]
  assert(src->sz_attribute < 66536);
  for (size_t i = 0; i < src->sz_attribute; i++){
    free_byte_array(src->attribute[i].attribute_name);
  }

  if (src->attribute != NULL) {
    free(src->attribute);
  }
}

bool eq_ev_trg_ran_conf(ev_trg_ran_conf_t const* m0, ev_trg_ran_conf_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(!eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name))
    return false;

  // List of Attributes
  // [0 - 66535]
  assert(m0->sz_attribute < 66536);
  assert(m1->sz_attribute < 66536);
  for (size_t i = 0; i < m0->sz_attribute; i++){
    if (!eq_byte_array(&m0->attribute[i].attribute_name, &m1->attribute[i].attribute_name))
      return false;
  }

  return true;
}


ev_trg_ran_conf_t cp_ev_trg_ran_conf(ev_trg_ran_conf_t const* src)
{
  assert(src != NULL);
  ev_trg_ran_conf_t dst = {0};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  // [0 - 66535]
  assert(src->sz_attribute < 66536);
  dst.sz_attribute = src->sz_attribute;
  if (src->attribute != NULL){
    dst.attribute = calloc(src->sz_attribute, sizeof(attribute_t));
    assert(dst.attribute != NULL && "Memory exhausted");
    for(size_t i = 0; i < src->sz_attribute; ++i)
      dst.attribute[i] = cp_attribute(&src->attribute[i]);
  }

  return dst;
}



