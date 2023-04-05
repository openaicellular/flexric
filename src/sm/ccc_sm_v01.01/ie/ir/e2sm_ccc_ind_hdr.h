#ifndef CELL_CONFIGURATION_AND_CONTROL_INDICATION_HEADER_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_INDICATION_HEADER_SM_H

#include "e2sm_ccc_ind_hdr_frm_1.h"

typedef enum {
    FORMAT_1_INDICATION_HEADER_CCC,

    END_INDICATION_HEADER_CCC

} e2sm_ccc_ind_hdr_e;


/* 9.2.1.3  RIC INDICATION HEADER IE */

typedef struct {
    e2sm_ccc_ind_hdr_e type;

    union {
        e2sm_ccc_ind_hdr_frm_1_t frm_1;  // 9.2.1.3.1
    };


} e2sm_ccc_ind_hdr_t;

#endif
