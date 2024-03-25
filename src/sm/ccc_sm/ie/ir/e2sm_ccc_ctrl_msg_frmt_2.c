#include "e2sm_ccc_ctrl_msg_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ctrl_msg_cell_conf(ctrl_msg_cell_conf_t* src)
{
  assert(src != NULL);

  assert(src->ran_conf_name.buf != NULL);
  free_byte_array(src->ran_conf_name);

  assert(src->old_atr_val.buf != NULL);
  free_byte_array(src->old_atr_val);

  assert(src->new_atr_val.buf != NULL);
  free_byte_array(src->new_atr_val);
}

static
bool eq_ctrl_msg_cell_conf(ctrl_msg_cell_conf_t const* m0, ctrl_msg_cell_conf_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  assert(m0->ran_conf_name.buf != NULL);
  assert(m1->ran_conf_name.buf != NULL);
  if(!eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name))
    return false;

  assert(m0->old_atr_val.buf != NULL);
  assert(m1->old_atr_val.buf != NULL);
  if(!eq_byte_array(&m0->old_atr_val, &m1->old_atr_val))
    return false;

  assert(m0->new_atr_val.buf != NULL);
  assert(m1->new_atr_val.buf != NULL);
  if(!eq_byte_array(&m0->new_atr_val, &m1->new_atr_val))
    return false;

  return true;
}

static
ctrl_msg_cell_conf_t cp_ctrl_msg_cell_conf(ctrl_msg_cell_conf_t const* src)
{
  assert(src != NULL);

  ctrl_msg_cell_conf_t dst = {0};

  assert(dst.ran_conf_name.buf != NULL);
  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  assert(dst.old_atr_val.buf != NULL);
  dst.old_atr_val= copy_byte_array(src->old_atr_val);

  assert(dst.new_atr_val.buf != NULL);
  dst.new_atr_val= copy_byte_array(src->new_atr_val);

  return dst;
}

static
void free_ctrl_msg_cell(ctrl_msg_cell_t* src)
{
  assert(src != NULL);

  free_cell_global_id(&src->cell_global_id);

  // List of Configuration Structures
  // [1-65535]
  assert(src->sz_ctrl_msg_cell_conf> 0 && src->sz_ctrl_msg_cell_conf < 65535);
  for(size_t i = 0; i < src->sz_ctrl_msg_cell_conf; ++i){
    free_ctrl_msg_cell_conf(&src->ctrl_msg_cell_conf[i]);
  }
}

static
bool eq_ctrl_msg_cell(ctrl_msg_cell_t const* m0, ctrl_msg_cell_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ctrl_msg_cell_conf != m1->sz_ctrl_msg_cell_conf)
    return false;

  assert(m0->sz_ctrl_msg_cell_conf > 0 && m0->sz_ctrl_msg_cell_conf < 65535);
  assert(m1->sz_ctrl_msg_cell_conf > 0 && m1->sz_ctrl_msg_cell_conf < 65535);
  for(size_t i = 0; i < m0->sz_ctrl_msg_cell_conf; ++i){
    if(eq_ctrl_msg_cell_conf(&m0->ctrl_msg_cell_conf[i], &m1->ctrl_msg_cell_conf[i]) == false){
      return false;
    }
  }

  return true;
}

static
ctrl_msg_cell_t cp_ctrl_msg_cell(ctrl_msg_cell_t const* src)
{
  assert(src != NULL);
  ctrl_msg_cell_t dst = {0};

  // List of Configuration Structures
  // [1-65535]
  assert(src->sz_ctrl_msg_cell_conf > 0 && src->sz_ctrl_msg_cell_conf < 65535);
  dst.sz_ctrl_msg_cell_conf = src->sz_ctrl_msg_cell_conf;

  dst.ctrl_msg_cell_conf = calloc(dst.sz_ctrl_msg_cell_conf, sizeof(ctrl_msg_cell_conf_t));
  assert(dst.ctrl_msg_cell_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_msg_cell_conf; ++i){
    dst.ctrl_msg_cell_conf[i] = cp_ctrl_msg_cell_conf(&src->ctrl_msg_cell_conf[i]);
  }

  return dst;
}

void free_e2sm_ccc_ctrl_msg_frmt_2( e2sm_ccc_ctrl_msg_frmt_2_t* src)
{
  assert(src != NULL);

  // List of Cells
  // [1-65535]
  assert(src->sz_ctrl_msg_cell > 0 && src->sz_ctrl_msg_cell < 65535);
  for(size_t i = 0; i < src->sz_ctrl_msg_cell ; ++i){
    free_ctrl_msg_cell(&src->ctrl_msg_cell[i]);
  }
}

bool eq_e2sm_ccc_ctrl_msg_frmt_2( e2sm_ccc_ctrl_msg_frmt_2_t const* m0,  e2sm_ccc_ctrl_msg_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ctrl_msg_cell!= m1->sz_ctrl_msg_cell)
    return false;

  assert(m0->sz_ctrl_msg_cell > 0 && m0->sz_ctrl_msg_cell < 65535);
  assert(m1->sz_ctrl_msg_cell > 0 && m1->sz_ctrl_msg_cell < 65535);
  for(size_t i = 0; i < m0->sz_ctrl_msg_cell; ++i){
    if(eq_ctrl_msg_cell(&m0->ctrl_msg_cell[i], &m1->ctrl_msg_cell[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_ctrl_msg_frmt_2_t cp_e2sm_ccc_ctrl_msg_frmt_2(e2sm_ccc_ctrl_msg_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ctrl_msg_frmt_2_t dst = {0};

  // List of Cells
  // [1-65535]
  assert(src->sz_ctrl_msg_cell > 0 && src->sz_ctrl_msg_cell < 65535);
  dst.sz_ctrl_msg_cell = src->sz_ctrl_msg_cell;

  dst.ctrl_msg_cell = calloc(dst.sz_ctrl_msg_cell, sizeof(ctrl_msg_cell_t));
  assert(dst.ctrl_msg_cell != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_msg_cell; ++i){
    dst.ctrl_msg_cell[i] = cp_ctrl_msg_cell(&src->ctrl_msg_cell[i]);
  }

  return dst;
}

