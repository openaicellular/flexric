#include <assert.h>

#include "enc_cell_level_ran_conf_struct_report_et_ad.h"
#include "enc_rrm_policy_ratio_attribute_name_list_report.h"


static cJSON * enc_nr_cell_cu_attribute_name_list(const o_nr_cell_cu_attribute_name_e * nr_cell_cu, const size_t len)
{
  assert(nr_cell_cu != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  { 
    switch (nr_cell_cu[i])
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

  return attribute_list;
}

static cJSON * enc_nr_cell_du_attribute_name_list(const o_nr_cell_du_attribute_name_e * nr_cell_du, const size_t len)
{
  assert(nr_cell_du != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  {
    switch (nr_cell_du[i])
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

  return attribute_list;
}

static cJSON * enc_bwp_attribute_name_list(const o_bwp_attribute_name_e * bwp, const size_t len)
{
  assert(bwp != NULL);

  cJSON *attribute_list = cJSON_CreateArray();
  assert(attribute_list != NULL);

  for (size_t i = 0; i<len; i++)
  {
    switch (bwp[i])
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

  return attribute_list;
}

cJSON * enc_ccc_cell_level_ran_conf_struct_report_et_ad(const cell_level_ran_conf_struct_report_et_ad_t * cell_struct)
{
  cJSON *cell_level_conf_item_json = cJSON_CreateObject();
  assert(cell_level_conf_item_json != NULL);

  // Mandatory
  // RAN Configuration Structure Name
  // 9.3.7 => 8.2.2
  switch (cell_struct->type)
  {
  case O_NR_Cell_CU_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-NrCellCu"));

    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.2
    if (cell_struct->attribute_name_list_len != 0)
    {
      assert(cell_struct->attribute_name_list_len >= 1 && cell_struct->attribute_name_list_len <= 65535);

      cJSON *attribute_list = enc_nr_cell_cu_attribute_name_list(cell_struct->attribute_name_list.o_nr_cell_cu_cell, cell_struct->attribute_name_list_len);
      cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
    }
    break;

  case O_NR_Cell_DU_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-NrCellDu"));

    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.2
    if (cell_struct->attribute_name_list_len != 0)
    {
      assert(cell_struct->attribute_name_list_len >= 1 && cell_struct->attribute_name_list_len <= 65535);

      cJSON *attribute_list = enc_nr_cell_du_attribute_name_list(cell_struct->attribute_name_list.o_nr_cell_du_cell, cell_struct->attribute_name_list_len);
      cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
    }
    break;

  case O_BWP_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-Bwp"));

    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.2
    if (cell_struct->attribute_name_list_len != 0)
    {
      assert(cell_struct->attribute_name_list_len >= 1 && cell_struct->attribute_name_list_len <= 65535);

      cJSON *attribute_list = enc_bwp_attribute_name_list(cell_struct->attribute_name_list.o_bwp_cell, cell_struct->attribute_name_list_len); 
      cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
    }
    break;

  case O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT:
    cJSON_AddItemToObject(cell_level_conf_item_json, "ranConfigurationStructureName", cJSON_CreateString("O-RRMPolicyRatio"));
        
    // Optional
    // List of Attributes  -  [0..65535]
    // 8.8.2
    if (cell_struct->attribute_name_list_len != 0)
    {
      assert(cell_struct->attribute_name_list_len >= 1 && cell_struct->attribute_name_list_len <= 65535);

      cJSON *attribute_list = enc_ccc_rrm_policy_ratio_attribute_name_report(cell_struct->attribute_name_list.o_rrm_policy_ratio_cell, cell_struct->attribute_name_list_len);
      cJSON_AddItemToObject(cell_level_conf_item_json, "listOfAttributes", attribute_list);
    }
    break;
    
  default:
    break;
  } 
      
  return cell_level_conf_item_json;
}
