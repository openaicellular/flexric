#include "e2sm_ccc_act_def_frmt_1.h"

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

void free_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t* src)
{
  assert(src != NULL);

  // Parameters to be Reported List
  // [1-256]
  assert(src->sz_act_def_ran_conf > 0 && src->sz_act_def_ran_conf < 257);

  for(size_t i = 0; i < src->sz_act_def_ran_conf; ++i){
    free_act_def_ran_conf(&src->act_def_ran_conf[i]);
  }

  assert(src->act_def_ran_conf != NULL);
  free(src->act_def_ran_conf);
}


bool eq_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t const* m0, e2sm_ccc_act_def_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_act_def_ran_conf != m1->sz_act_def_ran_conf)
    return false;

  for(size_t i = 0; i < m0->sz_act_def_ran_conf; ++i){
    if(eq_act_def_ran_conf(&m0->act_def_ran_conf[i], &m1->act_def_ran_conf[i]) == false){
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
  assert(src->sz_act_def_ran_conf > 0 && src->sz_act_def_ran_conf < 257);
  dst.sz_act_def_ran_conf = src->sz_act_def_ran_conf;

  dst.act_def_ran_conf = calloc(dst.sz_act_def_ran_conf, sizeof(act_def_ran_conf_t));
  assert(dst.act_def_ran_conf!= NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_act_def_ran_conf; ++i){
    dst.act_def_ran_conf[i] = cp_act_def_ran_conf(&src->act_def_ran_conf[i]);
  }

  return dst;
}

