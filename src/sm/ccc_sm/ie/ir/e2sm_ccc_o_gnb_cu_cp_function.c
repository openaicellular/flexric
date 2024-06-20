#include "e2sm_ccc_o_gnb_cu_cp_function.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void free_e2sm_ccc_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t* src)
{
  assert(src != NULL);

//  src->gnb_id;
//  src->gnb_id_len;
//  src->plmn_id;

  free(src->gnb_cu_name);

  if(src->xn_ho_block_list.sz_g_gnb_id_lst> 0)
    free_g_gnb_id_lst(&src->xn_ho_block_list);

  if(src->x2_ho_block_list.sz_g_enb_id_lst > 0)
    free_g_enb_id_lst(&src->x2_ho_block_list);

  if(src->x2_allow_list.sz_g_enb_id_lst > 0)
    free_g_enb_id_lst(&src->x2_allow_list);

  if(src->x2_block_list.sz_g_enb_id_lst > 0)
    free_g_enb_id_lst(&src->x2_block_list);

  if(src->xn_allow_list.sz_g_gnb_id_lst> 0)
    free_g_gnb_id_lst(&src->xn_allow_list);

  if(src->xn_block_list.sz_g_gnb_id_lst> 0)
    free_g_gnb_id_lst(&src->xn_block_list);
}

bool eq_e2sm_ccc_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t const* m0, e2sm_ccc_o_gnb_cu_cp_function_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->gnb_id != m1->gnb_id)
    return false;

  if(m0->gnb_id_len != m1->gnb_id_len)
    return false;

  if(!eq_e2sm_plmn(&m0->plmn_id, &m1->plmn_id))
    return false;

  if(!eq_g_gnb_id_lst(&m0->xn_block_list, &m1->xn_block_list))
    return false;

  if(!eq_g_enb_id_lst(&m0->x2_block_list, &m1->x2_block_list))
    return false;

  if(!eq_g_enb_id_lst(&m0->x2_allow_list, &m1->x2_allow_list))
    return false;

  if(!eq_g_enb_id_lst(&m0->x2_ho_block_list, &m1->x2_ho_block_list))
    return false;

  if(!eq_g_gnb_id_lst(&m0->xn_allow_list, &m1->xn_allow_list))
    return false;

  if(!eq_g_gnb_id_lst(&m0->xn_ho_block_list, &m1->xn_ho_block_list))
    return false;

  return true;
}


e2sm_ccc_o_gnb_cu_cp_function_t cp_e2sm_ccc_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_o_gnb_cu_cp_function_t dst = {0};

  dst.gnb_id = src->gnb_id;
  dst.gnb_id_len = src->gnb_id_len;
  dst.plmn_id = cp_e2sm_plmn(&src->plmn_id);

  if (src->xn_ho_block_list.sz_g_gnb_id_lst> 0)
    dst.xn_ho_block_list = cp_g_gnb_id_lst(&src->xn_ho_block_list);

  if (src->x2_ho_block_list.sz_g_enb_id_lst > 0)
    dst.x2_ho_block_list = cp_g_enb_id_lst(&src->x2_ho_block_list);

  if (src->x2_allow_list.sz_g_enb_id_lst > 0)
    dst.x2_allow_list = cp_g_enb_id_lst(&src->x2_allow_list);

  if (src->x2_block_list.sz_g_enb_id_lst > 0)
    dst.x2_block_list = cp_g_enb_id_lst(&src->x2_block_list);

  if (src->xn_allow_list.sz_g_gnb_id_lst> 0)
    dst.xn_allow_list = cp_g_gnb_id_lst(&src->xn_allow_list);

  if (src->xn_block_list.sz_g_gnb_id_lst> 0)
    dst.xn_block_list = cp_g_gnb_id_lst(&src->xn_block_list);

  return dst;
}



