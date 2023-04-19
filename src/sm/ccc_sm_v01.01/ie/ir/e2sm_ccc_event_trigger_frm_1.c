#include <assert.h>

#include "e2sm_ccc_event_trigger_frm_1.h"

void free_ccc_event_trigger_frm_1(e2sm_ccc_ev_trg_frm_1_t *src)
{
  assert(src != NULL);

  for (size_t i = 0; i<src->node_level_conf_list_len; i++)
    free_ccc_node_level_ran_conf_struct_report_et_ad(&src->node_level_ran_conf_struct_list[i]);

  free(src->node_level_ran_conf_struct_list);

}
