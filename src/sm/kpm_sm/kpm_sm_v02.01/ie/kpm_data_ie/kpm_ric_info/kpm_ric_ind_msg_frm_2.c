#include <assert.h>

#include "kpm_ric_ind_msg_frm_2.h"

void free_kpm_ind_msg_frm_2(kpm_ind_msg_format_2_t* src)
{
  assert(src != NULL);

  // Measurement Data  move this
  // [1, 65535]
  for (size_t i = 0; i<src->meas_data_lst_len; i++)
  {
    free(src->meas_data_lst[i].meas_record_lst);

    if (src->meas_data_lst[i].incomplete_flag != NULL)
      free(src->meas_data_lst[i].incomplete_flag);
  }
  free(src->meas_data_lst);

  // Measurement Information Condition UE List
  // [1, 65535]
  for (size_t i = 0; i<src->meas_info_cond_ue_lst_len; i++)
    free_kpm_meas_info_cond_ue(&src->meas_info_cond_ue_lst[i]);
  free(src->meas_info_cond_ue_lst);

  // Granularity Period - OPTIONAL
  if (src->gran_period_ms != NULL)
    free(src->gran_period_ms);

}

static
bool eq_gran_period_ms(uint32_t const* m0, uint32_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(*m0 != *m1)
    return false;

  return true;
}

bool eq_kpm_ind_msg_frm_2(kpm_ind_msg_format_2_t const* m0, kpm_ind_msg_format_2_t const* m1)
{
  assert(m0 != NULL);
  assert(m1 != NULL);

  // Measurement Data
  // [1, 65535]
  if (m0->meas_data_lst_len != m1->meas_data_lst_len)
    return false;

  for (size_t i = 0; i<m0->meas_data_lst_len; i++)
  {
    if (eq_meas_data_lst(&m0->meas_data_lst[i], &m1->meas_data_lst[i]) != true){
      assert(0!=0 && "Debugging purposes");
      return false;
    }
  }

  // Measurement Information Condition UE List
  // [1, 65535]
  if (m0->meas_info_cond_ue_lst_len != m1->meas_info_cond_ue_lst_len)
    return false;

  for (size_t i = 0; i<m0->meas_info_cond_ue_lst_len; i++)
  {
    if (eq_kpm_meas_info_cond_ue(&m0->meas_info_cond_ue_lst[i], &m1->meas_info_cond_ue_lst[i]) != true)
      return false;
  }

  // Granularity Period - OPTIONAL
  if(eq_gran_period_ms(m0->gran_period_ms, m1->gran_period_ms) == false)
    return false;

  return true;
}

kpm_ind_msg_format_2_t cp_kpm_ind_msg_frm_2(kpm_ind_msg_format_2_t const* src)
{
  assert(src != NULL);
  kpm_ind_msg_format_2_t dst = {0};

  // Granularity Period
  if (src->gran_period_ms) {
    dst.gran_period_ms = calloc(1, sizeof(uint32_t));
    assert(dst.gran_period_ms != NULL && "Memory exhausted");
    *dst.gran_period_ms = *src->gran_period_ms; 
  }

  // Measurement Data
  if (src->meas_data_lst_len)
  {
    dst.meas_data_lst_len = src->meas_data_lst_len;

    dst.meas_data_lst = calloc(src->meas_data_lst_len, sizeof(meas_data_lst_t));
    assert(dst.meas_data_lst != NULL && "Memory exhausted");

    for (size_t i = 0; i<dst.meas_data_lst_len; i++){
     dst.meas_data_lst[i] = cp_meas_data_lst(&src->meas_data_lst[i]);
    }
  }

  // Measurement Condition UE Information
  if (src->meas_info_cond_ue_lst_len)
  {
    dst.meas_info_cond_ue_lst_len = src->meas_info_cond_ue_lst_len;

    dst.meas_info_cond_ue_lst = calloc(dst.meas_info_cond_ue_lst_len, sizeof(meas_info_cond_ue_lst_t));
    memcpy(dst.meas_info_cond_ue_lst, src->meas_info_cond_ue_lst, dst.meas_info_cond_ue_lst_len * sizeof(meas_info_cond_ue_lst_t));

    for (size_t i = 0; i<dst.meas_info_cond_ue_lst_len; i++)
    {
      dst.meas_info_cond_ue_lst[i] = cp_kpm_meas_info_cond_ue(&src->meas_info_cond_ue_lst[i]);
    }
  }


  return dst;
}
