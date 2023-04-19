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
