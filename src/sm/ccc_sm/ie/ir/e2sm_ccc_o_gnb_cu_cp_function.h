#ifndef E2SM_CCC_O_GNB_CU_CP_FUNCTION_H
#define E2SM_CCC_O_GNB_CU_CP_FUNCTION_H

#include <stdint.h>
#include <stdbool.h>
#include "../../../../lib/3gpp/ie/plmn_identity.h"

// 8.8.1.2
typedef struct{
  // Integer
  // [0..4294967295]
  uint32_t gnb_id;

  // Integer
  // [22..32]
  uint8_t gnb_id_len;

  // String
  char* gnb_cu_name;

  // plmn_id
  // 9.3.12
  // E2SM 6.2.3.1
  e2sm_plmn_t plmn_id;

  // String
  // Conflict with O-RAN structure
  // Following the json schema
  // TODO: Change to array type
  char* x2_block_list;

  // String
  char* x2_allow_list;

  // String
  char* xn_block_list;

  // String
  char* xn_allow_list;

  // String
  char* x2_ho_block_list;

  // String
  char* xn_ho_block_list;

} e2sm_ccc_o_gnb_cu_cp_function_t;

void free_e2sm_ccc_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t* src);

bool eq_e2sm_ccc_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t const* m0, e2sm_ccc_o_gnb_cu_cp_function_t const* m1);

e2sm_ccc_o_gnb_cu_cp_function_t cp_e2sm_ccc_o_gnb_cu_cp_function(e2sm_ccc_o_gnb_cu_cp_function_t const* src);

#endif

