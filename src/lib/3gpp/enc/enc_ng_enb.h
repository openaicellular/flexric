#ifndef ENCRYPTION_NG_ENB_H
#define ENCRYPTION_NG_ENB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/ng_enb.h"
#include "enc_asn.h"

UEID_NG_ENB_t * enc_ng_eNB_UE_asn(const ng_enb_e2sm_t * ng_enb);

#ifdef __cplusplus
}
#endif

#endif
