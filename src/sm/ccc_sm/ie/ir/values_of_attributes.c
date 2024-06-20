#include "values_of_attributes.h"

#include <assert.h>
#include <stddef.h>

void free_values_of_attributes(values_of_attributes_t* src)
{
  assert(src != NULL);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction)
    free_e2sm_ccc_o_gnb_cu_cp_function(&src->e2sm_ccc_o_gnb_cu_cp_function);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction)
    free_e2sm_ccc_o_gnb_cu_up_function(&src->e2sm_ccc_o_gnb_cu_up_function);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBDUFunction)
    free_e2sm_ccc_o_gnb_du_function(&src->e2sm_ccc_o_gnb_du_function);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio)
    free_e2sm_ccc_o_rrm_policy_ratio(&src->e2sm_ccc_o_rrm_policy_ratio);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_NRCellCU)
    free_e2sm_ccc_o_nr_cell_cu(&src->e2sm_ccc_o_nr_cell_cu);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_NRCellDU)
    free_e2sm_ccc_o_nr_cell_du(&src->e2sm_ccc_o_nr_cell_du);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP)
    free_e2sm_ccc_o_bwp(&src->e2sm_ccc_o_bwp);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_CESManagementFunction)
    free_e2sm_ccc_o_ces_management_function(&src->e2sm_ccc_o_ces_management_function);
}

bool eq_values_of_attributes(values_of_attributes_t const* m0, values_of_attributes_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->values_of_attributes_type != m1->values_of_attributes_type)
    return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction)
    if (!eq_e2sm_ccc_o_gnb_cu_cp_function(&m0->e2sm_ccc_o_gnb_cu_cp_function, &m1->e2sm_ccc_o_gnb_cu_cp_function))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction)
    if (!eq_e2sm_ccc_o_gnb_cu_up_function(&m0->e2sm_ccc_o_gnb_cu_up_function, &m1->e2sm_ccc_o_gnb_cu_up_function))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBDUFunction)
    if (!eq_e2sm_ccc_o_gnb_du_function(&m0->e2sm_ccc_o_gnb_du_function, &m1->e2sm_ccc_o_gnb_du_function))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio)
    if (!eq_e2sm_ccc_o_rrm_policy_ratio(&m0->e2sm_ccc_o_rrm_policy_ratio, &m1->e2sm_ccc_o_rrm_policy_ratio))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_NRCellCU)
    if (!eq_e2sm_ccc_o_nr_cell_cu(&m0->e2sm_ccc_o_nr_cell_cu, &m1->e2sm_ccc_o_nr_cell_cu))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_NRCellDU)
    if (!eq_e2sm_ccc_o_nr_cell_du(&m0->e2sm_ccc_o_nr_cell_du, &m1->e2sm_ccc_o_nr_cell_du))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP)
    if (!eq_e2sm_ccc_o_bwp(&m0->e2sm_ccc_o_bwp, &m1->e2sm_ccc_o_bwp))
      return false;

  if(m0->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_CESManagementFunction)
    if (!eq_e2sm_ccc_o_ces_management_function(&m0->e2sm_ccc_o_ces_management_function, &m1->e2sm_ccc_o_ces_management_function))
      return false;

  return true;
}


values_of_attributes_t cp_values_of_attributes(values_of_attributes_t const* src)
{
  assert(src != NULL);

  values_of_attributes_t dst = {.values_of_attributes_type = src->values_of_attributes_type};

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBCUCPFunction)
    dst.e2sm_ccc_o_gnb_cu_cp_function = cp_e2sm_ccc_o_gnb_cu_cp_function(&src->e2sm_ccc_o_gnb_cu_cp_function);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBCUUPFunction)
    dst.e2sm_ccc_o_gnb_cu_up_function = cp_e2sm_ccc_o_gnb_cu_up_function(&src->e2sm_ccc_o_gnb_cu_up_function);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_GNBDUFunction)
    dst.e2sm_ccc_o_gnb_du_function = cp_e2sm_ccc_o_gnb_du_function(&src->e2sm_ccc_o_gnb_du_function);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_RRMPolicyRatio)
    dst.e2sm_ccc_o_rrm_policy_ratio= cp_e2sm_ccc_o_rrm_policy_ratio(&src->e2sm_ccc_o_rrm_policy_ratio);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_NRCellCU)
    dst.e2sm_ccc_o_nr_cell_cu = cp_e2sm_ccc_o_nr_cell_cu(&src->e2sm_ccc_o_nr_cell_cu);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_NRCellDU)
    dst.e2sm_ccc_o_nr_cell_du = cp_e2sm_ccc_o_nr_cell_du(&src->e2sm_ccc_o_nr_cell_du);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_BWP)
    dst.e2sm_ccc_o_bwp = cp_e2sm_ccc_o_bwp(&src->e2sm_ccc_o_bwp);

  if (src->values_of_attributes_type == VALUES_OF_ATTRIBUTES_O_CESManagementFunction)
    dst.e2sm_ccc_o_ces_management_function = cp_e2sm_ccc_o_ces_management_function(&src->e2sm_ccc_o_ces_management_function);

  return dst;
}



