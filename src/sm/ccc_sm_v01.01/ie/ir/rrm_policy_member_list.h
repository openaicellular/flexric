#ifndef RRM_POLICY_MEMBER_LIST_H
#define RRM_POLICY_MEMBER_LIST_H


#include <stdlib.h>

#include "rrm_policy_member.h"


//  9.3.17  rRMPolicyMemberList

typedef struct {
    size_t rrm_policy_member_list_len;
    rrm_policy_member_t *rrm_policy_member;  // 9.3.16

} rrm_policy_member_list_t;


#endif
