#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "dec_node_level_ran_conf_struct_report_et_ad.h"
#include "dec_rrm_policy_ratio_attribute_name_list_report.h"


static o_gnb_du_node_attribute_name_e * dec_gnb_du_attribute_name_list(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_gnb_du_node_attribute_name_e * gnb_du = calloc(len, sizeof(o_gnb_du_node_attribute_name_e));
  assert(gnb_du != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "gnbDuId") == 0)
    {
      gnb_du[i] = GNB_DU_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbDuName") == 0)
    {
      gnb_du[i] = GNB_DU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbId") == 0)
    {
      gnb_du[i] = GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbIdLength") == 0)
    {
      gnb_du[i] = GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
    }
    else
    {
      assert("Unknown gNB DU attribute name");
    }
    ++i;
  }

  return gnb_du;
}

static o_gnb_cu_cp_node_attribute_name_e * dec_gnb_cu_cp_attribute_name_list(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_gnb_cu_cp_node_attribute_name_e * gnb_cu_cp = calloc(len, sizeof(o_gnb_cu_cp_node_attribute_name_e));
  assert(gnb_cu_cp != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "gnbId") == 0)
    {
      gnb_cu_cp[i] = GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbIdLength") == 0)
    {
      gnb_cu_cp[i] = GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbCuName") == 0)
    {
      gnb_cu_cp[i] = GNB_CU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "plmnId") == 0)
    {
      gnb_cu_cp[i] = PLMN_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "x2ExcludeList") == 0)
    {
      gnb_cu_cp[i] = X2_EXCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "xnExcludeList") == 0)
    {
      gnb_cu_cp[i] = XN_EXCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "x2IncludeList") == 0)
    {
      gnb_cu_cp[i] = X2_INCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "xnIncludeList") == 0)
    {
      gnb_cu_cp[i] = XN_INCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION;
    }
    else
    {
      assert("Unknown gNB CU CP attribute name");
    }
    ++i;
  }

  return gnb_cu_cp;
}

static o_gnb_cu_up_node_attribute_name_e * dec_gnb_cu_up_attribute_name_list(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_gnb_cu_up_node_attribute_name_e * gnb_cu_up = calloc(len, sizeof(o_gnb_cu_up_node_attribute_name_e));
  assert(gnb_cu_up != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "gnbId") == 0)
    {
      gnb_cu_up[i] = GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbIdLength") == 0)
    {
      gnb_cu_up[i] = GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "gnbCuUpId") == 0)
    {
      gnb_cu_up[i] = GNB_CU_UP_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
    }
    else if (strcmp(attribute_item_json->valuestring, "plmnInfoList") == 0)
    {
      gnb_cu_up[i] = PLMN_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_UP_FUNCTION;
    }
    else
    {
      assert("Unknown gNB CU UP attribute name");
    }
    ++i;
  }


  return gnb_cu_up;
}

node_level_ran_conf_struct_report_et_ad_t dec_ccc_node_level_ran_conf_struct_report_et_ad(const cJSON * node_struct_json)
{
  assert(node_struct_json != NULL);

  node_level_ran_conf_struct_report_et_ad_t node_struct = {0};

  const cJSON *ran_conf_name_json = cJSON_GetObjectItem(node_struct_json, "ranConfigurationStructureName");
  assert(cJSON_IsString(ran_conf_name_json));

  const cJSON *attribute_list_json = cJSON_GetObjectItem(node_struct_json, "listOfAttributes");
    
    
  if (strcmp(ran_conf_name_json->valuestring, "O-GnbDuFunction") == 0)
  {
    node_struct.type = O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT;
      
    if (cJSON_IsArray(attribute_list_json))
    {
      node_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(node_struct.attribute_name_list_len >= 1 && node_struct.attribute_name_list_len <= 65535);

      node_struct.attribute_name_list.o_gnb_du = dec_gnb_du_attribute_name_list(attribute_list_json, node_struct.attribute_name_list_len);
    }
    else
    {
      node_struct.attribute_name_list_len = 0;
      node_struct.attribute_name_list.o_gnb_du = NULL;
    }
  }
  else if (strcmp(ran_conf_name_json->valuestring, "O-GnbCuCpFunction") == 0)
  {
    node_struct.type = O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT;
      
    if (cJSON_IsArray(attribute_list_json))
    {
      node_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(node_struct.attribute_name_list_len >= 1 && node_struct.attribute_name_list_len <= 65535);

      node_struct.attribute_name_list.o_gnb_cu_cp = dec_gnb_cu_cp_attribute_name_list(attribute_list_json, node_struct.attribute_name_list_len);
    }
    else
    {
      node_struct.attribute_name_list_len = 0;
      node_struct.attribute_name_list.o_gnb_cu_cp = NULL;
    }
  }
  else if (strcmp(ran_conf_name_json->valuestring, "O-GnbCuUpFunction") == 0)
  {
    node_struct.type = O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT;
      
      if (cJSON_IsArray(attribute_list_json))
      {
        node_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
        assert(node_struct.attribute_name_list_len >= 1 && node_struct.attribute_name_list_len <= 65535);

        node_struct.attribute_name_list.o_gnb_cu_up = dec_gnb_cu_up_attribute_name_list(attribute_list_json, node_struct.attribute_name_list_len);
      }
      else
      {
        node_struct.attribute_name_list_len = 0;
        node_struct.attribute_name_list.o_gnb_cu_up = NULL;
      }
  }
  else if (strcmp(ran_conf_name_json->valuestring, "O-RRMPolicyRatio") == 0)
  {
    node_struct.type = O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT;

    if (cJSON_IsArray(attribute_list_json))
    {
      node_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(node_struct.attribute_name_list_len >= 1 && node_struct.attribute_name_list_len <= 65535);

      node_struct.attribute_name_list.o_rrm_policy_ratio_node = dec_ccc_rrm_policy_ratio_attribute_name_report(attribute_list_json, node_struct.attribute_name_list_len);
    }
    else
    {
      node_struct.attribute_name_list_len = 0;
      node_struct.attribute_name_list.o_rrm_policy_ratio_node = NULL;
    }
  }


  return node_struct;
}
