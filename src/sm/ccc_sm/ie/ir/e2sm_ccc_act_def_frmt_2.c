#include "e2sm_ccc_act_def_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

static
void free_act_def_cell_report(act_def_cell_report_t *src)
{
  assert(src != NULL);

  free_cell_global_id(&src->cell_global_id);

  // List of Cell-level RAN Configuration Structures
  // [1-1024]
  assert(src->sz_act_def_ran_conf > 0 && src->sz_act_def_ran_conf < 1025);
  for (size_t i = 0; i < src->sz_act_def_ran_conf; i++){
    free_act_def_ran_conf(src->act_def_ran_conf);
  }

  assert(src->act_def_ran_conf != NULL);
  free(src->act_def_ran_conf);
}

static
bool eq_act_def_cell_report(act_def_cell_report_t const* m0, act_def_cell_report_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(!eq_cell_global_id(&m0->cell_global_id, &m1->cell_global_id))
    return false;

  for (size_t i = 0; i < m0->sz_act_def_ran_conf; i++){
    if (!eq_act_def_ran_conf(m0->act_def_ran_conf, m1->act_def_ran_conf))
      return false;
  }

  return true;
}

static
act_def_cell_report_t cp_act_def_cell_report(act_def_cell_report_t const* src)
{
  assert(src != NULL);

  act_def_cell_report_t dst = {0};

  dst.cell_global_id = cp_cell_global_id(&src->cell_global_id);

  // List of Cell-level RAN Configuration Structures
  // [1 - 1024]
  assert(src->sz_act_def_ran_conf > 0 && src->sz_act_def_ran_conf < 1025);
  dst.sz_act_def_ran_conf = src->sz_act_def_ran_conf;

  dst.act_def_ran_conf = calloc(dst.sz_act_def_ran_conf, sizeof(act_def_ran_conf_t));
  assert(dst.act_def_ran_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_act_def_ran_conf; ++i){
    dst.act_def_ran_conf[i] = cp_act_def_ran_conf(&src->act_def_ran_conf[i]);
  }

  return dst;
}

void free_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t* src)
{
  assert(src != NULL);

  // Parameters to be Reported List
  // [1-1024]
  assert(src->sz_act_def_cell_report > 0 && src->sz_act_def_cell_report < 1025);

  for(size_t i = 0; i < src->sz_act_def_cell_report; ++i){
    free_act_def_cell_report(&src->act_def_cell_report[i]);
  }
}

bool eq_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const* m0, e2sm_ccc_act_def_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;


  if(m0->sz_act_def_cell_report != m1->sz_act_def_cell_report)
    return false;

  for(size_t i = 0; i < m0->sz_act_def_cell_report; ++i){
    if(eq_act_def_cell_report(&m0->act_def_cell_report[i], &m1->act_def_cell_report[i]) == false){
      return false;
    }
  }

  return true;
}

e2sm_ccc_act_def_frmt_2_t cp_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_act_def_frmt_2_t dst = {0};

  // Parameters to be Reported List
  // [1-1024]
  assert(src->sz_act_def_cell_report > 0 && src->sz_act_def_cell_report < 1025);
  dst.sz_act_def_cell_report = src->sz_act_def_cell_report;

  dst.act_def_cell_report = calloc(dst.sz_act_def_cell_report, sizeof(act_def_cell_report_t));
  assert(dst.act_def_cell_report != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_act_def_cell_report ; ++i){
    dst.act_def_cell_report[i] = cp_act_def_cell_report(&src->act_def_cell_report[i]);
  }

  return dst;
}
