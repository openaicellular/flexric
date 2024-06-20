#ifndef DECRYPTION_GLOBAL_ENB_ID_H
#define DECRYPTION_GLOBAL_ENB_ID_H


#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/global_enb_id.h"
#include "dec_asn.h"

global_enb_id_t dec_global_enb_id_asn(const GlobalENB_ID_t * global_enb_id_asn);

#ifdef __cplusplus
}
#endif

#endif
