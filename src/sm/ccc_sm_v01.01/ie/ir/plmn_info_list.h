#ifndef PLMN_INFORMATION_LIST_H
#define PLMN_INFORMATION_LIST_H

#include <stdlib.h>

#include "plmn_item.h"

//  9.3.15  pLMNInfoList

typedef struct {
    size_t plmn_info_list_len;  // [1..65536]
    plmn_item_t *plmn_item;  // 9.3.14

} plmn_info_list_t;


#endif
