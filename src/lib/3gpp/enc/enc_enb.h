#ifndef ENCRYPTION_ENB_H
#define ENCRYPTION_ENB_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/enb.h"
#include "enc_asn.h"

UEID_ENB_t * enc_eNB_UE_asn(const enb_e2sm_t * enb);

#ifdef __cplusplus
}
#endif

#endif
