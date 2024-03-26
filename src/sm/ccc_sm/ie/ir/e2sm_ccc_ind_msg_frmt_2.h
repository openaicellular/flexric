#ifndef E2SM_CCC_INDICATION_MSG_FORMAT_2_H
#define E2SM_CCC_INDICATION_MSG_FORMAT_2_H

#include <stdbool.h>

#include "../../../../lib/sm/ie/cell_global_id.h"
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

} ind_msg_cell_conf_t;

typedef struct {
  // Cell global id
  // Mandatory
  // 9.3.6
  cell_global_id_t cell_global_id;

  // List of Configuration Structures Reported
  // [1 - 65535]
  size_t sz_ind_msg_cell_conf;
  ind_msg_cell_conf_t* ind_msg_cell_conf;

} ind_msg_cell_report_t;

//9.2.1.4.2
typedef struct{
  // List of Cells Reported
  // [1 - 65535]
  size_t sz_ind_msg_cell_report;
  ind_msg_cell_report_t* ind_msg_cell_report;

} e2sm_ccc_ind_msg_frmt_2_t;

void free_e2sm_ccc_ind_msg_frmt_2(e2sm_ccc_ind_msg_frmt_2_t const* src);

bool eq_e2sm_ccc_ind_msg_frmt_2( e2sm_ccc_ind_msg_frmt_2_t const* m0,  e2sm_ccc_ind_msg_frmt_2_t const* m1);

e2sm_ccc_ind_msg_frmt_2_t cp_e2sm_ccc_ind_msg_frmt_2(e2sm_ccc_ind_msg_frmt_2_t const* src);

#endif

