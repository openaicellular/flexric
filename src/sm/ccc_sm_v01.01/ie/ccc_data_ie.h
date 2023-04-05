#ifndef CCC_V01_01_INFORMATION_ELEMENTS_H
#define CCC_V01_01_INFORMATION_ELEMENTS_H

#include "ir/e2sm_ccc_event_trigger.h"
#include "ir/e2sm_ccc_action_def.h"
#include "ir/e2sm_ccc_ind_hdr.h"
#include "ir/e2sm_ccc_ind_msg.h"
#include "ir/e2sm_ccc_control_hdr.h"
#include "ir/e2sm_ccc_control_msg.h"
#include "ir/e2sm_ccc_control_outcome.h"


/*
 * O-RAN defined 5 Procedures: RIC Subscription, RIC Indication, RIC Control, E2 Setup and RIC Service Update 
*/


/*  RIC SUBSCRIPTION  */

typedef struct {
    e2sm_ccc_event_trigger_t et;
    e2sm_ccc_action_def_t ad;

} ccc_sub_data_t;



/*  RIC INDICATION  */

typedef struct {
    e2sm_ccc_ind_hdr_t hdr;
    e2sm_ccc_ind_msg_t msg;

} ccc_ind_data_t;



/*  RIC CONTROL  */

typedef struct {
    e2sm_ccc_control_hdr_t hdr;
    e2sm_ccc_control_msg_t msg;

} ccc_ctrl_req_data_t;

typedef struct {
    e2sm_ccc_control_outcome_t out;

} ccc_ctrl_out_data_t;



/*  E2 SETUP  */


/*  RIC SERVICE UPDATE  */



#endif
