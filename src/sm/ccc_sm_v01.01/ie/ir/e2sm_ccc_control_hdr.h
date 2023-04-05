#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_HEADER_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_HEADER_SM_H

#include "e2sm_ccc_control_hdr_frm_1.h"

typedef enum {
    FORMAT_1_CONTROL_HEADER_CCC,

    END_CONTROL_HEADER_CCC

} e2sm_ccc_control_hdr_e;


/*  9.2.1.6  RIC CONTROL HEADER IE  */

typedef struct {
    e2sm_ccc_control_hdr_e type;

    union {
        e2sm_ccc_control_hdr_frm_1_t frm_1;  // 9.2.1.6.1
    };


} e2sm_ccc_control_hdr_t;


#endif
