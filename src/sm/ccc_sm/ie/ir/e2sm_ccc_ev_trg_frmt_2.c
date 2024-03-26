#include "e2sm_ccc_ev_trg_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ev_trg_cell_conf(ev_trg_cell_conf_t* src)
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
}

static
bool eq_ev_trg_cell_conf(ev_trg_cell_conf_t const* m0, ev_trg_cell_conf_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
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
ev_trg_cell_conf_t cp_ev_trg_cell_conf(ev_trg_cell_conf_t const* src)
{
  assert(src != NULL);
  ev_trg_cell_conf_t dst = {0};

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

void free_ev_trg_cell(ev_trg_cell_t* src)
{
  assert(src != NULL);

  if (src->cell_global_id.type == NR_CGI_RAT_TYPE || src->cell_global_id.type == EUTRA_CGI_RAT_TYPE)
    free_cell_global_id(&src->cell_global_id);

  // List of Cell-level RAN Configuration Structures
  // [1 - 1024]
  assert(src->sz_ev_trg_cell_conf > 0 && src->sz_ev_trg_cell_conf < 1024);
  for(size_t i = 0; i < src->sz_ev_trg_cell_conf; ++i){
    free_ev_trg_cell_conf(&src->ev_trg_cell_conf[i]);
  }
}

bool eq_ev_trg_cell(ev_trg_cell_t const* m0, ev_trg_cell_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if ((m0->cell_global_id.type == NR_CGI_RAT_TYPE || m0->cell_global_id.type == EUTRA_CGI_RAT_TYPE) &&
  (m1->cell_global_id.type == NR_CGI_RAT_TYPE || m1->cell_global_id.type == EUTRA_CGI_RAT_TYPE)){
    if(eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id) == false)
      return false;
  }

  if(m0->sz_ev_trg_cell_conf != m1->sz_ev_trg_cell_conf)
    return false;

  assert(m0->sz_ev_trg_cell_conf > 0 && m0->sz_ev_trg_cell_conf < 1024);
  assert(m1->sz_ev_trg_cell_conf > 0 && m1->sz_ev_trg_cell_conf < 1024);
  for(size_t i = 0; i < m0->sz_ev_trg_cell_conf; ++i){
    if(eq_ev_trg_cell_conf(&m0->ev_trg_cell_conf[i], &m1->ev_trg_cell_conf[i]) == false){
      return false;
    }
  }

  return true;
}

ev_trg_cell_t cp_ev_trg_cell(ev_trg_cell_t const* src)
{
  assert(src != NULL);
  ev_trg_cell_t dst = {0};

  if (src->cell_global_id.type == NR_CGI_RAT_TYPE || src->cell_global_id.type == EUTRA_CGI_RAT_TYPE)
    dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  // List of Cell-level RAN Configuration Structures
  // [1 - 1024]
  assert(src->sz_ev_trg_cell_conf > 0 && src->sz_ev_trg_cell_conf < 1024);
  dst.sz_ev_trg_cell_conf = src->sz_ev_trg_cell_conf;

  dst.ev_trg_cell_conf = calloc(dst.sz_ev_trg_cell_conf, sizeof(ev_trg_cell_conf_t));
  assert(dst.ev_trg_cell_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ev_trg_cell_conf; ++i){
    dst.ev_trg_cell_conf[i] = cp_ev_trg_cell_conf(&src->ev_trg_cell_conf[i]);
  }

  return dst;
}

void free_e2sm_ccc_ev_trg_frmt_2(e2sm_ccc_ev_trg_frmt_2_t const* src)
{
  assert(src != NULL);

  // List of Cell-level Configuration Structures
  // [1 - 1024]
  assert(src->sz_ev_trg_cell > 0 && src->sz_ev_trg_cell < 256);
  for(size_t i = 0; i < src->sz_ev_trg_cell; ++i){
    free_ev_trg_cell(&src->ev_trg_cell[i]);
  }
}

bool eq_e2sm_ccc_ev_trg_frmt_2(e2sm_ccc_ev_trg_frmt_2_t const* m0, e2sm_ccc_ev_trg_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ev_trg_cell != m1->sz_ev_trg_cell)
    return false;

  assert(m0->sz_ev_trg_cell > 0 && m0->sz_ev_trg_cell < 256);
  assert(m1->sz_ev_trg_cell> 0 && m1->sz_ev_trg_cell < 256);
  for(size_t i = 0; i < m0->sz_ev_trg_cell; ++i){
    if(eq_ev_trg_cell(&m0->ev_trg_cell[i], &m1->ev_trg_cell[i]) == false){
      return false;
    }

    return true;
  }

  return true;
}

e2sm_ccc_ev_trg_frmt_2_t cp_e2sm_ccc_ev_trg_frmt_2(e2sm_ccc_ev_trg_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ev_trg_frmt_2_t dst = {0};

  // List of Cell-level Configuration Structures
  // [1 - 1024]
  assert(src->sz_ev_trg_cell > 0 && src->sz_ev_trg_cell < 1024);
  dst.sz_ev_trg_cell = src->sz_ev_trg_cell;

  dst.ev_trg_cell = calloc(dst.sz_ev_trg_cell, sizeof(ev_trg_cell_t));
  assert(dst.ev_trg_cell!= NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ev_trg_cell; ++i){
    dst.ev_trg_cell[i] = cp_ev_trg_cell(&src->ev_trg_cell[i]);
  }

  return dst;
}

