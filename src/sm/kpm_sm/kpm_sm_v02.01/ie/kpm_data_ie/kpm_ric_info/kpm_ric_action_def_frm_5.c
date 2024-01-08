#include <assert.h>

#include "kpm_ric_action_def_frm_5.h"

void free_kpm_action_def_frm_5(kpm_act_def_format_5_t * src)
{
  assert(src != NULL);

  // UE ID List
  for (size_t i = 0; i<src->ue_id_lst_len; i++)
  {
    free_ue_id_e2sm(&src->ue_id_lst[i]);
  }
  free(src->ue_id_lst);


  // Action Definition Format 1
  free_kpm_action_def_frm_1(&src->action_def_format_1);

}

bool eq_kpm_action_def_frm_5(kpm_act_def_format_5_t const * m0, kpm_act_def_format_5_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // UE ID List
  if (m0->ue_id_lst_len != m1->ue_id_lst_len)
    return false;

  for (size_t i = 0; i<m0->ue_id_lst_len; i++)
  {
    if (eq_ue_id_e2sm(&m0->ue_id_lst[i], &m1->ue_id_lst[i]) != true)
      return false;
  }


  // Action Definition Format 1
  if (eq_kpm_action_def_frm_1(&m0->action_def_format_1, &m1->action_def_format_1) != true)
    return false;

  return true;
}

kpm_act_def_format_5_t cp_kpm_action_def_frm_5(const kpm_act_def_format_5_t * src)
{
  assert(src != NULL);

  kpm_act_def_format_5_t dst = {0};

  // UE ID List
  dst.ue_id_lst_len = src->ue_id_lst_len;

  dst.ue_id_lst = calloc(dst.ue_id_lst_len, sizeof(ue_id_e2sm_t));
  memcpy(dst.ue_id_lst, src->ue_id_lst, dst.ue_id_lst_len * sizeof(ue_id_e2sm_t));

  for (size_t i = 0; i<dst.ue_id_lst_len; i++)
  {
    dst.ue_id_lst[i] = cp_ue_id_e2sm(&src->ue_id_lst[i]);
  }


  // Action Definition Format 1
  dst.action_def_format_1 = cp_kpm_action_def_frm_1(&src->action_def_format_1);


  return dst;
}
