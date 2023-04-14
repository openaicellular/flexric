#include <assert.h>
#include <cjson/cJSON.h>
#include <stdio.h>
#include "/usr/include/x86_64-linux-gnu/bits/types/FILE.h"

#include "enc_ccc_ric_event_trigger_frm_1.h"

cJSON * enc_ccc_event_trigger_frm_1(const e2sm_ccc_ev_trg_frm_1_t * ev_trg)
{
  assert(ev_trg != NULL);

  cJSON *object = cJSON_CreateObject();
  cJSON *event_trigger_name = cJSON_CreateObject();
  cJSON *node_level_conf_list_json = cJSON_CreateArray();


  for (size_t i = 0; i<ev_trg->node_level_conf_list_len; i++)
  {
    cJSON *node_level_conf_item_json = cJSON_CreateObject();
    
    cJSON *attribute_list = cJSON_CreateArray();

    switch (ev_trg->node_level_ran_conf_struct_list[i].type)
    {
    case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(node_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-GnbDuFunction"));
        

        for (size_t j = 0; j<ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
        {
            
            switch (ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j])
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
        cJSON_AddItemToObject(node_level_conf_item_json, "listOfAttributes", attribute_list);
        break;

    case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(node_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-GnbCuCpFunction"));
        

        for (size_t j = 0; j<ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
        {
            
            switch (ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_cp[j])
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
        cJSON_AddItemToObject(node_level_conf_item_json, "listOfAttributes", attribute_list);
        break;
    
    case O_GNB_CU_UP_Function_NODE_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(node_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-GnbCuUpFunction"));
        

        for (size_t j = 0; j<ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
        {
            
            switch (ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_cu_up[j])
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
        cJSON_AddItemToObject(node_level_conf_item_json, "listOfAttributes", attribute_list);
        break;

    case O_RRM_Policy_Ratio_NODE_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(node_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-RRMPolicyRatio"));
        

        for (size_t j = 0; j<ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
        {
            
            switch (ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list.o_rrm_policy_ratio_node[j])
            {
            case RESOURCE_TYPE_ATTRIBUTE_NAME:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("resourceType"));
              break;

            case RRM_POLICY_MEMBER_LIST_ATTRIBUTE_NAME:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyMemberList"));
              break;

            case RRM_POLICY_MAX_RATIO_ATTRIBUTE_NAME:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyMaxRatio"));
              break;

            case RRM_POLICY_MIN_RATIO_ATTRIBUTE_NAME:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyMinRatio"));
              break;

            case RRM_POLICY_DEDICATED_RATIO_ATTRIBUTE_NAME:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("rRMPolicyDedicatedRatio"));
              break;
            
            default:
                break;
            }
        }
        cJSON_AddItemToObject(node_level_conf_item_json, "listOfAttributes", attribute_list);
        break;


    default:
        break;
    }

    cJSON_AddItemToArray(node_level_conf_list_json, node_level_conf_item_json);
  }


  cJSON_AddItemToObject(event_trigger_name, "listOfNodeLevelConfigurationStructuresForEventTrigger", node_level_conf_list_json);

  cJSON_AddItemToObject(object, "E2SM-CCC-EventTriggerDefinition-Format1", event_trigger_name);
  

  return object;
}
