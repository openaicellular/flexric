#ifndef IND_MSG_RAN_CONF_H
#define IND_MSG_RAN_CONF_H

#include <stdbool.h>

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
  byte_array_t vals_attributes;

  // Old Values of Attributes
  // Optional
  byte_array_t old_vals_attributes;

} ind_msg_ran_conf_t;

void free_ind_msg_ran_conf(ind_msg_ran_conf_t* src);

bool eq_ind_msg_ran_conf(ind_msg_ran_conf_t const* m0, ind_msg_ran_conf_t const* m1);

ind_msg_ran_conf_t cp_ind_msg_ran_conf(ind_msg_ran_conf_t const* src);

#endif

