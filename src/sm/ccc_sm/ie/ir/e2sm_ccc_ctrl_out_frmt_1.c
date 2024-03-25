#include "e2sm_ccc_ctrl_out_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t * src)
{
  assert(src != NULL);

  assert(src->ran_conf_name.buf != NULL);
  free_byte_array(src->ran_conf_name);

  assert(src->old_atr_val.buf != NULL);
  free_byte_array(src->old_atr_val);

  assert(src->cur_atr_val.buf != NULL);
  free_byte_array(src->cur_atr_val);

  if(src->app_timestamp.buf != NULL)
    free_byte_array(src->app_timestamp);
}

static
bool eq_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t const* m0, ctrl_out_conf_accepted_t const* m1)
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

  assert(m0->cur_atr_val.buf != NULL);
  assert(m1->cur_atr_val.buf != NULL);
  if(!eq_byte_array(&m0->cur_atr_val, &m1->cur_atr_val))
    return false;

  return true;
}

static
ctrl_out_conf_accepted_t cp_ctrl_out_conf_accepted(ctrl_out_conf_accepted_t const* src)
{
  assert(src != NULL);

  ctrl_out_conf_accepted_t  dst = {0};

  assert(src->ran_conf_name.buf != NULL);
  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  assert(src->old_atr_val.buf != NULL);
  dst.old_atr_val= copy_byte_array(src->old_atr_val);

  assert(src->cur_atr_val.buf != NULL);
  dst.cur_atr_val= copy_byte_array(src->cur_atr_val);

  if (src->app_timestamp.buf != NULL)
    dst.app_timestamp = copy_byte_array(src->app_timestamp);

  return dst;
}

static
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

  assert(src->ran_conf_name.buf != NULL);
  free_byte_array(src->ran_conf_name);

  assert(src->old_atr_val.buf != NULL);
  free_byte_array(src->old_atr_val);

  assert(src->req_atr_val.buf != NULL);
  free_byte_array(src->req_atr_val);
}

static
bool eq_ctrl_out_conf_failed(ctrl_out_conf_failed_t const* m0, ctrl_out_conf_failed_t const* m1)
{
  // TODO Assert cause
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

  assert(m0->req_atr_val.buf != NULL);
  assert(m1->req_atr_val.buf != NULL);
  if(!eq_byte_array(&m0->req_atr_val, &m1->req_atr_val))
    return false;

  if (m0->cause != m1->cause)
    return false;

  return true;
}

static
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

  ctrl_out_conf_failed_t dst = {0};

  assert(src->ran_conf_name.buf != NULL);
  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  assert(src->old_atr_val.buf != NULL);
  dst.old_atr_val= copy_byte_array(src->old_atr_val);

  assert(src->req_atr_val.buf != NULL);
  dst.req_atr_val = copy_byte_array(src->req_atr_val);

  dst.cause = src->cause;

  return dst;
}

void free_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t* src)
{
  assert(src != NULL);

  if (src->rev_timestamp.buf != NULL)
    free_byte_array(src->rev_timestamp);

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

bool eq_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* m0, e2sm_ccc_ctrl_out_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
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

e2sm_ccc_ctrl_out_frmt_1_t cp_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ctrl_out_frmt_1_t dst = {0};

  if (src->rev_timestamp.buf != NULL)
    dst.rev_timestamp = copy_byte_array(src->rev_timestamp);

  // RAN Configuration Structures Accepted List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_accepted< 65535);
  dst.sz_ctrl_out_conf_accepted = src->sz_ctrl_out_conf_accepted;

  dst.ctrl_out_conf_accepted = calloc(dst.sz_ctrl_out_conf_accepted, sizeof(ctrl_out_conf_accepted_t));
  assert(dst.ctrl_out_conf_accepted != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_conf_accepted; ++i){
    dst.ctrl_out_conf_accepted[i] = cp_ctrl_out_conf_accepted(&src->ctrl_out_conf_accepted[i]);
  }

  // RAN Configuration Structures Failed List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_failed< 65535);
  dst.sz_ctrl_out_conf_failed = src->sz_ctrl_out_conf_failed;

  dst.ctrl_out_conf_failed = calloc(dst.sz_ctrl_out_conf_failed, sizeof(ctrl_out_conf_failed_t));
  assert(dst.ctrl_out_conf_failed != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_conf_failed; ++i){
    dst.ctrl_out_conf_failed[i] = cp_ctrl_out_conf_failed(&src->ctrl_out_conf_failed[i]);
  }

  return dst;
}


