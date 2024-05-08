#include "e2sm_ccc_o_ces_management_function.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_o_ces_management_function(e2sm_ccc_o_ces_management_function_t* src)
{
  assert(src != NULL);

  //src->ces_switch
  //src->energy_saving_control
  //src->energy_saving_state
}

bool eq_e2sm_ccc_o_ces_management_function(e2sm_ccc_o_ces_management_function_t const* m0, e2sm_ccc_o_ces_management_function_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->ces_switch != m1->ces_switch)
    return false;

  if(m0->energy_saving_state != m1->energy_saving_state)
    return false;

  if(m0->energy_saving_control != m1->energy_saving_control)
    return false;

  return true;
}


e2sm_ccc_o_ces_management_function_t cp_e2sm_ccc_o_ces_management_function(e2sm_ccc_o_ces_management_function_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_ces_management_function_t dst = {0};

  dst.energy_saving_control = src->energy_saving_control;

  dst.energy_saving_state = src->energy_saving_state;

  dst.ces_switch = src->ces_switch;

  return dst;
}



