#include "e2sm_ccc_ind_msg_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ind_msg_cell_report(ind_msg_cell_report_t* src)
{
  assert(src != NULL);

  free_cell_global_id(&src->cell_global_id);

  // List of Configuration Structures Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_ran_conf > 0 && src->sz_ind_msg_ran_conf < 65536);

  for(size_t i = 0; i < src->sz_ind_msg_ran_conf; ++i){
    free_ind_msg_ran_conf(&src->ind_msg_ran_conf[i]);
  }

  free(src->ind_msg_ran_conf);
}

static
bool eq_ind_msg_cell_report(ind_msg_cell_report_t const* m0,  ind_msg_cell_report_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id) == false)
    return false;

  if(m0->sz_ind_msg_ran_conf != m1->sz_ind_msg_ran_conf)
    return false;

  for(size_t i = 0; i < m0->sz_ind_msg_ran_conf; ++i){
    if(eq_ind_msg_ran_conf(&m0->ind_msg_ran_conf[i], &m1->ind_msg_ran_conf[i]) == false){
      return false;
    }
  }

  return true;
}

static
ind_msg_cell_report_t cp_ind_msg_cell_report(ind_msg_cell_report_t const* src)
{
  assert(src != NULL);

  ind_msg_cell_report_t dst = {0};

  dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  // List of Cells Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_ran_conf > 0 && src->sz_ind_msg_ran_conf < 65536);
  dst.sz_ind_msg_ran_conf = src->sz_ind_msg_ran_conf;

  dst.ind_msg_ran_conf = calloc(dst.sz_ind_msg_ran_conf, sizeof(ind_msg_ran_conf_t));
  assert(dst.ind_msg_ran_conf!= NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ind_msg_ran_conf; ++i){
    dst.ind_msg_ran_conf[i] = cp_ind_msg_ran_conf(&src->ind_msg_ran_conf[i]);
  }

  return dst;
}

void free_e2sm_ccc_ind_msg_frmt_2(e2sm_ccc_ind_msg_frmt_2_t const* src)
{
  assert(src != NULL);

  // List of Cells Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_cell_report > 0 && src->sz_ind_msg_cell_report < 65536);

  for(size_t i = 0; i < src->sz_ind_msg_cell_report; ++i){
    free_ind_msg_cell_report(&src->ind_msg_cell_report[i]);
  }

  free(src->ind_msg_cell_report);
}

bool eq_e2sm_ccc_ind_msg_frmt_2( e2sm_ccc_ind_msg_frmt_2_t const* m0,  e2sm_ccc_ind_msg_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ind_msg_cell_report != m1->sz_ind_msg_cell_report)
    return false;

  for(size_t i = 0; i < m0->sz_ind_msg_cell_report; ++i){
    if(eq_ind_msg_cell_report(&m0->ind_msg_cell_report[i], &m1->ind_msg_cell_report[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_ind_msg_frmt_2_t cp_e2sm_ccc_ind_msg_frmt_2(e2sm_ccc_ind_msg_frmt_2_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ind_msg_frmt_2_t dst = {0};

  // List of Cells Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_cell_report > 0 && src->sz_ind_msg_cell_report < 65536);
  dst.sz_ind_msg_cell_report = src->sz_ind_msg_cell_report;

  dst.ind_msg_cell_report = calloc(dst.sz_ind_msg_cell_report, sizeof(ind_msg_cell_report_t));
  assert(dst.ind_msg_cell_report!= NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ind_msg_cell_report; ++i){
    dst.ind_msg_cell_report[i] = cp_ind_msg_cell_report(&src->ind_msg_cell_report[i]);
  }

  return dst;
}


