#include <assert.h>

#include "kpm_ric_action_def_frm_4.h"

void free_kpm_action_def_frm_4(kpm_act_def_format_4_t* src)
{
  assert(src != NULL);

  // Matching Condition
  for (size_t i = 0; i<src->matching_cond_lst_len; i++)
  {
    free_matching_cond_frm_4(&src->matching_cond_lst[i]);
  }
  free(src->matching_cond_lst);


  // Action Definition Format 1
  free_kpm_action_def_frm_1(&src->action_def_format_1);


}

bool eq_kpm_action_def_frm_4(kpm_act_def_format_4_t const * m0, kpm_act_def_format_4_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Matching Condition
  if (m0->matching_cond_lst_len != m1->matching_cond_lst_len)
    return false;

  for (size_t i = 0; i<m0->matching_cond_lst_len; i++)
  {
    if (eq_matching_cond_frm_4(&m0->matching_cond_lst[i], &m1->matching_cond_lst[i]) != true)
      return false;
  }

  // Action Definition Format 1
  if (eq_kpm_action_def_frm_1(&m0->action_def_format_1, &m1->action_def_format_1) != true)
    return false;


  return true;
}

kpm_act_def_format_4_t cp_kpm_action_def_frm_4(const kpm_act_def_format_4_t * src)
{
  assert(src != NULL);

  kpm_act_def_format_4_t dst = {0};

  // Matching Condition
  dst.matching_cond_lst_len = src->matching_cond_lst_len;

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_4_lst_t));
  memcpy(dst.matching_cond_lst, src->matching_cond_lst, dst.matching_cond_lst_len * sizeof(matching_condition_format_4_lst_t));

  for (size_t i = 0; i<dst.matching_cond_lst_len; i++)
  {
    dst.matching_cond_lst[i] = cp_matching_cond_frm_4(&src->matching_cond_lst[i]);
  }


  // Action Definition Format 1
  dst.action_def_format_1 = cp_kpm_action_def_frm_1(&src->action_def_format_1);


  return dst;
}
