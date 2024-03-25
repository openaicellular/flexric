#include "e2sm_ccc_ctrl_out_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

void free_ctrl_out_cell(ctrl_out_cell_t * src)
{
  assert(src != NULL);

  free_cell_global_id(&src->cell_global_id);

  // RAN Configuration Structures Accepted List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_accepted < 65535);
  for(size_t i = 0; i < src->sz_ctrl_out_conf_accepted; ++i){
    free_ctrl_out_conf_accepted(&src->ctrl_out_conf_accepted[i]);
  }

  // RAN Configuration Structures Failed List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_failed < 65535);
  for(size_t i = 0; i < src->sz_ctrl_out_conf_failed; ++i){
    free_ctrl_out_conf_failed(&src->ctrl_out_conf_failed[i]);
  }
}

bool eq_ctrl_out_cell(ctrl_out_cell_t const* m0, ctrl_out_cell_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id) == false)
    return false;

  if(m0->sz_ctrl_out_conf_accepted!= m1->sz_ctrl_out_conf_accepted)
    return false;

  assert(m0->sz_ctrl_out_conf_accepted < 65535);
  assert(m1->sz_ctrl_out_conf_accepted < 65535);
  for(size_t i = 0; i < m0->sz_ctrl_out_conf_accepted; ++i){
    if(eq_ctrl_out_conf_accepted(&m0->ctrl_out_conf_accepted[i], &m1->ctrl_out_conf_accepted[i]) == false){
      return false;
    }
  }

  if(m0->sz_ctrl_out_conf_failed!= m1->sz_ctrl_out_conf_failed)
    return false;

  assert(m0->sz_ctrl_out_conf_failed < 65535);
  assert(m1->sz_ctrl_out_conf_failed < 65535);
  for(size_t i = 0; i < m0->sz_ctrl_out_conf_failed; ++i){
    if(eq_ctrl_out_conf_failed(&m0->ctrl_out_conf_failed[i], &m1->ctrl_out_conf_failed[i]) == false){
      return false;
    }
  }

  return true;
}

ctrl_out_cell_t cp_ctrl_out_cell(ctrl_out_cell_t const* src)
{
  assert(src != NULL);
  ctrl_out_cell_t dst = {0};

  dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  // RAN Configuration Structures Accepted List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_accepted < 65535);
  dst.sz_ctrl_out_conf_accepted = src->sz_ctrl_out_conf_accepted;

  dst.ctrl_out_conf_accepted = calloc(dst.sz_ctrl_out_conf_accepted, sizeof(ctrl_out_conf_accepted_t));
  assert(dst.ctrl_out_conf_accepted != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_conf_accepted; ++i){
    dst.ctrl_out_conf_accepted[i] = cp_ctrl_out_conf_accepted(&src->ctrl_out_conf_accepted[i]);
  }

  // RAN Configuration Structures Failed List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_failed < 65535);
  dst.sz_ctrl_out_conf_failed = src->sz_ctrl_out_conf_failed;

  dst.ctrl_out_conf_failed = calloc(dst.sz_ctrl_out_conf_failed, sizeof(ctrl_out_conf_failed_t));
  assert(dst.ctrl_out_conf_failed != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_conf_failed; ++i){
    dst.ctrl_out_conf_failed[i] = cp_ctrl_out_conf_failed(&src->ctrl_out_conf_failed[i]);
  }

  return dst;
}

void free_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t* src)
{
  assert(src != NULL);

  if (src->rev_timestamp.buf != NULL)
    free_byte_array(src->rev_timestamp);

  // List of cells
  // [1-65535]
  assert(src->sz_ctrl_out_cell > 0 && src->sz_ctrl_out_cell < 65535);
  for(size_t i = 0; i < src->sz_ctrl_out_cell; ++i){
    free_ctrl_out_cell(&src->ctrl_out_cell[i]);
  }
}

bool eq_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t const* m0, e2sm_ccc_ctrl_out_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ctrl_out_cell != m1->sz_ctrl_out_cell)
    return false;

  assert(m0->sz_ctrl_out_cell > 0 && m0->sz_ctrl_out_cell < 65535);
  assert(m1->sz_ctrl_out_cell > 0 && m1->sz_ctrl_out_cell < 65535);
  for(size_t i = 0; i < m0->sz_ctrl_out_cell; ++i){
    if(eq_ctrl_out_cell(&m0->ctrl_out_cell[i], &m1->ctrl_out_cell[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_ctrl_out_frmt_2_t cp_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ctrl_out_frmt_2_t dst = {0};

  if (src->rev_timestamp.buf != NULL)
    dst.rev_timestamp = copy_byte_array(src->rev_timestamp);

  // List of cells
  // [1-65535]
  assert(src->sz_ctrl_out_cell > 0 && src->sz_ctrl_out_cell < 65535);
  dst.sz_ctrl_out_cell = src->sz_ctrl_out_cell;

  dst.ctrl_out_cell = calloc(dst.ctrl_out_cell, sizeof(ctrl_out_cell_t));
  assert(dst.ctrl_out_cell != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_cell; ++i){
    dst.ctrl_out_cell[i] = cp_ctrl_out_cell(&src->ctrl_out_cell[i]);
  }

  return dst;
}


