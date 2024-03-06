#ifndef ENCRYPTION_GNB_CU_UP_H
#define ENCRYPTION_GNB_CU_UP_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/gnb_cu_up.h"
#include "enc_asn.h"

UEID_GNB_CU_UP_t * enc_gNB_CU_UP_UE_asn(const gnb_cu_up_e2sm_t * gnb_cu_up);

#ifdef __cplusplus
}
#endif

#endif
