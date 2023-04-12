#ifndef O_GNB_CU_UP_NODE_ATTRIBUTE_NAME_H
#define O_GNB_CU_UP_NODE_ATTRIBUTE_NAME_H

#include <stdint.h>


// 8.8.1.3  O-GNBCUUPFunction - REPORT service

typedef enum {
    GNB_ID_NODE_ATTRIBUTE_NAME,
    GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME,
    GNB_CU_UP_ID_NODE_ATTRIBUTE_NAME,
    PLMN_LIST_NODE_ATTRIBUTE_NAME,

    END_GNB_CU_UP_NODE_ATTRIBUTE_NAME

} o_gnb_cu_up_node_attribute_name_e;  //  for event trigger and action definition



#endif
