#ifndef E2SM_CCC_CTRL_OUT_FRMT_1_H
#define E2SM_CCC_CTRL_OUT_FRMT_1_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// 9.2.1.8.1
typedef struct{
  // TODO
    uint32_t test;
} e2sm_ccc_ctrl_out_frmt_1_t;

void free_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t* src);

e2sm_ccc_ctrl_out_frmt_1_t cp_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* src);

bool eq_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* m0, e2sm_ccc_ctrl_out_frmt_1_t const* m1);

#endif

