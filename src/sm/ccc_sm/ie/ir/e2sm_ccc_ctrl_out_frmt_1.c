#include "e2sm_ccc_ctrl_out_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t* src)
{
  assert(src != NULL);

  free_byte_array(src->rev_timestamp);

  // RAN Configuration Structures Accepted List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_accepted < 65535);
  for(size_t i = 0; i < src->sz_ctrl_out_conf_accepted; ++i){
    free_ctrl_out_conf_accepted(&src->ctrl_out_conf_accepted[i]);
  }

  free(src->ctrl_out_conf_accepted);

  // RAN Configuration Structures Failed List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_failed < 65535);
  for(size_t i = 0; i < src->sz_ctrl_out_conf_failed; ++i){
    free_ctrl_out_conf_failed(&src->ctrl_out_conf_failed[i]);
  }

  free(src->ctrl_out_conf_failed);
}

bool eq_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* m0, e2sm_ccc_ctrl_out_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->rev_timestamp.buf != NULL && m1->rev_timestamp.buf != NULL)
    if(eq_byte_array(&m0->rev_timestamp, &m1->rev_timestamp) == false)
      return false;

  if(m0->sz_ctrl_out_conf_accepted != m1->sz_ctrl_out_conf_accepted)
    return false;

  assert(m0->sz_ctrl_out_conf_accepted < 65536);
  assert(m1->sz_ctrl_out_conf_accepted < 65536);
  for(size_t i = 0; i < m0->sz_ctrl_out_conf_accepted; ++i){
    if(eq_ctrl_out_conf_accepted(&m0->ctrl_out_conf_accepted[i], &m1->ctrl_out_conf_accepted[i]) == false){
      return false;
    }
  }

  if(m0->sz_ctrl_out_conf_failed!= m1->sz_ctrl_out_conf_failed)
    return false;

  assert(m0->sz_ctrl_out_conf_failed < 65536);
  assert(m1->sz_ctrl_out_conf_failed < 65536);
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
  assert(src->sz_ctrl_out_conf_accepted< 65536);
  dst.sz_ctrl_out_conf_accepted = src->sz_ctrl_out_conf_accepted;
  dst.ctrl_out_conf_accepted = calloc(dst.sz_ctrl_out_conf_accepted, sizeof(ctrl_out_conf_accepted_t));
  assert(dst.ctrl_out_conf_accepted != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_conf_accepted; ++i){
    dst.ctrl_out_conf_accepted[i] = cp_ctrl_out_conf_accepted(&src->ctrl_out_conf_accepted[i]);
  }

  // RAN Configuration Structures Failed List
  // [0-65535]
  assert(src->sz_ctrl_out_conf_failed< 65536);
  dst.sz_ctrl_out_conf_failed = src->sz_ctrl_out_conf_failed;
  dst.ctrl_out_conf_failed = calloc(dst.sz_ctrl_out_conf_failed, sizeof(ctrl_out_conf_failed_t));
  assert(dst.ctrl_out_conf_failed != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_out_conf_failed; ++i){
    dst.ctrl_out_conf_failed[i] = cp_ctrl_out_conf_failed(&src->ctrl_out_conf_failed[i]);
  }

  return dst;
}


