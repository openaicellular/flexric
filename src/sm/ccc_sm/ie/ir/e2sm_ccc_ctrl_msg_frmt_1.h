#ifndef E2SM_CCC_CTRL_MSG_FRMT_1_H
#define E2SM_CCC_CTRL_MSG_FRMT_1_H

#include <stdlib.h>
#include <stdbool.h>

// 9.2.1.7.1
typedef struct{
  // TODO
  int test;
} e2sm_ccc_ctrl_msg_frmt_1_t;

void free_e2sm_ccc_ctrl_msg_frmt_1( e2sm_ccc_ctrl_msg_frmt_1_t* src);

bool eq_e2sm_ccc_ctrl_msg_frmt_1(e2sm_ccc_ctrl_msg_frmt_1_t const* m0, e2sm_ccc_ctrl_msg_frmt_1_t const* m1);

e2sm_ccc_ctrl_msg_frmt_1_t cp_e2sm_ccc_ctrl_msg_frmt_1(e2sm_ccc_ctrl_msg_frmt_1_t const* src);

#endif

