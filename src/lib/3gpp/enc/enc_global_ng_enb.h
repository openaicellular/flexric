#ifndef ENCRYPTION_GLOBAL_NG_ENB_H
#define ENCRYPTION_GLOBAL_NG_ENB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/global_ng_enb_id.h"
#include "enc_asn.h"

GlobalNgENB_ID_t * enc_global_ng_enb_asn(const global_ng_enb_id_t * global_ng_enb_id);

#ifdef __cplusplus
}
#endif

#endif
