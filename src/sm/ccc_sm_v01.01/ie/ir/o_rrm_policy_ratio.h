#ifndef O_RRM_POLICY_RATIO_NODE_H
#define O_RRM_POLICY_RATIO_NODE_H

#include "rrm_policy_member_list.h"

typedef enum {
    RESOURCE_TYPE_ATTRIBUTE_O_RRM_POLICY_RATIO,
    RRM_POLICY_MEMBER_LIST_ATTRIBUTE_O_RRM_POLICY_RATIO,
    RRM_POLICY_MAX_RATIO_ATTRIBUTE_O_RRM_POLICY_RATIO,
    RRM_POLICY_MIN_RATIO_ATTRIBUTE_O_RRM_POLICY_RATIO,
    RRM_POLICY_DEDICATED_RATIO_ATTRIBUTE_O_RRM_POLICY_RATIO,

    END_ATTRIBUTE_O_RRM_POLICY_RATIO

} o_rrm_policy_ratio_attributes_e;  //  for event trigger and action definition


typedef enum {
    PRB_UL_RESOURCE_TYPE,
    PRB_DL_RESOURCE_TYPE,
    DRB_RESOURCE_TYPE,
    RRC_RESOURCE_TYPE,

    END_RESOURCE_TYPE

} resource_type_e;



//  8.8.1.4  O-RRMPolicyRatio - REPORT & CONTROL services

typedef union {
    const resource_type_e type;  //  This IE should be included in RIC Services for enabling association with the correct instance of the O-RRMPolicyRatio RAN configuration structure.
    rrm_policy_member_list_t rrm_policy_member_list;  // 9.3.17
    uint8_t rrm_policy_max_ratio:7;  // [1..100] This attribute specifies the maximum percentage of radio resources that can be used by the associated rRMPolicyMemberList. The maximum percentage of radio resources include at least one of the shared resources, prioritized resources and dedicated resources.
    uint8_t rrm_policy_min_ratio:7;  // [1..100] This attribute specifies the minimum percentage of radio resources that can be used by the associated rRMPolicyMemberList. The minimum percentage of radio resources including at least one of prioritized resources and dedicated resources.
    uint8_t rrm_policy_dedicated_ratio:7;  // [1..100] This attribute specifies the percentage of radio resource that dedicatedly used by the associated  rRMPolicyMemberList. The sum of the ‘rRMPolicyDedicatedRatio’ values assigned to all RRMPolicyRatio(s) name-contained by same MangedEntity shall be less or equal 100.


} o_rrm_policy_ratio_u;  //  for indication message


#endif
