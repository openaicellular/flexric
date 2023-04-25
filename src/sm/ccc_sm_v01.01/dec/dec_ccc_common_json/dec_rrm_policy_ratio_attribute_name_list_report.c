#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "dec_rrm_policy_ratio_attribute_name_list_report.h"

o_rrm_policy_ratio_attribute_name_e * dec_ccc_rrm_policy_ratio_attribute_name_report(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_rrm_policy_ratio_attribute_name_e * rrm_policy_ratio = calloc(len, sizeof(o_rrm_policy_ratio_attribute_name_e));
  assert(rrm_policy_ratio != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "resourceType") == 0)
    {
      rrm_policy_ratio[i] = RESOURCE_TYPE_ATTRIBUTE_NAME;
    }
    else if (strcmp(attribute_item_json->valuestring, "rRMPolicyMemberList") == 0)
    {
      rrm_policy_ratio[i] = RRM_POLICY_MEMBER_LIST_ATTRIBUTE_NAME;
    }
    else if (strcmp(attribute_item_json->valuestring, "rRMPolicyMaxRatio") == 0)
    {
      rrm_policy_ratio[i] = RRM_POLICY_MAX_RATIO_ATTRIBUTE_NAME;
    }
    else if (strcmp(attribute_item_json->valuestring, "rRMPolicyMinRatio") == 0)
    {
      rrm_policy_ratio[i] = RRM_POLICY_MIN_RATIO_ATTRIBUTE_NAME;
    }
    else if (strcmp(attribute_item_json->valuestring, "rRMPolicyDedicatedRatio") == 0)
    {
      rrm_policy_ratio[i] = RRM_POLICY_DEDICATED_RATIO_ATTRIBUTE_NAME;
    }
    else
    {
      assert("Unknown RRM Policy Ratio attribute name");
    }
    ++i;
  }

  return rrm_policy_ratio;
}
