#include <assert.h>

#include "dec_ccc_ric_event_trigger_frm_1.h"

e2sm_ccc_ev_trg_frm_1_t dec_ccc_event_trigger_frm_1(const cJSON * ev_trg_json)
{
  assert(ev_trg_json != NULL);

  e2sm_ccc_ev_trg_frm_1_t ev_trg_frm_1 = {0};

  const cJSON *node_level_conf_list_json = cJSON_GetObjectItem(ev_trg_json, "listOfNodeLevelConfigurationStructuresForEventTrigger");
  assert(cJSON_IsArray(node_level_conf_list_json));
  
  ev_trg_frm_1.node_level_conf_list_len = (size_t)cJSON_GetArraySize(node_level_conf_list_json);
  assert(ev_trg_frm_1.node_level_conf_list_len >= 1 && ev_trg_frm_1.node_level_conf_list_len <= 256);

  ev_trg_frm_1.node_level_ran_conf_struct_list = calloc(ev_trg_frm_1.node_level_conf_list_len, sizeof(node_level_ran_conf_struct_report_et_ad_t));
  assert(ev_trg_frm_1.node_level_ran_conf_struct_list != NULL);

  const cJSON *node_level_conf_item_json;

  size_t i = 0;
  cJSON_ArrayForEach(node_level_conf_item_json, node_level_conf_list_json)
  {
    const cJSON *ran_conf_name_json = cJSON_GetObjectItem(node_level_conf_item_json, "ranConfigurationStructureName");
    assert(cJSON_IsString(ran_conf_name_json));

    const cJSON *attribute_list_json = cJSON_GetObjectItem(node_level_conf_item_json, "listOfAttributes");
    
    
    if (strcmp(ran_conf_name_json->string, "O-GnbDuFunction") == 0)
    {
      ev_trg_frm_1.node_level_ran_conf_struct_list[i].type = O_GNB_DU_Function_NODE_RAN_CONF_NAME_REPORT;
      
      if (cJSON_IsObject(attribute_list_json))
      {
        assert(cJSON_IsArray(attribute_list_json));
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
        assert(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len >= 1 && ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len <= 65535);

        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du = calloc(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len, sizeof(o_gnb_du_node_attribute_name_e));
        assert(ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du != NULL);

        size_t j = 0;
        const cJSON *attribute_item_json;
        cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
        {
          if (strcmp(attribute_item_json->string, "gnbDuId"))
          {
            ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j] = GNB_DU_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
          }
          else if (strcmp(attribute_item_json->string, "gnbDuName"))
          {
            ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j] = GNB_DU_NAME_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
          }
          else if (strcmp(attribute_item_json->string, "gnbId"))
          {
            ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j] = GNB_ID_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
          }
          else if (strcmp(attribute_item_json->string, "gnbIdLength"))
          {
            ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du[j] = GNB_ID_LENGTH_NODE_ATTRIBUTE_NAME_O_GNB_DU_FUNCTION;
          }
          else
          {
            assert("Unknown gNB DU attribute name");
          }
          
        }
        j++;

      }
      else
      {
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list_len = 0;
        ev_trg_frm_1.node_level_ran_conf_struct_list[i].attribute_name_list.o_gnb_du = NULL;
      }
    }
    else if (strcmp(ran_conf_name_json->string, "O-GnbCuCpFunction") == 0)
    {
      /* code */
    }
    


  i++;
  }





  return ev_trg_frm_1;
}
