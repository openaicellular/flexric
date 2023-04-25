#include <assert.h>

#include "e2sm_ccc_action_def.h"

void free_ccc_action_def(e2sm_ccc_action_def_t *src)
{
  assert(src != NULL);

  switch (src->report_style_type)
  {
  case STYLE_1_RIC_SERVICE_REPORT_CCC:
    free_ccc_action_def_frm_1(&src->frm_1);
    break;

  case STYLE_2_RIC_SERVICE_REPORT_CCC:
    free_ccc_action_def_frm_2(&src->frm_2);
    break;
  
  default:
    assert("Unknown CCC RIC Action Definition Format Type");
  }
}

bool eq_ccc_action_def(e2sm_ccc_action_def_t const* m0,  e2sm_ccc_action_def_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->report_style_type != m1->report_style_type)
    return false;

  switch (m0->report_style_type)
  {
  case STYLE_1_RIC_SERVICE_REPORT_CCC:
    if (m0->type != FORMAT_1_ACTION_DEFINITION_CCC || m1->type != FORMAT_1_ACTION_DEFINITION_CCC) return false;
    if (eq_ccc_action_def_frm_1(&m0->frm_1, &m1->frm_1) != true)
      return false;
    break;

  case STYLE_2_RIC_SERVICE_REPORT_CCC:
    if (m0->type != FORMAT_2_ACTION_DEFINITION_CCC || m1->type != FORMAT_2_ACTION_DEFINITION_CCC) return false;
    if (eq_ccc_action_def_frm_2(&m0->frm_2, &m1->frm_2) != true)
      return false;
    break;
  
  default:
    assert("Unknown CCC RIC Action Definition Style Type");
  }

  return true;
}
