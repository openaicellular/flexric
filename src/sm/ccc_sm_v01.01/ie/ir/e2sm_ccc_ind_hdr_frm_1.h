#ifndef CELL_CONFIGURATION_AND_CONTROL_INDICATION_HEADER_FORMAT_1_SM_H
#define CELL_CONFIGURATION_AND_CONTROL_INDICATION_HEADER_FORMAT_1_SM_H

#include "../../../../util/byte_array.h"

typedef enum {
    UPON_SUBSCRIPTION_INDICATION_REASON,
    UPON_CHANGE_INDICATION_REASON,
    PERIODIC_INDICATION_REASON,

    END_INDICATION_REASON

} indication_reason_e;


/*  9.2.1.3.1  E2SM-CCC Indication Header Format 1  */

typedef struct {
    // Mandatory
    // Indication Reason
    // Provides the cause of the event trigger
    indication_reason_e indication_reason;

    // Mandatory
    // Event Time - Provides the time of the event trigger.
    // Carries UTC time encoded as the 64-bit timestamp format as defined in section Section 6 of IETF RFC 5905 [5] containing both seconds and fraction parts.
    // 9.3.10
    byte_array_t event_time;


} e2sm_ccc_ind_hdr_frm_1_t;

#endif
