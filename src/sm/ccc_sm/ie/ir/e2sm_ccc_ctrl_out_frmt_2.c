#include "e2sm_ccc_ctrl_out_frmt_2.h"

#include <assert.h>
#include <stdlib.h>

void free_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t* src)
{
  assert(src != NULL);
}

bool eq_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t const* m0, e2sm_ccc_ctrl_out_frmt_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return true;
}

e2sm_ccc_ctrl_out_frmt_2_t cp_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t const* src)
{
  assert(src != NULL);
  e2sm_ccc_ctrl_out_frmt_2_t dst = {};
  return dst;
}


