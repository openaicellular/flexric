#ifndef E2SM_CCC_CTRL_OUT_FRMT_2_H
#define E2SM_CCC_CTRL_OUT_FRMT_2_H

#include <stdbool.h>
#include <stddef.h>

// 9.2.1.8.2
typedef struct{
  // TODO
} e2sm_ccc_ctrl_out_frmt_2_t;

void free_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t* src);

e2sm_ccc_ctrl_out_frmt_2_t cp_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t const* src);

bool eq_e2sm_ccc_ctrl_out_frmt_2(e2sm_ccc_ctrl_out_frmt_2_t const* m0, e2sm_ccc_ctrl_out_frmt_2_t const* m1);

#endif

