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

bool eq_ccc_node_level_ran_conf_struct_report_et_ad(node_level_ran_conf_struct_report_et_ad_t const *m0, node_level_ran_conf_struct_report_et_ad_t const *m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // RAN Configuration Structure Name
  // 9.3.7 => 8.2.1
  if (m0->type != m1->type)
    return false;


  // Optional
  // List of Attributes
  // 8.8.1
  if (m0->attribute_name_list_len != m1->attribute_name_list_len)
    return false;

  if (m0->attribute_name_list_len != 0)
  {
    switch (m0->type)
    {
    case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_gnb_du[i] != m1->attribute_name_list.o_gnb_du[i])
          return false;
      }
      break;
    
    case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_gnb_cu_cp[i] != m1->attribute_name_list.o_gnb_cu_cp[i])
          return false;
      }
      break;

    case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_gnb_cu_up[i] != m1->attribute_name_list.o_gnb_cu_up[i])
          return false;
      }
      break;

    case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_rrm_policy_ratio_node[i] != m1->attribute_name_list.o_rrm_policy_ratio_node[i])
          return false;
      }
      break;
    
    default:
      assert("Unknown Node Level RAN Configuration Structure Name");
    }
  }



  return true;
}
