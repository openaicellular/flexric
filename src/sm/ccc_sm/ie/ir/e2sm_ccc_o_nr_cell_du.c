#include "e2sm_ccc_o_nr_cell_du.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_o_nr_cell_du(e2sm_ccc_o_nr_cell_du_t* src)
{
  assert(src != NULL);

  // plMNInfoList
  // [1...65536]
  assert(src->sz_plmn_info_lst > 0 && src->sz_plmn_info_lst < 65537);
  for (size_t i = 0; i < src->sz_plmn_info_lst; i++){
    free_e2sm_ccc_plmn_info(&src->plmn_info_lst[i]);
  }
  free(src->plmn_info_lst);

  // bWPList
  // [1...256]
  assert(src->sz_bwp_lst > 0 && src->sz_bwp_lst < 257);
  for (size_t i = 0; i < src->sz_bwp_lst; i++){
    free_e2sm_ccc_o_bwp(&src->bwp_lst[i]);
  }
  free(src->bwp_lst);

  // partition list
  free_e2sm_ccc_partition_lst(&src->partition_lst);
}

bool eq_e2sm_ccc_o_nr_cell_du(e2sm_ccc_o_nr_cell_du_t const* m0, e2sm_ccc_o_nr_cell_du_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->cell_local_id != m1->cell_local_id)
    return false;

  if (m0->operational_state != m1->operational_state)
    return false;

  if (m0->administrative_state != m1->administrative_state)
    return false;

  if (m0->cell_state != m1->cell_state)
    return false;

  if (m0->n_rpci != m1->n_rpci)
    return false;

  if (m0->n_rtac != m1->n_rtac)
    return false;

  if (m0->arfcn_ul != m1->arfcn_ul)
    return false;

  if (m0->arfcn_sul != m1->arfcn_sul)
    return false;

  if (m0->arfcn_dl != m1->arfcn_dl)
    return false;

  if (m0->ssb_frequency != m1->ssb_frequency)
    return false;

  if (m0->ssb_periodicity != m1->ssb_periodicity)
    return false;

  if (m0->ssb_sub_carrier_spacing != m1->ssb_sub_carrier_spacing)
    return false;

  if (m0->ssb_off_set != m1->ssb_off_set)
    return false;

  if (m0->ssb_duration != m1->ssb_duration)
    return false;

  if (m0->bS_Channel_BwSUL != m1->bS_Channel_BwSUL)
    return false;

  if (m0->bS_Channel_BwUL != m1->bS_Channel_BwUL)
    return false;

  if (m0->bS_Channel_BwDL != m1->bS_Channel_BwDL)
    return false;
  // plMNInfoList
  // [1..65536]
  assert(m0->sz_plmn_info_lst > 0 && m1->sz_plmn_info_lst < 65537);
  if (m0->sz_plmn_info_lst != m1->sz_plmn_info_lst)
    return false;

  for (size_t i = 0; i < m0->sz_plmn_info_lst; i++){
    if (!eq_e2sm_ccc_plmn_info(&m0->plmn_info_lst[i], &m1->plmn_info_lst[i]))
      return false;
  }
  // bWPList
  // [1...256]
  assert(m0->sz_bwp_lst > 0 && m1->sz_bwp_lst < 257);
  if (m0->sz_bwp_lst != m1->sz_bwp_lst)
    return false;

  for (size_t i = 0; i < m0->sz_bwp_lst; i++){
    if (!eq_e2sm_ccc_o_bwp(&m0->bwp_lst[i], &m1->bwp_lst[i]))
      return false;
  }

  if (!eq_e2sm_ccc_partition_lst(&m0->partition_lst, &m1->partition_lst))
    return false;

  return true;
}


e2sm_ccc_o_nr_cell_du_t cp_e2sm_ccc_o_nr_cell_du(e2sm_ccc_o_nr_cell_du_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_nr_cell_du_t dst = {0};

  dst.cell_local_id = src->cell_local_id;
  dst.operational_state = src->operational_state;
  dst.administrative_state = src->administrative_state;
  dst.cell_state = src->cell_state;
  dst.n_rpci = src->n_rpci;
  dst.n_rtac = src->n_rtac;
  dst.arfcn_dl = src->arfcn_dl;
  dst.arfcn_ul = src->arfcn_ul;
  dst.arfcn_sul = src->arfcn_sul;
  dst.ssb_frequency = src->ssb_frequency;
  dst.ssb_periodicity = src->ssb_periodicity;
  dst.ssb_sub_carrier_spacing = src->ssb_sub_carrier_spacing;
  dst.ssb_off_set = src->ssb_off_set;
  dst.ssb_duration = src->ssb_duration;
  dst.bS_Channel_BwDL = src->bS_Channel_BwDL;
  dst.bS_Channel_BwUL = src->bS_Channel_BwUL;
  dst.bS_Channel_BwSUL = src->bS_Channel_BwSUL;

  // [1..65536]
  assert(src->sz_plmn_info_lst > 0 && src->sz_plmn_info_lst < 65537);
  dst.sz_plmn_info_lst = src->sz_plmn_info_lst;
  dst.plmn_info_lst = calloc(src->sz_plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));
  assert(dst.plmn_info_lst!= NULL);
  for (size_t i = 0; i < src->sz_plmn_info_lst; i++){
    dst.plmn_info_lst[i] = cp_e2sm_ccc_plmn_info(&src->plmn_info_lst[i]);
  }

  // [1..128]
  assert(src->sz_bwp_lst > 0 && src->sz_bwp_lst< 129);
  dst.sz_bwp_lst = src->sz_bwp_lst;
  dst.bwp_lst = calloc(src->sz_bwp_lst, sizeof(e2sm_ccc_o_bwp_t));
  assert(dst.bwp_lst != NULL);
  for (size_t i = 0; i < dst.sz_plmn_info_lst; i++){
    dst.bwp_lst[i] = cp_e2sm_ccc_o_bwp(&src->bwp_lst[i]);
  }

  dst.partition_lst = cp_e2sm_ccc_partition_lst(&src->partition_lst);

  return dst;
}



