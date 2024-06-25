#ifndef DECRYPTION_MATCHING_CONDITION_KPM_V2_01_H
#define DECRYPTION_MATCHING_CONDITION_KPM_V2_01_H



#include "../../ie/asn/MatchingCondItem.h"
#include "../../ie/kpm_data_ie/data/matching_cond_frm_3_lst.h"

matching_condition_format_3_lst_t kpm_dec_matching_cond_asn(const MatchingCondItem_t * matching_cond);


#endif
