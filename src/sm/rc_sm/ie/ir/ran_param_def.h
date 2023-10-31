#ifndef RAN_PARAM_DEFINITION_H
#define RAN_PARAM_DEFINITION_H

#include <stdint.h>
#include "../../../../util/byte_array.h"

typedef struct ran_param_def_s ran_param_def_t;

typedef struct {
  // RAN Parameter ID
  // Mandatory
  // 9.3.8
  uint32_t ran_param_id;

  // RAN Parameter Name
  // Mandatory
  // 9.3.9
  byte_array_t ran_param_name;

  // RAN Parameter Definition
  // Optional
  // 9.3.51
  ran_param_def_t* ran_param_def;

} ran_param_lst_struct_t;

typedef struct {
  // [1-65535]
  size_t sz_ran_param;
  ran_param_lst_struct_t* ran_param;

} ran_param_type_t;

typedef enum {
  LIST_RAN_PARAMETER_DEF_TYPE,
  STRUCTURE_RAN_PARAMETER_DEF_TYPE,

  END_RAN_PARAMETER_DEF_TYPE

} ran_parameter_def_type_e;

// 9.3.51
typedef struct ran_param_def_s{
  
  // RAN Parameter Type
  // Mandatory
  ran_parameter_def_type_e type;

  union {
    ran_param_type_t* lst;
    ran_param_type_t* strct;
  };
  
} ran_param_def_t;

void free_ran_param_def(ran_param_def_t* src);

bool eq_ran_param_def(ran_param_def_t const* m0, ran_param_def_t const* m1);

#endif
