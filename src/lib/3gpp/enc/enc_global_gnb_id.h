#ifndef ENCRYPTION_GLOBAL_GNB_ID_H
#define ENCRYPTION_GLOBAL_GNB_ID_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/global_gnb_id.h"
#include "enc_asn.h"

GlobalGNB_ID_t * enc_global_gnb_id_asn(const global_gnb_id_t * global_gnb_id);

#ifdef __cplusplus
}
#endif

#endif
