#include "e2sm_ccc_o_nr_cell_cu.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_o_nr_cell_cu(e2sm_ccc_o_nr_cell_cu_t* src)
{
  assert(src != NULL);

//  src->cell_local_id;

  // [1..65536]
  assert(src->sz_plmn_info_lst > 0 && src->sz_plmn_info_lst < 65537);
  for (size_t i = 0; i < src->sz_plmn_info_lst; ++i){
    free_e2sm_ccc_plmn_info(&src->plmn_info_lst[i]);
  }

  free(src->plmn_info_lst);
}

bool eq_e2sm_ccc_o_nr_cell_cu(e2sm_ccc_o_nr_cell_cu_t const* m0, e2sm_ccc_o_nr_cell_cu_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->cell_local_id != m1->cell_local_id)
    return false;

  // [1..65536]
  assert(m0->sz_plmn_info_lst > 0 && m0->sz_plmn_info_lst < 65537);
  assert(m1->sz_plmn_info_lst > 0 && m1->sz_plmn_info_lst < 65537);
  for(size_t i = 0; i < m0->sz_plmn_info_lst; ++i){
    if (!eq_e2sm_ccc_plmn_info(&m0->plmn_info_lst[i], &m1->plmn_info_lst[i])){
      return false;
    }
  }

  return true;
}


e2sm_ccc_o_nr_cell_cu_t cp_e2sm_ccc_o_nr_cell_cu(e2sm_ccc_o_nr_cell_cu_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_nr_cell_cu_t dst = {0};

  dst.cell_local_id = src->cell_local_id;

  // [1..65536]
  assert(src->sz_plmn_info_lst > 0 && src->sz_plmn_info_lst < 65537);
  dst.sz_plmn_info_lst = src->sz_plmn_info_lst;
  dst.plmn_info_lst = calloc(src->sz_plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));
  assert(dst.plmn_info_lst != NULL);
  memcpy(dst.plmn_info_lst, src->plmn_info_lst, sizeof(e2sm_ccc_plmn_info_t));

  return dst;
}



