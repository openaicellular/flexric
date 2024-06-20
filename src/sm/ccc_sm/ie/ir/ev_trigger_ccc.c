#include "ev_trigger_ccc.h"

#include <assert.h>
#include <stdlib.h>

static
void free_lst_ev_trigger_ccc(lst_ev_trigger_ccc_t* src)
{
  assert(src != NULL);

  // Event trigger style name
  // Mandatory
  // [1 - 150]
  assert(src->ev_trigger_style_name.len > 0 && src->ev_trigger_style_name.len < 151);
  free_byte_array(src->ev_trigger_style_name);
}

static
bool eq_lst_ev_trigger_ccc(lst_ev_trigger_ccc_t const* m0, lst_ev_trigger_ccc_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Event trigger style type
  // Mandatory
  if (m0->ev_trigger_style_type != m1->ev_trigger_style_type)
    return false;

  // Event trigger format type
  // Mandatory
  if (m0->ev_trigger_fmr_type != m1->ev_trigger_fmr_type)
    return false;

  // Event trigger style name
  // Mandatory
  // [1 - 150]
  assert(m0->ev_trigger_style_name.len > 0 && m0->ev_trigger_style_name.len < 151);
  assert(m1->ev_trigger_style_name.len > 0 && m1->ev_trigger_style_name.len < 151);
  if(eq_byte_array(&m0->ev_trigger_style_name, &m1->ev_trigger_style_name) == false)
    return false;

  return true;
}

static
lst_ev_trigger_ccc_t cp_lst_ev_trigger_ccc(lst_ev_trigger_ccc_t const* src)
{
  assert(src != NULL);

  lst_ev_trigger_ccc_t dst = {0};

  // Event trigger style type
  // Mandatory
  dst.ev_trigger_style_type = src->ev_trigger_style_type;

  // Event trigger format type
  // Mandatory
  dst.ev_trigger_fmr_type = src->ev_trigger_fmr_type;

  // Event trigger style name
  // Mandatory
  // [1 - 150]
  assert(src->ev_trigger_style_name.len > 0 && src->ev_trigger_style_name.len < 151);
  dst.ev_trigger_style_name = copy_byte_array(src->ev_trigger_style_name);

  return dst;
}

void free_ev_trigger_ccc(ev_trigger_ccc_t* src)
{
  assert(src != NULL);

  // List of supported event trigger styles
  // [1..63]
  assert(src->sz_lst_ev_trigger_ccc > 0 && src->sz_lst_ev_trigger_ccc < 64);
  for (size_t i = 0; i < src->sz_lst_ev_trigger_ccc; ++i){
    free_lst_ev_trigger_ccc(&src->lst_ev_trigger_ccc[i]);
  }
  free(src->lst_ev_trigger_ccc);
}

bool eq_ev_trigger_ccc(ev_trigger_ccc_t const* m0, ev_trigger_ccc_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // List of supported event trigger styles
  // [1..63]
  assert(m0->sz_lst_ev_trigger_ccc > 0 && m0->sz_lst_ev_trigger_ccc < 64);
  assert(m1->sz_lst_ev_trigger_ccc > 0 && m1->sz_lst_ev_trigger_ccc < 64);
  if (m0->sz_lst_ev_trigger_ccc != m1->sz_lst_ev_trigger_ccc)
    return false;

  for (size_t i = 0; i < m0->sz_lst_ev_trigger_ccc; ++i){
    if(eq_lst_ev_trigger_ccc(&m0->lst_ev_trigger_ccc[i], &m1->lst_ev_trigger_ccc[i]) == false)
      return false;
  }

  return true;
}

ev_trigger_ccc_t cp_ev_trigger_ccc(ev_trigger_ccc_t const* src)
{
  assert(src != NULL);

  ev_trigger_ccc_t dst = {0};

  // List of supported event trigger styles
  // [1..63]
  assert(src->sz_lst_ev_trigger_ccc > 0 && src->sz_lst_ev_trigger_ccc < 64);

  dst.sz_lst_ev_trigger_ccc = src->sz_lst_ev_trigger_ccc;
  dst.lst_ev_trigger_ccc = calloc(src->sz_lst_ev_trigger_ccc, sizeof(lst_ev_trigger_ccc_t));
  assert(dst.lst_ev_trigger_ccc != NULL && "Memory exhausted");

  for (size_t i = 0; i < dst.sz_lst_ev_trigger_ccc; ++i){
    dst.lst_ev_trigger_ccc[i] = cp_lst_ev_trigger_ccc(&src->lst_ev_trigger_ccc[i]);
  }

  return dst;
}


