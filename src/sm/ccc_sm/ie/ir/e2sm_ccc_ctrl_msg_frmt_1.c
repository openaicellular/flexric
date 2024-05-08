#include "e2sm_ccc_ctrl_msg_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_ccc_ctrl_msg_frmt_1( e2sm_ccc_ctrl_msg_frmt_1_t* src)
{
  assert(src != NULL);

  // List of Configuration Structures
  // [1-65535]
  assert(src->sz_ctrl_msg_ran_conf > 0 && src->sz_ctrl_msg_ran_conf < 65536);
  for(size_t i = 0; i < src->sz_ctrl_msg_ran_conf; ++i){
    free_ctrl_msg_ran_conf(&src->ctrl_msg_ran_conf[i]);
  }

  free(src->ctrl_msg_ran_conf);
}

bool eq_e2sm_ccc_ctrl_msg_frmt_1(e2sm_ccc_ctrl_msg_frmt_1_t const* m0, e2sm_ccc_ctrl_msg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ctrl_msg_ran_conf != m1->sz_ctrl_msg_ran_conf)
    return false;

  assert(m0->sz_ctrl_msg_ran_conf > 0 && m0->sz_ctrl_msg_ran_conf < 65536);
  assert(m1->sz_ctrl_msg_ran_conf > 0 && m1->sz_ctrl_msg_ran_conf < 65536);
  for(size_t i = 0; i < m0->sz_ctrl_msg_ran_conf; ++i){
    if(eq_ctrl_msg_ran_conf(&m0->ctrl_msg_ran_conf[i], &m1->ctrl_msg_ran_conf[i]) == false){
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
  assert(src->sz_ctrl_msg_ran_conf > 0 && src->sz_ctrl_msg_ran_conf < 65536);
  dst.sz_ctrl_msg_ran_conf = src->sz_ctrl_msg_ran_conf;

  dst.ctrl_msg_ran_conf = calloc(dst.sz_ctrl_msg_ran_conf, sizeof(ctrl_msg_ran_conf_t));
  assert(dst.ctrl_msg_ran_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ctrl_msg_ran_conf; ++i){
    dst.ctrl_msg_ran_conf[i] = cp_ctrl_msg_ran_conf(&src->ctrl_msg_ran_conf[i]);
  }

  return dst;
}

