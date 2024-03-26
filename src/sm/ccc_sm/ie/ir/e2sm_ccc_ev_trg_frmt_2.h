#ifndef E2SM_CCC_EV_TRG_FRMT_2
#define E2SM_CCC_EV_TRG_FRMT_2

#include <stdbool.h>

#include "../../../../util/byte_array.h"
#include "../../../../lib/sm/ie/cell_global_id.h"
#include "attribute.h"

typedef struct{
  // RAN Configuration Structure Name
  // Mandatory
  // 9.3.7
  byte_array_t ran_conf_name;

  // List attributes
  // [0 - 66535]
  size_t sz_attribute;
  attribute_t* attribute;
} ev_trg_cell_conf_t;

typedef struct{
  // Cell global ID
  // Optional
  // 9.3.6
  cell_global_id_t cell_global_id;

  // List of Cell-level RAN Configuration Structures
  // [1 - 1024]
  size_t sz_ev_trg_cell_conf;
  ev_trg_cell_conf_t* ev_trg_cell_conf;

} ev_trg_cell_t;

// 9.2.1.1.2
typedef struct {
  // List of Cell-level Configuration Structures
  // [1 - 1024]
  size_t sz_ev_trg_cell;
  ev_trg_cell_t* ev_trg_cell;
} e2sm_ccc_ev_trg_frmt_2_t;

void free_e2sm_ccc_ev_trg_frmt_2(e2sm_ccc_ev_trg_frmt_2_t const* src);

bool eq_e2sm_ccc_ev_trg_frmt_2(e2sm_ccc_ev_trg_frmt_2_t const* m0, e2sm_ccc_ev_trg_frmt_2_t const* m1);

e2sm_ccc_ev_trg_frmt_2_t cp_e2sm_ccc_ev_trg_frmt_2(e2sm_ccc_ev_trg_frmt_2_t const* src);

#endif

