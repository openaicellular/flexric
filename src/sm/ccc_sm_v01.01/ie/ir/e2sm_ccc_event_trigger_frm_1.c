#include <assert.h>

#include "e2sm_ccc_event_trigger_frm_1.h"

void free_ccc_event_trigger_frm_1(e2sm_ccc_ev_trg_frm_1_t *src)
{
  assert(src != NULL);

  for (size_t i = 0; i<src->node_level_conf_list_len; i++)
    free_ccc_node_level_ran_conf_struct_report_et_ad(&src->node_level_ran_conf_struct_list[i]);

  free(src->node_level_ran_conf_struct_list);

}

bool eq_ccc_event_trigger_frm_1(e2sm_ccc_ev_trg_frm_1_t const* m0,  e2sm_ccc_ev_trg_frm_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // List of Node-level Configuration Structures  -  [1..256]
  if (m0->node_level_conf_list_len != m1->node_level_conf_list_len)
    return false;

  for (size_t i = 0; i<m0->node_level_conf_list_len; i++)
  {
    if(eq_ccc_node_level_ran_conf_struct_report_et_ad(&m0->node_level_ran_conf_struct_list[i], &m1->node_level_ran_conf_struct_list[i]) != true)
      return false;
  }


  return true;
}
