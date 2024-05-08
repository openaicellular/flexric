#include "e2sm_ccc_o_bwp.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_o_bwp(e2sm_ccc_o_bwp_t* src)
{
  assert(src != NULL);

//  src->bwp_context
//  src->cyclic_prefix
//  src->is_initial_bwp
//  src->number_of_rbs
//  src->start_rb
}

bool eq_e2sm_ccc_o_bwp(e2sm_ccc_o_bwp_t const* m0, e2sm_ccc_o_bwp_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->start_rb != m1->start_rb)
    return false;

  if(m0->number_of_rbs != m1->number_of_rbs)
    return false;

  if(m0->is_initial_bwp != m1->is_initial_bwp)
    return false;

  if(m0->cyclic_prefix != m1->cyclic_prefix)
    return false;

  if(m0->bwp_context != m1->bwp_context)
    return false;

  if(m0->sub_carrier_spacing != m1->sub_carrier_spacing)
    return false;

  return true;
}


e2sm_ccc_o_bwp_t cp_e2sm_ccc_o_bwp(e2sm_ccc_o_bwp_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_bwp_t dst = {0};

  dst.sub_carrier_spacing = src->sub_carrier_spacing;

  dst.bwp_context = src->bwp_context;

  dst.cyclic_prefix = src->cyclic_prefix;

  dst.is_initial_bwp = src->is_initial_bwp;

  dst.number_of_rbs = src->number_of_rbs;

  dst.start_rb = src->start_rb;

  return dst;
}



