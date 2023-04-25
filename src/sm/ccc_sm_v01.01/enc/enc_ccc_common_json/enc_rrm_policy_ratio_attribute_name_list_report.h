#ifndef ENCRYPTION_RRM_POLICY_RATIO_ATTRIBUTE_NAME_LIST_REPORT_H
#define ENCRYPTION_RRM_POLICY_RATIO_ATTRIBUTE_NAME_LIST_REPORT_H

#include <cjson/cJSON.h>
#include "../../ie/ir/o_rrm_policy_ratio_attribute_name.h"

cJSON * enc_ccc_rrm_policy_ratio_attribute_name_report(const o_rrm_policy_ratio_attribute_name_e * rrm, const size_t len);


#endif
