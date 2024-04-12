#include <assert.h>
#include <stdlib.h>

#include "e2sm_ccc_ev_trg_frmt_1.h"

void free_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t* src)
{
  assert(src != NULL);

  // List of Node-level Configuration Structures
  // [1-256]
  assert(src->sz_ev_trg_ran_conf > 0 && src->sz_ev_trg_ran_conf < 257);
  for(size_t i = 0; i < src->sz_ev_trg_ran_conf; ++i){
    free_ev_trg_ran_conf(&src->ev_trg_ran_conf[i]);
  }

  free(src->ev_trg_ran_conf);
}

bool eq_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t const* m0, e2sm_ccc_ev_trg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ev_trg_ran_conf != m1->sz_ev_trg_ran_conf)
    return false;

  assert(m0->sz_ev_trg_ran_conf > 0 && m0->sz_ev_trg_ran_conf < 257);
  assert(m1->sz_ev_trg_ran_conf > 0 && m1->sz_ev_trg_ran_conf < 257);
  for(size_t i = 0; i < m0->sz_ev_trg_ran_conf; ++i){
    if(eq_ev_trg_ran_conf(&m0->ev_trg_ran_conf[i], &m1->ev_trg_ran_conf[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_ev_trg_frmt_1_t cp_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ev_trg_frmt_1_t dst = {0};

  // List of Node-level Configuration Structures
  // [1 - 256]
  assert(src->sz_ev_trg_ran_conf > 0 && src->sz_ev_trg_ran_conf < 257);
  dst.sz_ev_trg_ran_conf = src->sz_ev_trg_ran_conf;

  dst.ev_trg_ran_conf = calloc(dst.sz_ev_trg_ran_conf, sizeof(ev_trg_ran_conf_t));
  assert(dst.ev_trg_ran_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ev_trg_ran_conf; ++i){
    dst.ev_trg_ran_conf[i] = cp_ev_trg_ran_conf(&src->ev_trg_ran_conf[i]);
  }

  return dst;
}

