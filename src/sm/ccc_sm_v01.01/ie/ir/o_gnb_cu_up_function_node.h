#ifndef O_GNB_CU_UP_fUNCTION_NODE_H
#define O_GNB_CU_UP_fUNCTION_NODE_H

#include <stdint.h>

#include "plmn_info_list.h"

typedef enum {
    GNB_ID_ATTRIBUTE_O_GNB_CU_UP_FUNCTION_NODE,
    GNB_ID_LENGTH_ATTRIBUTE_O_GNB_CU_UP_FUNCTION_NODE,
    GNB_CU_UP_ID_ATTRIBUTE_O_GNB_CU_UP_FUNCTION_NODE,
    PLMN_LIST_ATTRIBUTE_O_GNB_CU_UP_FUNCTION_NODE,

    END_ATTRIBUTE_O_GNB_CU_UP_FUNCTION_NODE

} o_gnb_cu_up_attributes_e;  //  for event trigger and action definition


// 8.8.1.3  O-GNBCUUPFunction - REPORT service

typedef union {
    const uint32_t gnb_id;  // It identifies a gNB within a PLMN. The gNB ID is part of the NR Cell Identifier (NCI) of the gNB cells. See "gNB Identifier (gNB ID)" of subclause 8.2 of TS 38.300 [3]. 
    const uint8_t gnb_id_length:5;  // This indicates the number of bits for encoding the gNB ID. See "Global gNB ID" in subclause 9.3.1.6 of TS 38.413 [5].
    const uint64_t gnb_cu_up_id:36;  //  It uniquely identifies the gNB-CU-UP at least within a gNB-CU-CP. See 'gNB-CU-UP ID' in subclause 9.3.1.15 of 3GPP TS 38.463 [48].
    const plmn_info_list_t plmn_list;  // 9.3.15  The PLMNInfoList is a list of PLMNInfo data type. It defines which PLMNs that can be served by the GNBCUUPFunction [6] and which S-NSSAIs can be supported by the GNBCUUPFunction [6] for corresponding PLMN in case of network slicing feature is supported.

} o_gnb_cu_up_function_node_u;  //  for indication message

#endif
