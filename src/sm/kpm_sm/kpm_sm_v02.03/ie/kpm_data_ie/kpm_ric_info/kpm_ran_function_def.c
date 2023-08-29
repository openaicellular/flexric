#include <assert.h>

#include "kpm_ran_function_def.h"

void free_kpm_ran_function_def(kpm_ran_function_def_t* src)
{
  assert(src != NULL);
  
  // RAN Function Name
  free_ran_function_name(&src->name);

  // RIC Event Trigger Style List
  if (src->ric_event_trigger_style_list != NULL || src->sz_ric_event_trigger_style_list != 0)
  {
    for (size_t i = 0; i<src->sz_ric_event_trigger_style_list; i++)
      free_byte_array(src->ric_event_trigger_style_list[i].style_name);

    free(src->ric_event_trigger_style_list);
  }

  // RIC Report Style List
  if (src->ric_report_style_list != NULL || src->sz_ric_report_style_list != 0)
  {
    for (size_t i = 0; i<src->sz_ric_report_style_list; i++)
    {
      free_byte_array(src->ric_report_style_list[i].report_style_name);

      for (size_t j = 0; j<src->ric_report_style_list[i].meas_info_for_action_lst_len; j++)
      {
        free_byte_array(src->ric_report_style_list[i].meas_info_for_action_lst[j].name);

        if (src->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL)
          free(src->ric_report_style_list[i].meas_info_for_action_lst[j].id);

        if (src->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL)
          free_kpm_bin_range_def(src->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def);
      }
      free(src->ric_report_style_list[i].meas_info_for_action_lst);
    }
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
  if (m0->ric_event_trigger_style_list != NULL || m1->ric_event_trigger_style_list != NULL)
  {
    if (m0->sz_ric_event_trigger_style_list != m1->sz_ric_event_trigger_style_list)
      return false;

    for (size_t i = 0; i<m0->sz_ric_event_trigger_style_list; i++)
    {
      // RIC Event Trigger Style
      if (m0->ric_event_trigger_style_list[i].style_type != m1->ric_event_trigger_style_list[i].style_type)
        return false;

      switch (m0->ric_event_trigger_style_list[i].style_type)
      {
      case STYLE_1_RIC_EVENT_TRIGGER:
      {
        // RIC Event Trigger Style Name
        if (eq_byte_array(&m0->ric_event_trigger_style_list[i].style_name, &m1->ric_event_trigger_style_list[i].style_name) != true)
          return false;


        // RIC Event Trigger Format
        if (m0->ric_event_trigger_style_list[i].format_type != m1->ric_event_trigger_style_list[i].format_type)
          return false;
          
        break;
      }
      
      default:
        assert(false && "Unknown RIC Event Trigger Style Type");
      }
    }
  }

  // RIC Report Style List
  if (m0->ric_report_style_list != NULL || m1->ric_report_style_list != NULL)
  {
    if (m0->sz_ric_report_style_list != m1->sz_ric_report_style_list)
      return false;

    for (size_t i = 0; i<m0->sz_ric_report_style_list; i++)
    {
      // RIC Report Styles
      if (m0->ric_report_style_list[i].report_style_type != m1->ric_report_style_list[i].report_style_type)
        return false;

      if (m0->ric_report_style_list[i].act_def_format_type != m1->ric_report_style_list[i].act_def_format_type)
        return false;

      if (m0->ric_report_style_list[i].ind_hdr_format_type != m1->ric_report_style_list[i].ind_hdr_format_type)
        return false;

      if (m0->ric_report_style_list[i].ind_msg_format_type != m1->ric_report_style_list[i].ind_msg_format_type)
        return false;

      // RIC REPORT Style Name
      if (eq_byte_array(&m0->ric_report_style_list[i].report_style_name, &m1->ric_report_style_list[i].report_style_name) != true)
        return false;

      // Measurement Information for Action
      if (m0->ric_report_style_list[i].meas_info_for_action_lst_len != m1->ric_report_style_list[i].meas_info_for_action_lst_len)
        return false;

      for (size_t j = 0; j<m0->ric_report_style_list[i].meas_info_for_action_lst_len; j++)
      {
        // Measurement Type Name
        if (eq_byte_array(&m0->ric_report_style_list[i].meas_info_for_action_lst[j].name, &m1->ric_report_style_list[i].meas_info_for_action_lst[j].name) != true)
          return false;

        // Measurement Type ID
        if ((m0->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL || m1->ric_report_style_list[i].meas_info_for_action_lst[j].id != NULL) && *m0->ric_report_style_list[i].meas_info_for_action_lst[j].id != *m1->ric_report_style_list[i].meas_info_for_action_lst[j].id)
          return false;

        // Bin Range Definition
        // not yet implemented in ASN.1
        if (m0->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL || m1->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def != NULL)
        {
          if (eq_kpm_bin_range_def(m0->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def, m1->ric_report_style_list[i].meas_info_for_action_lst[j].bin_range_def) != true)
            return false;
        }

      }
    
    }
  }

  return true;
}


kpm_ran_function_def_t cp_kpm_ran_function_def(kpm_ran_function_def_t const * src)
{
  assert(src != NULL);

  kpm_ran_function_def_t dst = {0};

  dst.name = cp_ran_function_name(&src->name);

  if(src->ric_event_trigger_style_list != NULL)
    assert(false && "Not yet implemented");

  if(src->ric_report_style_list != NULL)
    assert(false && "Not yet implemented");

  return dst;
}
