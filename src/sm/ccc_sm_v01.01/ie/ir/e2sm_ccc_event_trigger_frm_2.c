#include <assert.h>

#include "e2sm_ccc_event_trigger_frm_2.h"

void free_ccc_event_trigger_frm_2(e2sm_ccc_ev_trg_frm_2_t *src)
{
  assert(src != NULL);

  // Mandatory
  // List of Cell-level Configuration Structures  [1..1024]
  for (size_t i = 0; i<src->cell_level_conf_list_len; i++)
  {
    // Optional
    // Cell Global ID
    // 9.3.6
    if (src->cell_level_conf_list[i].cell_global_id != NULL)
      free(src->cell_level_conf_list[i].cell_global_id);

    // Mandatory
    // List of RAN Configuration Structures  [1..1024]
    for (size_t j = 0; j<src->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len; j++)
      free_ccc_cell_level_ran_conf_struct_report_et_ad(&src->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j]);
    free(src->cell_level_conf_list[i].cell_level_ran_conf_struct_list);
  }
  free(src->cell_level_conf_list);
   
}

bool eq_ccc_event_trigger_frm_2(e2sm_ccc_ev_trg_frm_2_t const* m0,  e2sm_ccc_ev_trg_frm_2_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // List of Cell-level Configuration Structures  -  [1..1024]
  if (m0->cell_level_conf_list_len != m1->cell_level_conf_list_len)
    return false;

  for (size_t i = 0; i<m0->cell_level_conf_list_len; i++)
  {
    // Optional
    // Cell Global ID
    // 9.3.6
    if (m0->cell_level_conf_list[i].cell_global_id != NULL || m1->cell_level_conf_list[i].cell_global_id != NULL)
    {
      if (eq_cell_global_id(m0->cell_level_conf_list[i].cell_global_id, m1->cell_level_conf_list[i].cell_global_id) != true)
        return false;
    }
    
    // Mandatory
    // List of RAN Configuration Structures  -  [1..1024]
    if (m0->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len != m1->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len)
      return false;
    
    for (size_t j = 0; j<m0->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len; j++)
    {
      if(eq_ccc_cell_level_ran_conf_struct_report_et_ad(&m0->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j], &m1->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j]) != true)
        return false;
    }
  }

  return true;
}
