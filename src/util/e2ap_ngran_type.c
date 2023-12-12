#include "e2ap_ngran_types.h"

#include <assert.h>
#include <stddef.h>

char* get_e2ap_ngran_name(e2ap_ngran_node_t ran_type)
{
  assert(ran_type >= 0);
  switch (ran_type)
  {
    case e2ap_ngran_eNB: return "ngran_eNB";
    case e2ap_ngran_ng_eNB: return "ngran_ng_eNB";
    case e2ap_ngran_gNB: return "ngran_gNB";
    case e2ap_ngran_eNB_CU: return "ngran_eNB_CU";
    case e2ap_ngran_ng_eNB_CU: return "ngran_ng_eNB_CU";
    case e2ap_ngran_gNB_CU: return "ngran_gNB_CU";
    case e2ap_ngran_eNB_DU: return "ngran_eNB_DU";
    case e2ap_ngran_gNB_DU: return "ngran_gNB_DU";
    case e2ap_ngran_eNB_MBMS_STA: return "ngran_eNB_MBMS_STA";
    case e2ap_ngran_gNB_CUCP: return "ngran_gNB_CUCP";
    case e2ap_ngran_gNB_CUUP: return "ngran_gNB_CUUP";
    default: return NULL;
  }
}