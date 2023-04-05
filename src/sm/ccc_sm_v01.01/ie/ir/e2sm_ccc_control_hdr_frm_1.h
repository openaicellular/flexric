#ifndef CELL_CONFIGURATION_AND_CONTROL_CONTROL_HEADER_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_CONTROL_HEADER_FORMAT_1_SM_H

typedef enum {
    STYLE_1_RIC_SERVICE_CONTROL,  // Used to perform node-level configuration and control at the E2 Node
    STYLE_2_RIC_SERVICE_CONTROL,  // Used to perform cell-level configuration and control at the E2 Node

    END_RIC_SERVICE_CONTROL

} ric_service_control_ccc_e;



/*  9.2.1.6.1  E2SM-CCC Control Header Format 1  */

typedef struct {
    // Mandatory
    // RIC Style Type
    ric_service_control_ccc_e service_control_style_type;

} e2sm_ccc_control_hdr_frm_1_t;


#endif
