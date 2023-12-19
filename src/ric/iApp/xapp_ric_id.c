
#include "xapp_ric_id.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int cmp_xapp_ric_gen_id(xapp_ric_id_t const* m0,  xapp_ric_id_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(m0->xapp_id < m1->xapp_id) return 1;
  if(m0->xapp_id > m1->xapp_id) return -1;

  return cmp_ric_gen_id(&m0->ric_id, &m1->ric_id);
}

int cmp_xapp_ric_gen_id_wrapper(void const* m0, void const* m1)
{
  return  cmp_xapp_ric_gen_id(m0, m1);
}

bool eq_xapp_ric_gen_id(xapp_ric_id_t const* m0, xapp_ric_id_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  if(eq_xapp_id(m0->xapp_id, m0->xapp_id) == false)
    return false;

  return eq_ric_gen_id(&m0->ric_id, &m1->ric_id);
}


bool eq_xapp_id(uint16_t m0, uint16_t m1)
{
  if(m0 != m1)
    return false;

  return true;
}

bool eq_xapp_ric_gen_id_wrapper(void const* m0, void const* m1)
{
  return  eq_xapp_ric_gen_id(m0, m1);
}



static
bool eq_xapp_ric_gen_id2(xapp_ric_id_t const* m0,  xapp_ric_id_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  return m0->xapp_id == m1->xapp_id;
}


bool eq_xapp_id_gen_wrapper(void const* m0_v, void const* m1_v)
{
  assert(m0_v != NULL);
  assert(m1_v != NULL);
  return eq_xapp_ric_gen_id2(m0_v, m1_v);


  //uint16_t* m0 = (uint16_t*)m0_v;
  //uint16_t* m1 = (uint16_t*)m1_v;
  //return eq_xapp_id(*m0, *m1);
}

