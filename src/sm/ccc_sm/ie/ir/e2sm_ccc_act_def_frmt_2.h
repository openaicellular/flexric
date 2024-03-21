#ifndef E2SM_CCC_ACTION_DEFINITION_FORMAT_2
#define E2SM_CCC_ACTION_DEFINITION_FORMAT_2

#include <stdbool.h>
#include <stdlib.h>

#include "../../../../lib/sm/ie/cell_global_id.h"
#include "../../../../util/byte_array.h"
#include "report_type.h"
#include "attribute.h"

typedef struct{
  // Report Type
  // Mandatory
  // 9.3.9
  report_type_e report_type;

  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // List attributes
  // [0 - 66535]
  size_t sz_attribute;
  attribute_t* attribute;

} cell_conf_t;

typedef struct{
  // Cell global ID
  // Optional
  // 9.3.6
  cell_global_id_t cell_global_id;

  // List of Cell-level RAN Configuration Structures
  // [1 - 1024]
  size_t sz_cell_conf;
  cell_conf_t* cell_conf;

} act_def_cell_report_t;

// 9.2.1.2.2
typedef struct{
  // List of Cell Configuration to be reported
  // [1-1024]
  size_t sz_act_def_cell_report;
  act_def_cell_report_t* act_def_cell_report;

} e2sm_ccc_act_def_frmt_2_t; 

void free_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t* src);

bool eq_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const* m0, e2sm_ccc_act_def_frmt_2_t const* m1);

e2sm_ccc_act_def_frmt_2_t cp_e2sm_ccc_act_def_frmt_2(e2sm_ccc_act_def_frmt_2_t const* src); 

#endif

