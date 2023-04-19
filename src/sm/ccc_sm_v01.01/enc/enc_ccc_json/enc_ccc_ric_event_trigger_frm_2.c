#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "enc_ccc_ric_event_trigger_frm_2.h"

cJSON * enc_ccc_event_trigger_frm_2(const e2sm_ccc_ev_trg_frm_2_t * ev_trg)
{
  assert(ev_trg != NULL);
  
  char str[1024];

  cJSON *object = cJSON_CreateObject();
  assert(object != NULL);

  cJSON *event_trigger_name = cJSON_CreateObject();
  assert(event_trigger_name != NULL);

  cJSON *cells_list_json = cJSON_CreateArray();
  assert(cells_list_json != NULL);
    
  // Mandatory
  // List of Cell-level Configuration Structures  -  [1..1024]
  assert(ev_trg->cell_level_conf_list_len >= 1 && ev_trg->cell_level_conf_list_len <= 1024);

  for (size_t i = 0; i<ev_trg->cell_level_conf_list_len; i++)
  {
    cJSON *cells_item_json = cJSON_CreateObject();
    assert(cells_item_json != NULL);

    // Optional
    // Cell Global ID
    // 9.3.6
    if (ev_trg->cell_level_conf_list[i].cell_global_id != NULL)
    {
      cJSON *cell_global_id_json = cJSON_CreateObject();
      assert(cell_global_id_json != NULL);
      
      cJSON *plmn_id_json = cJSON_CreateObject();
      assert(plmn_id_json != NULL);
      
      switch (ev_trg->cell_level_conf_list[i].cell_global_id->type)
      {
      case NR_CGI_RAT_TYPE:

        sprintf(str, "%u", ev_trg->cell_level_conf_list[i].cell_global_id->nr_cgi.plmn_id.mcc);
        cJSON_AddItemToObject(plmn_id_json, "mcc", cJSON_CreateString(str));

        sprintf(str, "%u", ev_trg->cell_level_conf_list[i].cell_global_id->nr_cgi.plmn_id.mnc);
        cJSON_AddItemToObject(plmn_id_json, "mnc", cJSON_CreateString(str));

        cJSON_AddItemToObject(cell_global_id_json, "plmnIdentity", plmn_id_json);

        sprintf(str, "%lu", ev_trg->cell_level_conf_list[i].cell_global_id->nr_cgi.nr_cell_id);
        cJSON_AddItemToObject(cell_global_id_json, "NRCellIdentity", cJSON_CreateString(str));

        break;

      case EUTRA_CGI_RAT_TYPE:
        
        sprintf(str, "%u", ev_trg->cell_level_conf_list[i].cell_global_id->eutra.plmn_id.mcc);
        cJSON_AddItemToObject(plmn_id_json, "mcc", cJSON_CreateString(str));

        sprintf(str, "%u", ev_trg->cell_level_conf_list[i].cell_global_id->eutra.plmn_id.mnc);
        cJSON_AddItemToObject(plmn_id_json, "mnc", cJSON_CreateString(str));

        cJSON_AddItemToObject(cell_global_id_json, "plmnIdentity", plmn_id_json);

        sprintf(str, "%u", ev_trg->cell_level_conf_list[i].cell_global_id->eutra.eutra_cell_id);
        cJSON_AddItemToObject(cell_global_id_json, "eUTRACellIdentity", cJSON_CreateString(str));

        break;
      
      default:
        assert(false && "Unknown Cell Global ID Type");
      }
      cJSON_AddItemToObject(cells_item_json, "cellGlobalId", cell_global_id_json);
    }
    
    // Mandatory
    // List of RAN Configuration Structures  -  [1..1024]
    assert(ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len >=1 && ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len <= 1024);

    for (size_t j = 0; j<ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list_len; j++)
    {
      cJSON *cell_level_conf_item_json = cJSON_CreateObject();
      assert(cell_level_conf_item_json != NULL);

      cJSON *attribute_list = cJSON_CreateArray();
      assert(attribute_list != NULL);

      // Mandatory
      // RAN Configuration Structure Name
      // 9.3.7 => 8.2.2
      switch (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].type)
      {
      case O_NR_Cell_CU_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-NrCellCu"));

        // Optional
        // List of Attributes  -  [0..65535]
        // 8.8.2
        if (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len != 0)
        {
          assert(ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len >= 1 && ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len <= 65535);

          for (size_t z = 0; z<ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
          {
            
            switch (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_cu_cell[z])
            {
            case CELL_LOCAL_ID_ATTRIBUTE_NAME_REPORT_NR_CELL_CU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("cellLocalId"));
              break;

            case PLMN_INFO_LIST_ATTRIBUTE_NAME_REPORT_NR_CELL_CU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("plmnInfoList"));
              break;
            
            default:
                break;
            }
          }
          cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
        }
        break;

      case O_NR_Cell_DU_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-NrCellDu"));

        // Optional
        // List of Attributes  -  [0..65535]
        // 8.8.2
        if (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len != 0)
        {
          assert(ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len >= 1 && ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len <= 65535);

          for (size_t z = 0; z<ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
          {
            
            switch (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_nr_cell_du_cell[z])
            {
            case CELL_LOCAL_ID_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("cellLocalId"));
              break;

            case OPERATIONAL_STATE_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("operationalState"));
              break;

            case ADMINISTRATIVE_STATE_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("administrativeState"));
              break;

            case CELL_STATE_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("cellState"));
              break;

            case PLMN_INFO_LIST_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("plmnInfoList"));
              break;

            case N_RPCI_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("nrPci"));
              break;

            case N_RTAC_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("nrTac"));
              break;

            case ARFCN_DL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("arfcnDL"));
              break;

            case ARFCN_UL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("arfcnUL"));
              break;

            case ARFCN_SUL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("arfcnSUL"));
              break;

            case BS_CHANNEL_BW_DL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("bSChannelBwDL"));
              break;

            case SSB_FREQUENCY_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("ssbFrequency"));
              break;

            case SSB_PERIODICITY_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("ssbPeriodicity"));
              break;

            case SSB_SUB_CARRIER_SPACING_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("ssbSubCarrierSpacing"));
              break;

            case SSB_OFFSET_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("ssbOffset"));
              break;

            case SSB_DURATION_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("ssbDuration"));
              break;

            case BS_CHANNEL_BW_UL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("bSChannelBwUL"));
              break;

            case BS_CHANNEL_BW_SUL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("bSChannelBwSUL"));
              break;

            case BWP_LIST_ATTRIBUTE_NAME_REPORT_NR_CELL_DU:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("bwpList"));
              break;
            
            default:
                break;
            }
          }
          cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
        }
        break;

      case O_BWP_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-Bwp"));

        // Optional
        // List of Attributes  -  [0..65535]
        // 8.8.2
        if (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len != 0)
        {
          assert(ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len >= 1 && ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len <= 65535);

          for (size_t z = 0; z<ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
          {
            
            switch (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_bwp_cell[z])
            {
            case BWP_CONTEXT_ATTRIBUTE_NAME_BWP_CELL:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("bwpContext"));
              break;

            case IS_INITIAL_BWP_ATTRIBUTE_NAME_BWP_CELL:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("isInitialBwp"));
              break;

            case SUB_CARRIER_SPACING_ATTRIBUTE_NAME_BWP_CELL:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("subCarrierSpacing"));
              break;

            case CYCLIC_PREFIX_ATTRIBUTE_NAME_BWP_CELL:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("cyclicPrefix"));
              break;

            case START_RB_ATTRIBUTE_NAME_BWP_CELL:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("startRB"));
              break;

            case NUMBER_OF_RBS_ATTRIBUTE_NAME_BWP_CELL:
              cJSON_AddItemToArray(attribute_list, cJSON_CreateString("numberOfRBs"));
              break;
            
            default:
                break;
            }
          }
          cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
        }
        break;

      case O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT:
        cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-RRMPolicyRatio"));
        
        // Optional
        // List of Attributes  -  [0..65535]
        // 8.8.2
        if (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len != 0)
        {
          assert(ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len >= 1 && ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len <= 65535);

          for (size_t z = 0; z<ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list_len; z++)
          {
            
            switch (ev_trg->cell_level_conf_list[i].cell_level_ran_conf_struct_list[j].attribute_name_list.o_rrm_policy_ratio_cell[z])
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
          cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
        }
        break;
      
      default:
        break;
      } 
      

      cJSON_AddItemToArray(cells_item_json, cell_level_conf_item_json);
    }

    cJSON_AddItemToArray(cells_list_json, cells_item_json);
  }

  cJSON_AddItemToObject(event_trigger_name, "listOfCellLevelConfigurationStructuresForEventTrigger", cells_list_json);
  cJSON_AddItemToObject(object, "E2SM-CCC-EventTriggerDefinition-Format2", event_trigger_name);

  return object;
}
