#ifndef E2SM_CCC_ACTION_DEFINITION_FORMAT_1
#define E2SM_CCC_ACTION_DEFINITION_FORMAT_1

#include <stdbool.h>
#include <stdlib.h>

#include "attribute.h"
#include "../../../../util/byte_array.h"
#include "report_type.h"

typedef struct{
  // Report Type
  // Mandatory
  // 9.3.9
  report_type_e report_type;

  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // [0 - 66535]
  size_t sz_attribute;
  attribute_t* attribute;

} act_def_node_conf_t;

// 9.2.1.2.2
typedef struct{
  // List of Node-Level RAN configuration Structures
  // [1-256]
  size_t sz_act_def_node_conf;
  act_def_node_conf_t* act_def_node_conf;

} e2sm_ccc_act_def_frmt_1_t; 

void free_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t* src);

bool eq_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t const* m0, e2sm_ccc_act_def_frmt_1_t const* m1);

e2sm_ccc_act_def_frmt_1_t cp_e2sm_ccc_act_def_frmt_1(e2sm_ccc_act_def_frmt_1_t const* src);

#endif

