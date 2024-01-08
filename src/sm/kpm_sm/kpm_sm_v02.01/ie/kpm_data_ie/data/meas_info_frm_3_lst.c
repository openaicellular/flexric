#include <assert.h>

#include "meas_info_frm_3_lst.h"

void free_meas_info_frm_3(meas_info_format_3_lst_t* src)
{
  assert(src != NULL);

  // Meas Type
  if (src->meas_type.type == NAME_MEAS_TYPE)
    free_byte_array(src->meas_type.name);

  // Bin Range Definition
  if (src->bin_range_def != NULL)
  {
    free_kpm_bin_range_def(src->bin_range_def);
    free(src->bin_range_def);
  }

  // Matching Condition Format 3
  for (size_t i = 0; i < src->matching_cond_lst_len; i++)
    free_matching_cond_frm_3(&src->matching_cond_lst[i]);
  free(src->matching_cond_lst);
  
}

bool eq_meas_info_frm_3(meas_info_format_3_lst_t const * m0, meas_info_format_3_lst_t const * m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Meas Type
  if (eq_meas_type(&m0->meas_type, &m1->meas_type) != true)
    return false;


  // Matching Condition Format 3
  if (m0->matching_cond_lst_len != m1->matching_cond_lst_len)
    return false;

  for (size_t i = 0; i < m0->matching_cond_lst_len; i++)
  {
    if (eq_matching_cond_frm_3(&m0->matching_cond_lst[i], &m1->matching_cond_lst[i]) != true)
      return false;
  }


  // Bin Range Definition
  if (m0->bin_range_def != NULL || m1->bin_range_def != NULL)
  {
    if (eq_kpm_bin_range_def(m0->bin_range_def, m1->bin_range_def) != true)
      return false;
  }


  return true;
}

meas_info_format_3_lst_t cp_meas_info_frm_3(const meas_info_format_3_lst_t * src)
{
  assert(src != NULL);

  meas_info_format_3_lst_t dst = {0};

  // Meas type
  dst.meas_type = cp_meas_type(&src->meas_type);

  // Matching Condition Format 3
  dst.matching_cond_lst_len = src->matching_cond_lst_len;

  dst.matching_cond_lst = calloc(dst.matching_cond_lst_len, sizeof(matching_condition_format_3_lst_t));
  memcpy(dst.matching_cond_lst, src->matching_cond_lst, dst.matching_cond_lst_len * sizeof(matching_condition_format_3_lst_t));

  for (size_t i = 0; i<dst.matching_cond_lst_len; i++)
  {
    dst.matching_cond_lst[i] = cp_kpm_matching_cond_frm_3(&src->matching_cond_lst[i]);
  }

  // Bin Range Definition
  if (src->bin_range_def != NULL)
  {
    dst.bin_range_def = calloc(1, sizeof(bin_range_def_t));
    *dst.bin_range_def = cp_kpm_bin_range_def(src->bin_range_def);
  }



  return dst;
}
