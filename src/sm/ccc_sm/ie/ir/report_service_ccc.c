#include "report_service_ccc.h"

#include <assert.h>
#include <stdlib.h>

static
void free_lst_report_service_ccc(lst_report_service_ccc_t* src)
{
  assert(src != NULL);

  // Event trigger style name
  // Mandatory
  // [1 - 150]
  assert(src->report_service_style_name.len > 0 && src->report_service_style_name.len < 151);
  free_byte_array(src->report_service_style_name);

  // List of Supported Event Trigger Styles
  // [1..63]
  assert(src->sz_lst_sp_ev_trigger_styles_report_service > 0 && src->sz_lst_sp_ev_trigger_styles_report_service < 64);
  free(src->lst_sp_ev_trigger_styles_report_service);
}

static
bool eq_lst_report_service_ccc(lst_report_service_ccc_t const* m0, lst_report_service_ccc_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Report service style type
  // Mandatory
  if (m0->report_service_style_type != m1->report_service_style_type)
    return false;

  // Report Service Action Definition Format Type
  // Mandatory
  if (m0->report_service_action_def_fmr_type != m1->report_service_action_def_fmr_type)
    return false;

  // Report Service Indication Header Format Type
  // Mandatory
  if (m0->report_service_ind_header_fmr_type != m1->report_service_ind_header_fmr_type)
    return false;

  // Report Service Indication Message Format Type
  // Mandatory
  if (m0->report_service_ind_message_fmr_type != m1->report_service_ind_message_fmr_type)
    return false;

  // Report service style name
  // Mandatory
  // [1 - 150]
  assert(m0->report_service_style_name.len > 0 && m0->report_service_style_name.len < 151);
  assert(m1->report_service_style_name.len > 0 && m1->report_service_style_name.len < 151);
  if(eq_byte_array(&m0->report_service_style_name, &m1->report_service_style_name) == false)
    return false;

  // List of Supported Event Trigger Styles
  // Mandatory
  // [1 - 63]
  assert(m0->sz_lst_sp_ev_trigger_styles_report_service > 0 && m0->sz_lst_sp_ev_trigger_styles_report_service < 64);
  assert(m1->sz_lst_sp_ev_trigger_styles_report_service > 0 && m1->sz_lst_sp_ev_trigger_styles_report_service < 64);
  if (m0->sz_lst_sp_ev_trigger_styles_report_service != m1->sz_lst_sp_ev_trigger_styles_report_service)
    return false;

  for (size_t i = 0; i < m0->sz_lst_sp_ev_trigger_styles_report_service; ++i){
    // Event trigger style type
    // Mandatory
    if(m0->lst_sp_ev_trigger_styles_report_service[i].ev_trigger_style_type !=  m1->lst_sp_ev_trigger_styles_report_service[i].ev_trigger_style_type)
      return false;
  }

  return true;
}

static
lst_report_service_ccc_t cp_lst_report_service_ccc(lst_report_service_ccc_t const* src)
{
  assert(src != NULL);

  lst_report_service_ccc_t dst = {0};

  // Report service style type
  // Mandatory
  dst.report_service_style_type = src->report_service_style_type;

  // Report Service Action Definition Format Type
  // Mandatory
  dst.report_service_action_def_fmr_type = src->report_service_action_def_fmr_type;

  // Report Service Indication Header Format Type
  // Mandatory
  dst.report_service_ind_header_fmr_type = src->report_service_ind_header_fmr_type;

  // Report Service Indication Message Format Type
  // Mandatory
  dst.report_service_ind_message_fmr_type = src->report_service_ind_message_fmr_type;

  // Report service style name
  // Mandatory
  // [1 - 150]
  assert(src->report_service_style_name.len > 0 && src->report_service_style_name.len < 151);
  dst.report_service_style_name = copy_byte_array(src->report_service_style_name);

  // List of Supported Event Trigger Styles
  // Mandatory
  // [1..63]
  assert(src->sz_lst_sp_ev_trigger_styles_report_service > 0 && src->sz_lst_sp_ev_trigger_styles_report_service < 64);
  dst.sz_lst_sp_ev_trigger_styles_report_service= src->sz_lst_sp_ev_trigger_styles_report_service;
  dst.lst_sp_ev_trigger_styles_report_service = calloc(src->sz_lst_sp_ev_trigger_styles_report_service, sizeof(lst_sp_ev_trigger_styles_report_service_t));
  assert(dst.lst_sp_ev_trigger_styles_report_service != NULL && "Memory exhausted");

  for (size_t i = 0; i < dst.sz_lst_sp_ev_trigger_styles_report_service; ++i){
    // Event trigger style type
    // Mandatory
    dst.lst_sp_ev_trigger_styles_report_service[i].ev_trigger_style_type = src->lst_sp_ev_trigger_styles_report_service[i].ev_trigger_style_type;
  }

  return dst;
}

void free_report_service_ccc(report_service_ccc_t* src)
{
  assert(src != NULL);

  // List of supported event trigger styles
  // [1..63]
  assert(src->sz_lst_report_service_ccc > 0 && src->sz_lst_report_service_ccc < 64);
  for (size_t i = 0; i < src->sz_lst_report_service_ccc; ++i){
    free_lst_report_service_ccc(&src->lst_report_service_ccc[i]);
  }
  free(src->lst_report_service_ccc);
}

bool eq_report_service_ccc(report_service_ccc_t const* m0, report_service_ccc_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // List of supported event trigger styles
  // [1..63]
  assert(m0->sz_lst_report_service_ccc > 0 && m0->sz_lst_report_service_ccc < 64);
  assert(m1->sz_lst_report_service_ccc > 0 && m1->sz_lst_report_service_ccc < 64);
  if (m0->sz_lst_report_service_ccc != m1->sz_lst_report_service_ccc)
    return false;

  for (size_t i = 0; i < m0->sz_lst_report_service_ccc; ++i){
    if(eq_lst_report_service_ccc(&m0->lst_report_service_ccc[i], &m1->lst_report_service_ccc[i]) == false)
      return false;
  }

  return true;
}

report_service_ccc_t cp_report_service_ccc(report_service_ccc_t const* src)
{
  assert(src != NULL);

  report_service_ccc_t dst = {0};

  // List of supported event trigger styles
  // [1..63]
  assert(src->sz_lst_report_service_ccc > 0 && src->sz_lst_report_service_ccc < 64);

  dst.sz_lst_report_service_ccc = src->sz_lst_report_service_ccc;
  dst.lst_report_service_ccc = calloc(src->sz_lst_report_service_ccc, sizeof(lst_report_service_ccc_t));
  assert(dst.lst_report_service_ccc != NULL && "Memory exhausted");

  for (size_t i = 0; i < dst.sz_lst_report_service_ccc; ++i){
    dst.lst_report_service_ccc[i] = cp_lst_report_service_ccc(&src->lst_report_service_ccc[i]);
  }

  return dst;
}


