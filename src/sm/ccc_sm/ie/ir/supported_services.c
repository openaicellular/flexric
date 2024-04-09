#include "supported_services.h"

#include <assert.h>
#include <stdlib.h>

void free_supported_services(supported_services_t* src)
{
  assert(src != NULL);

  if (src->ev_trigger_ccc != NULL)
    free_ev_trigger_ccc(src->ev_trigger_ccc);

  if (src->report_service_ccc != NULL)
    free_report_service_ccc(src->report_service_ccc);

  if (src->insert_service_ccc != NULL)
    assert(0 != 0 && "Not implemented");

  if (src->control_service_ccc != NULL)
    free_control_service_ccc(src->control_service_ccc);

  if (src->policy_service_ccc != NULL)
    assert(0 != 0 && "Not implemented");

}

bool eq_supported_services(supported_services_t const* m0, supported_services_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->ev_trigger_ccc != NULL && m1->ev_trigger_ccc!= NULL)
    return eq_ev_trigger_ccc(m0->ev_trigger_ccc, m1->ev_trigger_ccc);

  if(m0->report_service_ccc != NULL && m1->report_service_ccc != NULL)
    return eq_report_service_ccc(m0->report_service_ccc, m1->report_service_ccc);

  if(m0->insert_service_ccc != NULL && m1->insert_service_ccc != NULL)
    assert(0 != 0 && "Not implemented");

  if(m0->control_service_ccc != NULL && m1->control_service_ccc!= NULL)
    return eq_control_service_ccc(m0->control_service_ccc, m1->control_service_ccc);

  if(m0->policy_service_ccc != NULL && m1->policy_service_ccc != NULL)
    assert(0 != 0 && "Not implemented");

  return true;
}

supported_services_t cp_supported_services(supported_services_t const* src)
{
  assert(src != NULL);

  supported_services_t dst = {0};

  if (src->ev_trigger_ccc != NULL){
    dst.ev_trigger_ccc = calloc(1, sizeof(ev_trigger_ccc_t));
    assert(dst.ev_trigger_ccc != NULL && "Memory exhausted");
    *dst.ev_trigger_ccc = cp_ev_trigger_ccc(src->ev_trigger_ccc);
  }

  if (src->report_service_ccc != NULL){
    dst.report_service_ccc = calloc(1, sizeof(report_service_ccc_t));
    assert(dst.report_service_ccc != NULL && "Memory exhausted");
    *dst.report_service_ccc = cp_report_service_ccc(src->report_service_ccc);
  }

  if (src->insert_service_ccc != NULL)
    assert(0 != 0 && "Not implemented");

  if (src->control_service_ccc != NULL){
    dst.control_service_ccc = calloc(1, sizeof(control_service_ccc_t));
    assert(dst.control_service_ccc != NULL && "Memory exhausted");
    *dst.control_service_ccc = cp_control_service_ccc(src->control_service_ccc);
  }

  if (src->policy_service_ccc != NULL)
    assert(0 != 0 && "Not implemented");

  return dst;
}