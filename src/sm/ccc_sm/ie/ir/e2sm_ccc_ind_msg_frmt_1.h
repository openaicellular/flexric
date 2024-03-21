#ifndef E2SM_CCC_INDICATION_MSG_FORMAT_1_H
#define E2SM_CCC_INDICATION_MSG_FORMAT_1_H

#include <stdbool.h>
#include <stdlib.h>

#include "../../../../util/byte_array.h"
#include "change_type.h"

typedef struct{
  // Change Type
  // Mandatory
  change_type_e change_type;

  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // Values of Attributes
  // Mandatory
  byte_array_t val_attribute;

  // Old Values of Attributes
  // Optional
  byte_array_t old_val_attribute;

} ind_msg_ran_conf_t;

//9.2.1.4.1
typedef struct{
  //  List of Configuration Structures Reported
  //  [1 - 65535]
  size_t sz_ind_msg_ran_conf;
  ind_msg_ran_conf_t* ind_msg_ran_conf;

} e2sm_ccc_ind_msg_frmt_1_t;

void free_e2sm_ccc_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const* src);

bool eq_e2sm_ccc_ind_msg_frmt_1( e2sm_ccc_ind_msg_frmt_1_t const* m0,  e2sm_ccc_ind_msg_frmt_1_t const* m1);

e2sm_ccc_ind_msg_frmt_1_t cp_e2sm_ccc_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const* src);

#endif
