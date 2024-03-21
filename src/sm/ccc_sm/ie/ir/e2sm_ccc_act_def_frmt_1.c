#include "e2sm_ccc_act_def_frmt_1.h"

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

static
void free_act_def_node_conf(act_def_node_conf_t* src)
{
  assert(src != NULL);
  // [0 - 66535]
  assert(src->sz_attribute < 66535);

  free_byte_array(src->ran_conf_name);

  if (src->attribute != NULL) {
    for (size_t i = 0; i < src->sz_attribute; i++){
      free_byte_array(src->attribute[i].attribute_name);
    }
    free(src->attribute);
  }

  free(src);
}

static
bool eq_act_def_node_conf(act_def_node_conf_t const* m0, act_def_node_conf_t const* m1)
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

static
act_def_node_conf_t cp_act_def_node_conf(act_def_node_conf_t const* src)
{
  assert(src != NULL);
  assert(src->report_type == REPORT_TYPE_ALL || src->report_type == REPORT_TYPE_CHANGE);

  act_def_node_conf_t dst = {.report_type = src->report_type};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  // [0 - 66535]
  dst.sz_attribute = src->sz_attribute;
  if (src->attribute != NULL){
    dst.attribute = calloc(src->sz_attribute, sizeof(attribute_t));
    assert(dst.attribute != NULL && "Memory exhausted");
    memcpy(dst.attribute, src->attribute, sizeof(attribute_t) * src->sz_attribute);
  }

  return dst;
}

void free_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t* src)
{
  assert(src != NULL);

  // Parameters to be Reported List
  // [1-256]
  assert(src->sz_act_def_node_conf > 0 && src->sz_act_def_node_conf < 256);

  for(size_t i = 0; i < src->sz_act_def_node_conf; ++i){
    free_act_def_node_conf(&src->act_def_node_conf[i]);
  }

  assert(src->act_def_node_conf != NULL);
  free(src->act_def_node_conf);
}


bool eq_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t const* m0, e2sm_ccc_act_def_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  assert(m0->sz_act_def_node_conf > 0);
  assert(m1->sz_act_def_node_conf > 0);

  if(m0->sz_act_def_node_conf != m1->sz_act_def_node_conf)
    return false;

  for(size_t i = 0; i < m0->sz_act_def_node_conf; ++i){
    if(eq_act_def_node_conf(&m0->act_def_node_conf[i], &m1->act_def_node_conf[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_act_def_frmt_1_t cp_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_act_def_frmt_1_t dst = {0}; 

  // Parameters to be Reported List
  // [1-256]
  assert(src->sz_act_def_node_conf > 0 && src->sz_act_def_node_conf < 256);
  dst.sz_act_def_node_conf = src->sz_act_def_node_conf;

  dst.act_def_node_conf = calloc(dst.sz_act_def_node_conf, sizeof(act_def_node_conf_t));
  assert(dst.act_def_node_conf!= NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_act_def_node_conf; ++i){
    dst.act_def_node_conf[i] = cp_act_def_node_conf(&src->act_def_node_conf[i]);
  }

  return dst;
}

