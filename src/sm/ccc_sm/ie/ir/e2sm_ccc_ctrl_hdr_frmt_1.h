#ifndef E2SM_CCC_CTRL_HDR_FRMT_1_E2SM_ccc_H
#define E2SM_CCC_CTRL_HDR_FRMT_1_E2SM_ccc_H

#include <stdbool.h>


// 9.2.1.6.1
typedef struct{
  bool test;
} e2sm_ccc_ctrl_hdr_frmt_1_t;

void free_e2sm_ccc_ctrl_hdr_frmt_1(e2sm_ccc_ctrl_hdr_frmt_1_t* src);

bool eq_e2sm_ccc_ctrl_hdr_frmt_1(e2sm_ccc_ctrl_hdr_frmt_1_t const* m0, e2sm_ccc_ctrl_hdr_frmt_1_t const* m1);

e2sm_ccc_ctrl_hdr_frmt_1_t cp_e2sm_ccc_ctrl_hdr_frmt_1( e2sm_ccc_ctrl_hdr_frmt_1_t const* src);

#endif

