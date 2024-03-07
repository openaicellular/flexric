#ifndef DECRYPTION_GLOBAL_NG_RAN_H
#define DECRYPTION_GLOBAL_NG_RAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../ie/global_ng_ran_node_id.h"
#include "dec_asn.h"

global_ng_ran_node_id_t * dec_global_ng_ran_asn(const GlobalNGRANNodeID_t * global_ng_ran_asn);

#ifdef __cplusplus
}
#endif

#endif
