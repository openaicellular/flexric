#ifndef CTRL_MSG_RAN_CONF_H
#define CTRL_MSG_RAN_CONF_H

#include <stdbool.h>

#include "../../../../util/byte_array.h"
#include "values_of_attributes.h"
#include "change_type.h"

typedef struct{
  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // Values of Attributes
  // Mandatory
  values_of_attributes_t* vals_attributes;

  // Old Values of Attributes
  // Mandatory
  values_of_attributes_t* old_vals_attributes;

} ctrl_msg_ran_conf_t;

void free_ctrl_msg_ran_conf(ctrl_msg_ran_conf_t* src);

bool eq_ctrl_msg_ran_conf(ctrl_msg_ran_conf_t const* m0, ctrl_msg_ran_conf_t const* m1);

ctrl_msg_ran_conf_t cp_ctrl_msg_ran_conf(ctrl_msg_ran_conf_t const* src);

#endif

