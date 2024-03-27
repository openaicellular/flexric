#include "e2sm_ccc_act_def_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

static
void free_cell_conf(cell_conf_t* src)
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

static
bool eq_cell_conf(cell_conf_t const* m0, cell_conf_t const* m1)
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
cell_conf_t cp_cell_conf(cell_conf_t const* src)
{
  assert(src != NULL);
  assert(src->report_type == REPORT_TYPE_ALL || src->report_type == REPORT_TYPE_CHANGE);

  cell_conf_t dst = {.report_type = src->report_type};

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

static
void free_act_def_cell_report(act_def_cell_report_t *src)
{
  assert(src != NULL);

  free_cell_global_id(&src->cell_global_id);

  // List of Cell-level RAN Configuration Structures
  // [1-1024]
  assert(src->sz_cell_conf > 0 && src->sz_cell_conf < 1025);
  for (size_t i = 0; i < src->sz_cell_conf; i++){
    free_cell_conf(src->cell_conf);
  }
}

static
bool eq_act_def_cell_report(act_def_cell_report_t const* m0, act_def_cell_report_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(!eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id))
    return false;

  for (size_t i = 0; i < m0->sz_cell_conf; i++){
    if (!eq_cell_conf(m0->cell_conf, m1->cell_conf))
      return false;
  }

  return true;
}

static
act_def_cell_report_t cp_act_def_cell_report(act_def_cell_report_t const* src)
{
  assert(src != NULL);

  act_def_cell_report_t dst = {0};

  dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  // List of Cell-level RAN Configuration Structures
  // [1 - 1024]
  assert(src->sz_cell_conf > 0 && src->sz_cell_conf < 1025);
  dst.sz_cell_conf = src->sz_cell_conf;

  dst.cell_conf = calloc(dst.sz_cell_conf, sizeof(cell_conf_t));
  assert(dst.cell_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_cell_conf; ++i){
    dst.cell_conf[i] = cp_cell_conf(&src->cell_conf[i]);
  }

  return dst;
}

void free_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t* src)
{
  assert(src != NULL);

  // Parameters to be Reported List
  // [1-1024]
  assert(src->sz_act_def_cell_report > 0 && src->sz_act_def_cell_report < 1025);

  for(size_t i = 0; i < src->sz_act_def_cell_report; ++i){
    free_act_def_cell_report(&src->act_def_cell_report[i]);
  }
}

bool eq_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const* m0, e2sm_ccc_act_def_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  if(m0->sz_act_def_cell_report != m1->sz_act_def_cell_report)
    return false;

  for(size_t i = 0; i < m0->sz_act_def_cell_report; ++i){
    if(eq_act_def_cell_report(&m0->act_def_cell_report[i], &m1->act_def_cell_report[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_act_def_frmt_2_t cp_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_act_def_frmt_2_t dst = {0};

  // Parameters to be Reported List
  // [1-1024]
  assert(src->sz_act_def_cell_report > 0 && src->sz_act_def_cell_report < 1025);
  dst.sz_act_def_cell_report = src->sz_act_def_cell_report;

  dst.act_def_cell_report = calloc(dst.sz_act_def_cell_report, sizeof(act_def_cell_report_t));
  assert(dst.act_def_cell_report != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_act_def_cell_report ; ++i){
    dst.act_def_cell_report[i] = cp_act_def_cell_report(&src->act_def_cell_report[i]);
  }

  return dst;
}
