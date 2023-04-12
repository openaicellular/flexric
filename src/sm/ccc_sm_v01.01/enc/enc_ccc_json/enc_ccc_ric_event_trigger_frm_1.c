#include <assert.h>

#include "enc_ccc_ric_event_trigger_frm_1.h"

cJSON enc_ccc_event_trigger_frm_1(const e2sm_ccc_ev_trg_frm_1_t * ev_trg)
{
  assert(ev_trg != NULL);

  cJSON *object = cJSON_CreateObject();
  cJSON *node_level_conf_list_json = cJSON_CreateArray();
  cJSON *node_level_conf_item_json = NULL;

  cJSON_AddItemToObject(object, "name", "E2SM-CCC-EventTriggerDefinition-Format1");

  cJSON_AddItemToObject(object, "listOfNodeLevelConfigurationStructuresForEventTrigger", node_level_conf_list_json);

  for (size_t i = 0; i<ev_trg->node_level_conf_list_len; i++)
  {
    node_level_conf_item_json = cJSON_CreateObject();
    
    cJSON *attribute_list = cJSON_CreateArray();

    switch (ev_trg->node_level_ran_conf_struct_list[i].type)
    {
    case O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(node_level_conf_item_json, "ranConfigurationStructureName", "O-GnbDuFunction");
        

        for (size_t j = 0; j<ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
        {
            
            switch (ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list->o_gnb_du[j])
            {
            case GNB_DU_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbDuId");
              break;

            case GNB_DU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbDuName");
              break;

            case GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbId");
              break;

            case GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbIdLength");
              break;
            
            default:
                break;
            }
        }
        cJSON_AddItemToObject(node_level_conf_item_json, "listOfAttributes", attribute_list);
        break;

    case O_GNB_CU_CP_Function_NODE_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(node_level_conf_item_json, "ranConfigurationStructureName", "O-GnbCuCpFunction");
        

        for (size_t j = 0; j<ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list_len; j++)
        {
            
            switch (ev_trg->node_level_ran_conf_struct_list[i].attribute_name_list->o_gnb_cu_cp[j])
            {
            case GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbId");
              break;

            case GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbIdLength");
              break;

            case GNB_CU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "gnbCuName");
              break;

            case PLMN_ID_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "plmnId");
              break;

            case X2_EXCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "x2ExcludeList");
              break;

            case XN_EXCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "xnExcludeList");
              break;

            case X2_INCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "x2IncludeList");
              break;

            case XN_INCLUDE_LIST_NODE_ATTRIBUTE_NAME_O_GNB_CU_CP_FUNCTION:
              cJSON_AddItemToArray(attribute_list, "xnIncludeList");
              break;
            
            default:
                break;
            }
        }
        cJSON_AddItemToObject(node_level_conf_item_json, "listOfAttributes", attribute_list);
        break;
    
    /* to be finished */


    default:
        break;
    }
  }



}
