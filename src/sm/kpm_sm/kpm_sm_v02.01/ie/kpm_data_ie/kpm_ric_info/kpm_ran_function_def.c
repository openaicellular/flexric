#include <assert.h>

#include "kpm_ran_function_def.h"


void free_ric_event_trigger_style_list(ric_event_trigger_style_item_t* src)
{
  assert(src != NULL);

  // 8.3.3
  // src->style_type;
  // 8.3.4
  free_byte_array(src->style_name);
  // 8.3.5
  // src-> format_type;

}

bool eq_ric_event_trigger_style_list(ric_event_trigger_style_item_t const* m0, ric_event_trigger_style_item_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->style_type != m1->style_type)
    return false;

  if(eq_byte_array(&m0->style_name, &m1->style_name) == false)
    return false;

  if(m0->format_type != m1->format_type)
    return false;

  return true;
}

ric_event_trigger_style_item_t cp_ric_event_trigger_style_list(ric_event_trigger_style_item_t const* src)
{
  assert(src != NULL);
 
  ric_event_trigger_style_item_t dst = {0}; 

  // 8.3.3
  dst.style_type = src->style_type;
  // 8.3.4
  dst.style_name = copy_byte_array(src->style_name);
  // 8.3.5
  dst. format_type = src-> format_type;

  return dst;
}


void free_meas_info_for_action_lst(meas_info_for_action_lst_t* src)
{
  assert(src != NULL);

  // 8.3.9
  free_byte_array(src->name);

  // 8.3.10  -  OPTIONAL
  if(src->id != NULL)
    free(src->id);

  // 8.3.26  -  OPTIONAL; not yet implemented in ASN.1
  if(src->bin_range_def != NULL){
    free_kpm_bin_range_def(src->bin_range_def); 
    free(src->bin_range_def );
  }

}

bool eq_meas_info_for_action_lst(meas_info_for_action_lst_t const* m0,meas_info_for_action_lst_t const* m1)
{
  if(m0 == m1)
    return true;

  // 8.3.9
  if(eq_byte_array(&m0->name, &m1->name) == false)
    return false;

  // 8.3.10  -  OPTIONAL
  if(m0->id == NULL && m1->id != NULL)
    return false;

  if(m0->id != NULL && m1->id == NULL)
    return false;

  if(m0->id != NULL && m1->id != NULL){
    if(m0->id != m1->id)
      return false;
  }

  // 8.3.26  -  OPTIONAL; not yet implemented in ASN.1
  if(m0->bin_range_def == NULL && m1->bin_range_def != NULL)
    return false;

  if(m0->bin_range_def != NULL && m1->bin_range_def == NULL)
    return false;

  if(m0->bin_range_def != NULL && m1->bin_range_def != NULL){
    if(eq_kpm_bin_range_def(m0->bin_range_def, m1->bin_range_def) == false)
      return false;
  }

  return true;
}

meas_info_for_action_lst_t cp_meas_info_for_action_lst(meas_info_for_action_lst_t const* src)
{
  assert(src != NULL);
  meas_info_for_action_lst_t dst = {0}; 
  // 8.3.9
  dst.name = copy_byte_array(src->name);
// 8.3.10  -  OPTIONAL
  if(src->id != NULL){
    dst.id = calloc(1, sizeof(uint16_t  ));
    assert(dst.id != NULL && "Memory exhausted");
    *dst.id = *src->id; 
  }

  // 8.3.26  -  OPTIONAL; not yet implemented in ASN.1
  if(src-> bin_range_def != NULL){
    dst.bin_range_def = calloc(1, sizeof(bin_range_def_t));
    assert(dst.bin_range_def != NULL && "Memory exhausted" );
    *dst.bin_range_def = cp_kpm_bin_range_def(src->bin_range_def);
  }

  return dst;
}

void free_ric_report_style_item( ric_report_style_item_t const* src)
{
  assert(src != NULL);

 // ric_service_report_e report_style_type;  // 8.3.3
 free_byte_array(src->report_style_name);  // 8.3.4
 //format_action_def_e act_def_format_type;  // 8.3.5

 // [1, 65535]
 assert(src-> meas_info_for_action_lst_len> 0 && src-> meas_info_for_action_lst_len <  65536);
 assert(src->meas_info_for_action_lst != NULL);

 for(size_t i = 0; i < src-> meas_info_for_action_lst_len; ++i){
  free_meas_info_for_action_lst(&src->meas_info_for_action_lst[i] );
 }
 free(src->meas_info_for_action_lst);

 //format_ind_hdr_e ind_hdr_format_type;  // 8.3.5
 //format_ind_msg_e ind_msg_format_type;  // 8.3.5

}


bool eq_ric_report_style_item(ric_report_style_item_t const* m0, ric_report_style_item_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // 8.3.3
  if(m0->report_style_type != m1->report_style_type)
    return false;

  // 8.3.4
  if(eq_byte_array(&m0->report_style_name, &m1->report_style_name) == false)
    return false;

  // 8.3.5
  if(m0->act_def_format_type != m1->act_def_format_type)
    return false;

  // [1, 65535]
  assert(m0->meas_info_for_action_lst_len > 0 && m0->meas_info_for_action_lst_len <  65536);
  assert(m1->meas_info_for_action_lst_len > 0 && m1->meas_info_for_action_lst_len <  65536);

  if(m0->meas_info_for_action_lst_len != m1->meas_info_for_action_lst_len  )
    return false;

  for(size_t i =0; i < m0->meas_info_for_action_lst_len; ++i){
    if(eq_meas_info_for_action_lst(&m0->meas_info_for_action_lst[i], &m1->meas_info_for_action_lst[i]) == false)
      return false;
  }

  // 8.3.5
  if(m0->ind_hdr_format_type != m1->ind_hdr_format_type)
    return false;

  // 8.3.5
  if(m0->ind_msg_format_type != m1->ind_msg_format_type   )
    return false;

  return true;

}

ric_report_style_item_t cp_ric_report_style_item(ric_report_style_item_t const* src)
{
  assert(src != NULL);

  ric_report_style_item_t dst = {0}; 

  dst.report_style_type = src->report_style_type ;  // 8.3.3
  dst.report_style_name = copy_byte_array(src->report_style_name);  // 8.3.4
  dst.act_def_format_type = src->act_def_format_type ;  // 8.3.5

  // [1, 65535]
  assert(src->meas_info_for_action_lst_len > 0 && src-> meas_info_for_action_lst_len < 65536);
  dst.meas_info_for_action_lst_len = src->meas_info_for_action_lst_len ;
  dst.meas_info_for_action_lst = calloc(dst.meas_info_for_action_lst_len, sizeof(meas_info_for_action_lst_t));
  assert(dst.meas_info_for_action_lst != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->meas_info_for_action_lst_len; ++i){
    dst.meas_info_for_action_lst[i] = cp_meas_info_for_action_lst(&src->meas_info_for_action_lst[i]);  
  }

  dst.ind_hdr_format_type = src->ind_hdr_format_type;  // 8.3.5
  dst.ind_msg_format_type = src->ind_msg_format_type;  // 8.3.5

  return dst;
}





void free_kpm_ran_function_def(kpm_ran_function_def_t* src)
{
  assert(src != NULL);

  // RAN Function Name
  free_ran_function_name(&src->name);

  // RIC Event Trigger Style List
  if (src->ric_event_trigger_style_list != NULL) {
    assert(src->sz_ric_event_trigger_style_list > 0);

    for (size_t i = 0; i<src->sz_ric_event_trigger_style_list; i++)
      free_ric_event_trigger_style_list(&src->ric_event_trigger_style_list[i]);

    free(src->ric_event_trigger_style_list);
  }

  // RIC Report Style List
  if (src->ric_report_style_list != NULL ) {
    assert(src->sz_ric_report_style_list != 0);

    for (size_t i = 0; i<src->sz_ric_report_style_list; i++) 
      free_ric_report_style_item(&src->ric_report_style_list[i]);

    free(src->ric_report_style_list);
  }

}

bool eq_kpm_ran_function_def(kpm_ran_function_def_t const * m0, kpm_ran_function_def_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // RAN Function Name
  if (eq_ran_function_name(&m0->name, &m1->name) != true)
    return false;

  // RIC Event Trigger Style List
  if (m0->sz_ric_event_trigger_style_list != m1->sz_ric_event_trigger_style_list)
    return false;

  if(m0->sz_ric_event_trigger_style_list > 0){
    assert(m0->ric_event_trigger_style_list != NULL);
    assert(m1->ric_event_trigger_style_list != NULL);
  }

  for (size_t i = 0; i < m0->sz_ric_event_trigger_style_list; i++) {
    if (eq_ric_event_trigger_style_list(&m0->ric_event_trigger_style_list[i], &m1->ric_event_trigger_style_list[i]) == false)
      return false;
  }

  // RIC Report Style List
  if (m0->sz_ric_report_style_list != m1->sz_ric_report_style_list)
    return false;

  if(m0->sz_ric_report_style_list > 0){
    assert(m0->ric_report_style_list != NULL);
    assert(m1->ric_report_style_list != NULL);
  }

  for (size_t i = 0; i<m0->sz_ric_report_style_list; i++) {
    // RIC Report Styles
    if(eq_ric_report_style_item(&m0->ric_report_style_list[i], &m1->ric_report_style_list[i]) == false)
      return false;
  }

  return true;
}


kpm_ran_function_def_t cp_kpm_ran_function_def(kpm_ran_function_def_t const * src)
{
  assert(src != NULL);

  kpm_ran_function_def_t dst = {0};

  dst.name = cp_ran_function_name(&src->name);

  if(src->ric_event_trigger_style_list != NULL){
    assert(src->sz_ric_event_trigger_style_list > 0 && src->sz_ric_event_trigger_style_list < 64);

    dst.sz_ric_event_trigger_style_list = src->sz_ric_event_trigger_style_list;
    dst.ric_event_trigger_style_list = calloc(dst.sz_ric_event_trigger_style_list, sizeof(ric_event_trigger_style_item_t));
    assert(dst.ric_event_trigger_style_list != NULL && "Memory exhausted");

    for(size_t i = 0; i < dst.sz_ric_event_trigger_style_list; ++i){
     dst.ric_event_trigger_style_list[i] = cp_ric_event_trigger_style_list(&src->ric_event_trigger_style_list[i]);   
    }  
  }

  if(src->ric_report_style_list != NULL){
    assert(src->sz_ric_report_style_list > 0 && src->sz_ric_report_style_list < 64);

    dst.sz_ric_report_style_list = src->sz_ric_report_style_list;  
    dst.ric_report_style_list = calloc(dst.sz_ric_report_style_list,sizeof(ric_report_style_item_t));  
    assert(dst.ric_report_style_list != NULL && "Memory exhausted");

    for(size_t i = 0; i < dst.sz_ric_report_style_list; ++i){
     dst.ric_report_style_list[i] = cp_ric_report_style_item(&src->ric_report_style_list[i]); 
    }
  }

  return dst;
}

