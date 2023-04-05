#ifndef RRM_POLICY_MEMBER_H
#define RRM_POLICY_MEMBER_H


#include <stdlib.h>

#include "../../../../lib/e2sm_common_ie/3gpp_derived_ie/plmn_identity.h"
#include "../../../../lib/e2sm_common_ie/3gpp_derived_ie/s_nssai.h"


//  9.3.16  rRMPolicyMember

typedef struct {
    plmn_t plmn_id;
    S_NSSAI_t *s_nssai;

} rrm_policy_member_t;


#endif
