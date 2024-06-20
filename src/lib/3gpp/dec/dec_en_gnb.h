#ifndef DECRYPTION_EN_GNB_H
#define DECRYPTION_EN_GNB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/en_gnb.h"
#include "dec_asn.h"

en_gnb_e2sm_t dec_en_gNB_UE_asn(const UEID_EN_GNB_t * en_gnb_asn);

#ifdef __cplusplus
}
#endif

#endif
