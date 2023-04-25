#include <assert.h>

#include "cell_level_ran_conf_struct_report_et_ad.h"

void free_ccc_cell_level_ran_conf_struct_report_et_ad(cell_level_ran_conf_struct_report_et_ad_t *src)
{
  assert(src != NULL);

  // Optional
  // List of Attributes
  // 8.8.2
  if (src->attribute_name_list_len != 0)
  {

    switch (src->type)
    {
    case O_NR_Cell_CU_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_nr_cell_cu_cell);
        break;

    case O_NR_Cell_DU_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_nr_cell_du_cell);
        break;

    case O_BWP_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_bwp_cell);
        break;

    case O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT:
        free(src->attribute_name_list.o_rrm_policy_ratio_cell);
        break;
    
    default:
        assert("Unknown Cell Level RAN Configuration Structure Name");
    }
  }  
}

bool eq_ccc_cell_level_ran_conf_struct_report_et_ad(cell_level_ran_conf_struct_report_et_ad_t const *m0, cell_level_ran_conf_struct_report_et_ad_t const *m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Mandatory
  // RAN Configuration Structure Name
  // 9.3.7 => 8.2.2
  if (m0->type != m1->type)
    return false;


  // Optional
  // List of Attributes
  // 8.8.2
  if (m0->attribute_name_list_len != m1->attribute_name_list_len)
    return false;

  if (m0->attribute_name_list_len != 0)
  {
    switch (m0->type)
    {
    case O_NR_Cell_CU_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_nr_cell_cu_cell[i] != m1->attribute_name_list.o_nr_cell_cu_cell[i])
          return false;
      }
      break;

    case O_NR_Cell_DU_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_nr_cell_du_cell[i] != m1->attribute_name_list.o_nr_cell_du_cell[i])
          return false;
      }
      break;

    case O_BWP_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_bwp_cell[i] != m1->attribute_name_list.o_bwp_cell[i])
          return false;
      }
      break;

    case O_RRM_Policy_Ratio_CELL_RAN_CONF_NAME_REPORT:
      for (size_t i = 0; i<m0->attribute_name_list_len; i++)
      {
        if (m0->attribute_name_list.o_rrm_policy_ratio_cell[i] != m1->attribute_name_list.o_rrm_policy_ratio_cell[i])
          return false;
      }
      break;
    
    default:
      assert("Unknown Cell Level RAN Configuration Structure Name");
    }
  }

  return true;
}
