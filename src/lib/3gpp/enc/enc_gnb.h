#ifndef ENCRYPTION_GNB_H
#define ENCRYPTION_GNB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/gnb.h"
#include "enc_asn.h"

UEID_GNB_t * enc_gNB_UE_asn(const gnb_e2sm_t * gnb);

#ifdef __cplusplus
}
#endif

#endif
