#include <assert.h>

#include "enc_rrm_policy_ratio_attribute_name_list_report.h"

cJSON * enc_ccc_rrm_policy_ratio_attribute_name_report(const o_rrm_policy_ratio_attribute_name_e * rrm, const size_t len)
{
  assert(rrm != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  {
            
    switch (rrm[i])
    {
    case RESOURCE_TYPE_ATTRIBUTE_NAME:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("resourceType"));
      break;

    case RRM_POLICY_MEMBER_LIST_ATTRIBUTE_NAME:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyMemberList"));
      break;

    case RRM_POLICY_MAX_RATIO_ATTRIBUTE_NAME:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyMaxRatio"));
      break;

    case RRM_POLICY_MIN_RATIO_ATTRIBUTE_NAME:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyMinRatio"));
      break;

    case RRM_POLICY_DEDICATED_RATIO_ATTRIBUTE_NAME:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyDedicatedRatio"));
      break;
            
    default:
        break;
    }
  }


  return attribute_list;
}
