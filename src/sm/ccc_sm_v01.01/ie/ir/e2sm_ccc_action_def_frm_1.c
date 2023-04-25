#include <assert.h>

#include "e2sm_ccc_action_def_frm_1.h"

void free_ccc_action_def_frm_1(e2sm_ccc_action_def_frm_1_t *src)
{
  assert(src != NULL);

  // Mandatory
  // List of Node-level RAN Configuration Structures  -  [1..256]
  for (size_t i = 0; i<src->node_level_conf_list_len; i++)
  {
    // Mandatory
    // Report Type
    // 9.3.9
    // nothing to free

    // Mandatory
    // Node-Level RAN Configuration Structures
    // 8.2.1
    free_ccc_node_level_ran_conf_struct_report_et_ad(&src->node_level_conf_list_frm_1[i].node_level_ran_conf_struct);
  }
  free(src->node_level_conf_list_frm_1);

}

bool eq_ccc_action_def_frm_1(e2sm_ccc_action_def_frm_1_t const* m0,  e2sm_ccc_action_def_frm_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // List of Node-level RAN Configuration Structures  -  [1..256]
  if(m0->node_level_conf_list_len != m1->node_level_conf_list_len) return false;

  for (size_t i = 0; i<m0->node_level_conf_list_len; i++)
  {
    // Mandatory
    // Report Type
    // 9.3.9
    if (m0->node_level_conf_list_frm_1[i].report_type != m1->node_level_conf_list_frm_1[i].report_type)
      return false;

    // Mandatory
    // Node-Level RAN Configuration Structures
    // 8.2.1
    if (eq_ccc_node_level_ran_conf_struct_report_et_ad(&m0->node_level_conf_list_frm_1[i].node_level_ran_conf_struct, &m1->node_level_conf_list_frm_1[i].node_level_ran_conf_struct) != true)
      return false;

  }


  return true;
}
