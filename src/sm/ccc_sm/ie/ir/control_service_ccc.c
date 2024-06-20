#include "control_service_ccc.h"

#include <assert.h>
#include <stdlib.h>

static
void free_lst_control_service_ccc(lst_control_service_ccc_t* src)
{
  assert(src != NULL);

  // Control service style name
  // Mandatory
  // [1 - 150]
  assert(src->control_service_style_name.len > 0 && src->control_service_style_name.len < 151);
  free_byte_array(src->control_service_style_name);
}

static
bool eq_lst_control_service_ccc(lst_control_service_ccc_t const* m0, lst_control_service_ccc_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Control service style type
  // Mandatory
  if (m0->control_service_style_type != m1->control_service_style_type)
    return false;

  // Control Service Header Format Type
  // Mandatory
  if (m0->control_service_header_fmr_type != m1->control_service_header_fmr_type)
    return false;

  // Control Service Message Format Type
  // Mandatory
  if (m0->control_service_message_fmr_type != m1->control_service_message_fmr_type)
    return false;

  // Control Service Control Outcome Format Type
  // Mandatory
  if (m0->control_service_control_outcome_fmr_type != m1->control_service_control_outcome_fmr_type)
    return false;

  // RIC Call Process ID Format Type
  // Optional
  //  if (m0->ric_call_process_id_fmr_type != m1->ric_call_process_id_fmr_type)
  //    return false;

  // Control service style name
  // Mandatory
  // [1 - 150]
  assert(m0->control_service_style_name.len > 0 && m0->control_service_style_name.len < 151);
  assert(m1->control_service_style_name.len > 0 && m1->control_service_style_name.len < 151);
  if(eq_byte_array(&m0->control_service_style_name, &m1->control_service_style_name) == false)
    return false;

  return true;
}

static
lst_control_service_ccc_t cp_lst_control_service_ccc(lst_control_service_ccc_t const* src)
{
  assert(src != NULL);

  lst_control_service_ccc_t dst = {0};

  // Control service style type
  // Mandatory
  dst.control_service_style_type = src->control_service_style_type;

  // Control Service Header Format Type
  // Mandatory
  dst.control_service_header_fmr_type = src->control_service_header_fmr_type;

  // Control Service Message Format Type
  // Mandatory
  dst.control_service_message_fmr_type = src->control_service_message_fmr_type;

  // Control Service Control Outcome Format Type
  // Mandatory
  dst.control_service_control_outcome_fmr_type = src->control_service_control_outcome_fmr_type;

  // RIC Call Process ID Format Type
  // Optional
  dst.ric_call_process_id_fmr_type = src->ric_call_process_id_fmr_type;

  // Control service style name
  // Mandatory
  // [1 - 150]
  assert(src->control_service_style_name.len > 0 && src->control_service_style_name.len < 151);
  dst.control_service_style_name = copy_byte_array(src->control_service_style_name);

  return dst;
}

void free_control_service_ccc(control_service_ccc_t* src)
{
  assert(src != NULL);

  // List of supported Control service styles
  // [1..63]
  assert(src->sz_lst_control_service_ccc > 0 && src->sz_lst_control_service_ccc < 64);
  for (size_t i = 0; i < src->sz_lst_control_service_ccc; ++i){
    free_lst_control_service_ccc(&src->lst_control_service_ccc[i]);
  }
  free(src->lst_control_service_ccc);
}

bool eq_control_service_ccc(control_service_ccc_t const* m0, control_service_ccc_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // List of supported Control service styles
  // [1..63]
  assert(m0->sz_lst_control_service_ccc > 0 && m0->sz_lst_control_service_ccc < 64);
  assert(m1->sz_lst_control_service_ccc > 0 && m1->sz_lst_control_service_ccc < 64);
  if (m0->sz_lst_control_service_ccc != m1->sz_lst_control_service_ccc)
    return false;

  for (size_t i = 0; i < m0->sz_lst_control_service_ccc; ++i){
    if(eq_lst_control_service_ccc(&m0->lst_control_service_ccc[i], &m1->lst_control_service_ccc[i]) == false)
      return false;
  }

  return true;
}

control_service_ccc_t cp_control_service_ccc(control_service_ccc_t const* src)
{
  assert(src != NULL);

  control_service_ccc_t dst = {0};

  // List of supported Control service styles
  // [1..63]
  assert(src->sz_lst_control_service_ccc > 0 && src->sz_lst_control_service_ccc < 64);

  dst.sz_lst_control_service_ccc = src->sz_lst_control_service_ccc;
  dst.lst_control_service_ccc = calloc(src->sz_lst_control_service_ccc, sizeof(lst_control_service_ccc_t));
  assert(dst.lst_control_service_ccc != NULL && "Memory exhausted");

  for (size_t i = 0; i < dst.sz_lst_control_service_ccc; ++i){
    dst.lst_control_service_ccc[i] = cp_lst_control_service_ccc(&src->lst_control_service_ccc[i]);
  }

  return dst;
}


