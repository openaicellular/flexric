#ifndef CCC_EVENT_TRIGGER_FORMAT_1_SM_H
#define CCC_EVENT_TRIGGER_FORMAT_1_SM_H

#include <stdbool.h>
#include <stdlib.h>

#include "attribute.h"
#include "../../../../util/byte_array.h"

typedef struct{
  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // List of Attributes
  // [0 - 66535]
  size_t sz_attribute;
  attribute_t* attribute;
} ev_trg_node_conf_t;

// 9.2.1.1.1
typedef struct{
  // List of Node-level Configuration Structures
  // [1 - 256]
  size_t sz_ev_trg_node_conf;
  ev_trg_node_conf_t ev_trg_node_conf;
} e2sm_ccc_ev_trg_frmt_1_t;

void free_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t* src);

bool eq_e2sm_ccc_ev_trg_frmt_1(e2sm_ccc_ev_trg_frmt_1_t const* m0, e2sm_ccc_ev_trg_frmt_1_t const* m1);

e2sm_ccc_ev_trg_frmt_1_t cp_e2sm_ccc_ev_trg_frmt_1(  e2sm_ccc_ev_trg_frmt_1_t const* src);

#endif

