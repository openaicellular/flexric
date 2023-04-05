#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_OUTCOME_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_OUTCOME_SM_H

#include "e2sm_ccc_control_outcome_frm_1.h"
#include "e2sm_ccc_control_outcome_frm_2.h"

typedef enum {
    FORMAT_1_CONTROL_OUTCOME_CCC,
    FORMAT_2_CONTROL_OUTCOME_CCC,

    END_CONTROL_OUTCOME_CCC

} e2sm_ccc_control_outcome_e;


/*  9.2.1.8  RIC CONTROL OUTCOME IE  */

typedef struct {
    e2sm_ccc_control_outcome_e type;

    union {
        e2sm_ccc_control_outcome_frm_1_t frm_1;  // 9.2.1.8.1
        e2sm_ccc_control_outcome_frm_2_t frm_2;  // 9.2.1.8.2
    };


} e2sm_ccc_control_outcome_t;


#endif
