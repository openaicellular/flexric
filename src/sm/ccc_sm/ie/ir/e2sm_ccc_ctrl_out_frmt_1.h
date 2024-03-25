#ifndef E2SM_CCC_CTRL_OUT_FRMT_1_H
#define E2SM_CCC_CTRL_OUT_FRMT_1_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "../../../../util/byte_array.h"
#include "cause.h"

typedef struct{
  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // Old Values of Attributes
  // Mandatory
  byte_array_t old_atr_val;

  // Current Values of Attributes
  // Mandatory
  byte_array_t cur_atr_val;

  // Applied Timestamp
  // Optional
  byte_array_t app_timestamp;

} ctrl_out_conf_accepted_t;

typedef struct{
  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // Old Values of Attributes
  // Mandatory
  byte_array_t old_atr_val;

  // Requested Values of Attributes
  // Mandatory
  byte_array_t req_atr_val;

  // Cause
  // Mandatory
  // 9.3.11
  ccc_cause_e cause;

} ctrl_out_conf_failed_t;

// 9.2.1.8.1
typedef struct{
  // Received Timestamp
  // Optional
  byte_array_t rev_timestamp;

  // RAN Configuration Structures Accepted List
  // [0-65535]
  size_t sz_ctrl_out_conf_accepted;
  ctrl_out_conf_accepted_t* ctrl_out_conf_accepted;

  // RAN Configuration Structures Failed List
  // [0-65535]
  size_t sz_ctrl_out_conf_failed;
  ctrl_out_conf_failed_t* ctrl_out_conf_failed;

} e2sm_ccc_ctrl_out_frmt_1_t;

void free_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t* src);

e2sm_ccc_ctrl_out_frmt_1_t cp_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* src);

bool eq_e2sm_ccc_ctrl_out_frmt_1(e2sm_ccc_ctrl_out_frmt_1_t const* m0, e2sm_ccc_ctrl_out_frmt_1_t const* m1);

#endif

