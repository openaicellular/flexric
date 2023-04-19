#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "node_level_ran_conf_struct_report_et_ad.h"

void free_ccc_node_level_ran_conf_struct_report_et_ad(node_level_ran_conf_struct_report_et_ad_t *src)
{
  assert(src != NULL);

  // Optional
  // List of Attributes
  // 8.8.1
  if (src->attribute_name_list_len != 0)
  {

    switch (src->type)
    {
    case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_gnb_du);
        break;

    case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_gnb_cu_cp);
        break;

    case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_gnb_cu_up);
        break;

    case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_rrm_policy_ratio_node);
        break;
    
    default:
        assert("Unknown Node Level RAN Configuration Structure Name");
    }
  }
}
