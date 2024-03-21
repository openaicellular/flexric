#ifndef E2SM_CCC_CTRL_MSG_FRMT_2_H
#define E2SM_CCC_CTRL_MSG_FRMT_2_H


#include <stdbool.h>
#include <stdlib.h>

// 9.2.1.7.2
typedef struct{
  // TODO
  int test;
} e2sm_ccc_ctrl_msg_frmt_2_t;

void free_e2sm_ccc_ctrl_msg_frmt_2( e2sm_ccc_ctrl_msg_frmt_2_t* src);

bool eq_e2sm_ccc_ctrl_msg_frmt_2( e2sm_ccc_ctrl_msg_frmt_2_t const* m0,  e2sm_ccc_ctrl_msg_frmt_2_t const* m1);

e2sm_ccc_ctrl_msg_frmt_2_t cp_e2sm_ccc_ctrl_msg_frmt_2(e2sm_ccc_ctrl_msg_frmt_2_t const* src);

#endif



