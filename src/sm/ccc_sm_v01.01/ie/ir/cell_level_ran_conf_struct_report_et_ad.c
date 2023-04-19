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
