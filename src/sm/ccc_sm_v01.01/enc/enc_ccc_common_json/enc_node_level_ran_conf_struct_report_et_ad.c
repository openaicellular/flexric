#include <assert.h>

#include "enc_node_level_ran_conf_struct_report_et_ad.h"
#include "enc_rrm_policy_ratio_attribute_name_list_report.h"

static cJSON * enc_gnb_du_attribute_name_list(const o_gnb_du_node_attribute_name_e * gnb_du, const size_t len)
{
  assert(gnb_du != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  {
    switch (gnb_du[i])
    {
    case GNB_DU_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbDuId"));
      break;

    case GNB_DU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbDuName"));
      break;

    case GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbId"));
      break;

    case GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbIdLength"));
      break;
            
    default:
        break;
    }
  }


  return attribute_list;
}

static cJSON * enc_gnb_cu_cp_attribute_name_list(const o_gnb_cu_cp_node_attribute_name_e * gnb_cu_cp, const size_t len)
{
  assert(gnb_cu_cp != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  {
    switch (gnb_cu_cp[i])
    {
    case GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbId"));
      break;

    case GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbIdLength"));
      break;

    case GNB_CU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbCuName"));
      break;

    case PLMN_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("plmnId"));
      break;

    case X2_EXCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("x2ExcludeList"));
      break;

    case XN_EXCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("xnExcludeList"));
      break;

    case X2_INCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("x2IncludeList"));
      break;

    case XN_INCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("xnIncludeList"));
      break;
        
    default:
      break;
    }
  }


  return attribute_list;
}

static cJSON * enc_gnb_cu_up_attribute_name_list(const o_gnb_cu_up_node_attribute_name_e * gnb_cu_up, const size_t len)
{
  assert(gnb_cu_up != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  {
    switch (gnb_cu_up[i])
    {
    case GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbId"));
      break;

    case GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbIdLength"));
      break;

    case GNB_CU_UP_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("gnbCuUpId"));
      break;

    case PLMN_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION:
      cJSON_AddItemToArray(attribute_list, cJSON_CreateString("plmnInfoList"));
      break;
            
    default:
      break;
    }
  }


  return attribute_list;
}

cJSON * enc_ccc_node_level_ran_conf_struct_report_et_ad(const node_level_ran_conf_struct_report_et_ad_t * node_struct)
{
  assert(node_struct != NULL);

  cJSON *node_struct_json = cJSON_CreateObject();
  assert(node_struct_json != NULL);

    
  // Mandatory
  // RAN Configuration Structure Name
  // 9.3.7 => 8.2.1
  switch (node_struct->type)
  {
  case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(node_struct_json, "ranConfigurationStructureName", cJSON_CreateString("O-GnbDuFunction"));
        
    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.1
    if (node_struct->attribute_name_list_len != 0)
    {
      assert(node_struct->attribute_name_list_len >= 1 && node_struct->attribute_name_list_len <= 65535);

      cJSON *attribute_list = enc_gnb_du_attribute_name_list(node_struct->attribute_name_list.o_gnb_du, node_struct->attribute_name_list_len);
      cJSON_AddItemToObject(node_struct_json, "listOfAttributes", attribute_list);
    }
    break;

  case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(node_struct_json, "ranConfigurationStructureName", cJSON_CreateString("O-GnbCuCpFunction"));
        
    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.1
    if (node_struct->attribute_name_list_len != 0)
    {
      assert(node_struct->attribute_name_list_len >= 1 && node_struct->attribute_name_list_len <= 65535);
          
      cJSON *attribute_list = enc_gnb_cu_cp_attribute_name_list(node_struct->attribute_name_list.o_gnb_cu_cp, node_struct->attribute_name_list_len);
      cJSON_AddItemToObject(node_struct_json, "listOfAttributes", attribute_list);
    }
    break;
    
  case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(node_struct_json, "ranConfigurationStructureName", cJSON_CreateString("O-GnbCuUpFunction"));
        
    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.1
    if (node_struct->attribute_name_list_len != 0)
    {
      assert(node_struct->attribute_name_list_len >= 1 && node_struct->attribute_name_list_len <= 65535);
          
      cJSON *attribute_list = enc_gnb_cu_up_attribute_name_list(node_struct->attribute_name_list.o_gnb_cu_up, node_struct->attribute_name_list_len);
      cJSON_AddItemToObject(node_struct_json, "listOfAttributes", attribute_list);
    }
    break;

  case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(node_struct_json, "ranConfigurationStructureName", cJSON_CreateString("O-RRMPolicyRatio"));
        
    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.1
    if (node_struct->attribute_name_list_len != 0)
    {
      assert(node_struct->attribute_name_list_len >= 1 && node_struct->attribute_name_list_len <= 65535);
          
      cJSON *attribute_list = enc_ccc_rrm_policy_ratio_attribute_name_report(node_struct->attribute_name_list.o_rrm_policy_ratio_node, node_struct->attribute_name_list_len);
      cJSON_AddItemToObject(node_struct_json, "listOfAttributes", attribute_list);
    }
    break;


  default:
    assert("Unknown Node RAN Configuration Name Structure");
  }



  return node_struct_json;
}
