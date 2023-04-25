#include <assert.h>

#include "dec_cell_level_ran_conf_struct_report_et_ad.h"
#include "../dec_ccc_common_json/dec_rrm_policy_ratio_attribute_name_list_report.h"

static o_nr_cell_cu_attribute_name_e * dec_nr_cell_cu_attribute_name_list(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_nr_cell_cu_attribute_name_e * nr_cell_cu = calloc(len, sizeof(o_nr_cell_cu_attribute_name_e));
  assert(nr_cell_cu != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "cellLocalId") == 0)
    {
      nr_cell_cu[i] = CELL_LOCAL_ID_ATTRIBUTE_NAME_REPORT_NR_CELL_CU;
    }
    else if (strcmp(attribute_item_json->valuestring, "plmnInfoList") == 0)
    {
      nr_cell_cu[i] = PLMN_INFO_LIST_ATTRIBUTE_NAME_REPORT_NR_CELL_CU;
    }
    else
    {
      assert("Unknown NR Cell CU attribute type");
    }
    ++i;
  }

  return nr_cell_cu;
}

static o_nr_cell_du_attribute_name_e * dec_nr_cell_du_attribute_name_list(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_nr_cell_du_attribute_name_e * nr_cell_du = calloc(len, sizeof(o_nr_cell_du_attribute_name_e));
  assert(nr_cell_du != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "cellLocalId") == 0)
    {
      nr_cell_du[i] = CELL_LOCAL_ID_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "operationalState") == 0)
    {
      nr_cell_du[i] = OPERATIONAL_STATE_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "administrativeState") == 0)
    {
      nr_cell_du[i] = ADMINISTRATIVE_STATE_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "cellState") == 0)
    {
      nr_cell_du[i] = CELL_STATE_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "plmnInfoList") == 0)
    {
      nr_cell_du[i] = PLMN_INFO_LIST_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "nrPci") == 0)
    {
      nr_cell_du[i] = N_RPCI_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "nrTac") == 0)
    {
      nr_cell_du[i] = N_RTAC_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "arfcnDL") == 0)
    {
      nr_cell_du[i] = ARFCN_DL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "arfcnUL") == 0)
    {
      nr_cell_du[i] = ARFCN_UL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "arfcnSUL") == 0)
    {
      nr_cell_du[i] = ARFCN_SUL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "bSChannelBwDL") == 0)
    {
      nr_cell_du[i] = BS_CHANNEL_BW_DL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "ssbFrequency") == 0)
    {
      nr_cell_du[i] = SSB_FREQUENCY_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "ssbPeriodicity") == 0)
    {
      nr_cell_du[i] = SSB_PERIODICITY_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "ssbSubCarrierSpacing") == 0)
    {
      nr_cell_du[i] = SSB_SUB_CARRIER_SPACING_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "ssbOffset") == 0)
    {
      nr_cell_du[i] = SSB_OFFSET_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "ssbDuration") == 0)
    {
      nr_cell_du[i] = SSB_DURATION_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "bSChannelBwUL") == 0)
    {
      nr_cell_du[i] = BS_CHANNEL_BW_UL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "bSChannelBwSUL") == 0)
    {
      nr_cell_du[i] = BS_CHANNEL_BW_SUL_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else if (strcmp(attribute_item_json->valuestring, "bwpList") == 0)
    {
      nr_cell_du[i] = BWP_LIST_ATTRIBUTE_NAME_REPORT_NR_CELL_DU;
    }
    else
    {
      assert("Unknown NR Cell DU attribute type");
    }

    ++i;
  }


  return nr_cell_du;
} 

static o_bwp_attribute_name_e * dec_bwp_attribute_name_list(const cJSON * attribute_list_json, const size_t len)
{
  assert(attribute_list_json != NULL);

  o_bwp_attribute_name_e * bwp = calloc(len, sizeof(o_bwp_attribute_name_e));
  assert(bwp != NULL);

  size_t i = 0;
  const cJSON *attribute_item_json;
  cJSON_ArrayForEach(attribute_item_json, attribute_list_json)
  {
    if (strcmp(attribute_item_json->valuestring, "bwpContext") == 0)
    {
      bwp[i] = BWP_CONTEXT_ATTRIBUTE_NAME_BWP_CELL;
    }
    else if (strcmp(attribute_item_json->valuestring, "isInitialBwp") == 0)
    {
      bwp[i] = IS_INITIAL_BWP_ATTRIBUTE_NAME_BWP_CELL;
    }
    else if (strcmp(attribute_item_json->valuestring, "subCarrierSpacing") == 0)
    {
      bwp[i] = SUB_CARRIER_SPACING_ATTRIBUTE_NAME_BWP_CELL;
    }
    else if (strcmp(attribute_item_json->valuestring, "cyclicPrefix") == 0)
    {
      bwp[i] = CYCLIC_PREFIX_ATTRIBUTE_NAME_BWP_CELL;
    }
    else if (strcmp(attribute_item_json->valuestring, "startRB") == 0)
    {
      bwp[i] = START_RB_ATTRIBUTE_NAME_BWP_CELL;
    }
    else if (strcmp(attribute_item_json->valuestring, "numberOfRBs") == 0)
    {
      bwp[i] = NUMBER_OF_RBS_ATTRIBUTE_NAME_BWP_CELL;
    }
    else
    {
      assert("Unknown BWP attribute type");
    }

    ++i;
  }


  return bwp;
}  

cell_level_ran_conf_struct_report_et_ad_t dec_ccc_cell_level_ran_conf_struct_report_et_ad(const cJSON * ran_conf_item_json)
{
  assert(ran_conf_item_json != NULL);

  cell_level_ran_conf_struct_report_et_ad_t cell_struct = {0};

  const cJSON *ran_conf_name_json = cJSON_GetObjectItem(ran_conf_item_json, "ranConfigurationStructureName");
  assert(cJSON_IsString(ran_conf_name_json));

  const cJSON *attribute_list_json = cJSON_GetObjectItem(ran_conf_item_json, "listOfAttributes");

  if (strcmp(ran_conf_name_json->valuestring, "O-NrCellCu") == 0)
  {
    cell_struct.type = O_NR_Cell_CU_RAN_CONF_NAME_REPORT;

    if (cJSON_IsArray(attribute_list_json))
    {
      cell_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(cell_struct.attribute_name_list_len >= 1 && cell_struct.attribute_name_list_len <= 65535);

      cell_struct.attribute_name_list.o_nr_cell_cu_cell = dec_nr_cell_cu_attribute_name_list(attribute_list_json, cell_struct.attribute_name_list_len);
    }
  }
  else if (strcmp(ran_conf_name_json->valuestring, "O-NrCellDu") == 0)
  {
    cell_struct.type = O_NR_Cell_DU_RAN_CONF_NAME_REPORT;

    if (cJSON_IsArray(attribute_list_json))
    {
      cell_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(cell_struct.attribute_name_list_len >= 1 && cell_struct.attribute_name_list_len <= 65535);

      cell_struct.attribute_name_list.o_nr_cell_du_cell = dec_nr_cell_du_attribute_name_list(attribute_list_json, cell_struct.attribute_name_list_len);
    }
  }
  else if (strcmp(ran_conf_name_json->valuestring, "O-Bwp") == 0)
  {
    cell_struct.type = O_BWP_RAN_CONF_NAME_REPORT;

    if (cJSON_IsArray(attribute_list_json))
    {
      cell_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(cell_struct.attribute_name_list_len >= 1 && cell_struct.attribute_name_list_len <= 65535);

      cell_struct.attribute_name_list.o_bwp_cell = dec_bwp_attribute_name_list(attribute_list_json, cell_struct.attribute_name_list_len);
    }
  }
  else if (strcmp(ran_conf_name_json->valuestring, "O-RRMPolicyRatio") == 0)
  {
    cell_struct.type = O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT;

    if (cJSON_IsArray(attribute_list_json))
    {
      cell_struct.attribute_name_list_len = cJSON_GetArraySize(attribute_list_json);
      assert(cell_struct.attribute_name_list_len >= 1 && cell_struct.attribute_name_list_len <= 65535);

      cell_struct.attribute_name_list.o_rrm_policy_ratio_cell = dec_ccc_rrm_policy_ratio_attribute_name_report(attribute_list_json, cell_struct.attribute_name_list_len);
    }
  }


  return cell_struct;
}
