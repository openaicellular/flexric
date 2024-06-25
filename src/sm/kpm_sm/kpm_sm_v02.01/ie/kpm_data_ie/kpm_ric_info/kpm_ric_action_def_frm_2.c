#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "kpm_ric_action_def_frm_2.h"
#include "kpm_ric_action_def_frm_1.h"

void free_kpm_action_def_frm_2(kpm_act_def_format_2_t * src) 
{
    assert(src != NULL);

    // UE ID
    free_ue_id_e2sm(&src->ue_id);

    // Action Definition 
    free_kpm_action_def_frm_1(&src->action_def_format_1);

}


bool eq_kpm_action_def_frm_2(kpm_act_def_format_2_t const * m0, kpm_act_def_format_2_t const * m1)
{
    assert(m0 != NULL);
    assert(m1 != NULL);

    if (eq_ue_id_e2sm(&m0->ue_id, &m1->ue_id) != true)
      return false;

    if (eq_kpm_action_def_frm_1(&m0->action_def_format_1, &m1->action_def_format_1) != true)
      return false;

    return true;
}


kpm_act_def_format_2_t cp_kpm_action_def_frm_2(const kpm_act_def_format_2_t * src)
{
  assert(src != NULL);

  kpm_act_def_format_2_t dst = {0};


  // UE ID
  dst.ue_id = cp_ue_id_e2sm(&src->ue_id);

  // Action Definition Format 1
  dst.action_def_format_1 = cp_kpm_action_def_frm_1(&src->action_def_format_1);


  return dst;
}
