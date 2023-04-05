#ifndef CELL_CONFIGURATION_AND_CONTROL_ACTION_DEFINITION_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_ACTION_DEFINITION_SM_H


#include "e2sm_ccc_action_def_frm_1.h"
#include "e2sm_ccc_action_def_frm_2.h"

typedef enum{
  STYLE_1_RIC_SERVICE_REPORT_CCC,
  STYLE_2_RIC_SERVICE_REPORT_CCC,

  END_RIC_SERVICE_REPORT_CCC,

} ric_service_report_ccc_e ;


typedef enum {
    FORMAT_1_ACTION_DEFINITION_CCC,
    FORMAT_2_ACTION_DEFINITION_CCC,

    END_ACTION_DEFINITION_CCC

} e2sm_ccc_action_def_e;



/*  9.2.1.2  RIC ACTION DEFINITION IE  */

typedef struct {
    ric_service_report_ccc_e report_style_type;

    e2sm_ccc_action_def_e type;

    union {
        e2sm_ccc_action_def_frm_1_t frm_1;  //  9.2.1.2.1
        e2sm_ccc_action_def_frm_2_t frm_2;  //  9.2.1.2.2
    };

} e2sm_ccc_action_def_t;



#endif
