#include "e2sm_ccc_ind_msg_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_ccc_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const* src)
{
  assert(src != NULL);

  // List of Configuration Structures Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_node_conf > 0 && src->sz_ind_msg_node_conf < 65536);

  for(size_t i = 0; i < src->sz_ind_msg_node_conf; ++i){
    free_ind_msg_ran_conf(&src->ind_msg_ran_conf[i]);
  }

  assert(src->ind_msg_ran_conf!= NULL);
  free(src->ind_msg_ran_conf);
}


bool eq_e2sm_ccc_ind_msg_frmt_1( e2sm_ccc_ind_msg_frmt_1_t const* m0,  e2sm_ccc_ind_msg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ind_msg_node_conf != m1->sz_ind_msg_node_conf)
    return false;

  for(size_t i = 0; i < m0->sz_ind_msg_node_conf; ++i){
    if(eq_ind_msg_ran_conf(&m0->ind_msg_ran_conf[i], &m1->ind_msg_ran_conf[i]) == false){
      return false;
    }
  }

  return true;
}


e2sm_ccc_ind_msg_frmt_1_t cp_e2sm_ccc_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ind_msg_frmt_1_t dst = {0};

  // List of Configuration Structures Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_node_conf > 0 && src->sz_ind_msg_node_conf < 65536);
  dst.sz_ind_msg_node_conf = src->sz_ind_msg_node_conf;

  dst.ind_msg_ran_conf = calloc(dst.sz_ind_msg_node_conf, sizeof(ind_msg_ran_conf_t));
  assert(dst.ind_msg_ran_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ind_msg_node_conf; ++i){
    dst.ind_msg_ran_conf[i] = cp_ind_msg_ran_conf(&src->ind_msg_ran_conf[i]);
  }

  return dst;
}

