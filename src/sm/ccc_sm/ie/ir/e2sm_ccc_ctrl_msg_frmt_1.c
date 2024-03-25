#include "e2sm_ccc_ctrl_msg_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ctrl_msg_conf(ctrl_msg_conf_t * src)
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
bool eq_ctrl_msg_conf(ctrl_msg_conf_t const* m0, ctrl_msg_conf_t const* m1)
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
ctrl_msg_conf_t cp_ctrl_msg_conf(ctrl_msg_conf_t const* src)
{
  assert(src != NULL);

  ctrl_msg_conf_t dst = {0};

  assert(dst.ran_conf_name.buf != NULL);
  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  assert(dst.old_atr_val.buf != NULL);
  dst.old_atr_val= copy_byte_array(src->old_atr_val);

  assert(dst.new_atr_val.buf != NULL);
  dst.new_atr_val= copy_byte_array(src->new_atr_val);

  return dst;
}

void free_e2sm_ccc_ctrl_msg_frmt_1( e2sm_ccc_ctrl_msg_frmt_1_t* src)
{
  assert(src != NULL);

  // List of Configuration Structures
  // [1-65535]
  assert(src->sz_ctrl_msg_conf > 0 && src->sz_ctrl_msg_conf < 65535);
  for(size_t i = 0; i < src->sz_ctrl_msg_conf; ++i){
    free_ctrl_msg_conf(&src->ctrl_msg_conf[i]);
  }
}

bool eq_e2sm_ccc_ctrl_msg_frmt_1(e2sm_ccc_ctrl_msg_frmt_1_t const* m0, e2sm_ccc_ctrl_msg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ctrl_msg_conf != m1->sz_ctrl_msg_conf)
    return false;

  assert(m0->sz_ctrl_msg_conf > 0 && m0->sz_ctrl_msg_conf < 65535);
  assert(m1->sz_ctrl_msg_conf > 0 && m1->sz_ctrl_msg_conf < 65535);
  for(size_t i = 0; i < m0->sz_ctrl_msg_conf; ++i){
    if(eq_ctrl_msg_conf(&m0->ctrl_msg_conf[i], &m1->ctrl_msg_conf[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_ctrl_msg_frmt_1_t cp_e2sm_ccc_ctrl_msg_frmt_1(e2sm_ccc_ctrl_msg_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ctrl_msg_frmt_1_t dst = {0};

  // List of Configuration Structures
  // [1-65535]
  assert(src->sz_ctrl_msg_conf > 0 && src->sz_ctrl_msg_conf < 65535);
  dst.sz_ctrl_msg_conf = src->sz_ctrl_msg_conf;

  dst.ctrl_msg_conf = calloc(dst.sz_ctrl_msg_conf, sizeof(ctrl_msg_conf_t));
  assert(dst.ctrl_msg_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_msg_conf; ++i){
    dst.ctrl_msg_conf[i] = cp_ctrl_msg_conf(&src->ctrl_msg_conf[i]);
  }

  return dst;
}

