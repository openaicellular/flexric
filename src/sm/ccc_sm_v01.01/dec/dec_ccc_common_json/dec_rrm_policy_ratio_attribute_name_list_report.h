#ifndef DECRYPTION_RRM_POLICY_RATIO_ATTRIBUTE_NAME_LIST_REPORT_H
#define DECRYPTION_RRM_POLICY_RATIO_ATTRIBUTE_NAME_LIST_REPORT_H

#include <cjson/cJSON.h>
#include "../../ie/ir/o_rrm_policy_ratio_attribute_name.h"

o_rrm_policy_ratio_attribute_name_e * dec_ccc_rrm_policy_ratio_attribute_name_report(const cJSON * attribute_list_json, const size_t len);


#endif
