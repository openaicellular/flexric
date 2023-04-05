#ifndef PLMN_ITEM_H
#define PLMN_ITEM_H

#include "../../../../lib/e2sm_common_ie/3gpp_derived_ie/plmn_identity.h"
#include "../../../../lib/e2sm_common_ie/3gpp_derived_ie/s_nssai.h"

//  9.3.14  pLMNInfo

typedef struct {
    plmn_t plmn_id;
    S_NSSAI_t *s_nssai;

} plmn_item_t;

#endif
