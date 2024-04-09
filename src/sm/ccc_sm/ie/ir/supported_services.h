#ifndef SUPPORTED_SERVICES_H
#define SUPPORTED_SERVICES_H

#include "stdint.h"
#include "../../../../util/byte_array.h"
#include "ev_trigger_ccc.h"
#include "report_service_ccc.h"
#include "control_service_ccc.h"

typedef struct {
  // Empty
} insert_service_ccc_t;

typedef struct {
  // Empty
} policy_service_ccc_t;

typedef struct {
  // Event Trigger
  // Optional
  // 9.2.2.2.1
  ev_trigger_ccc_t* ev_trigger_ccc;

  // Report service
  // Optional
  // 9.2.2.2.2
  report_service_ccc_t* report_service_ccc;

  // Insert service
  // Optional
  // 9.2.2.2.3
  insert_service_ccc_t* insert_service_ccc;

  // Control service
  // Optional
  // 9.2.2.2.4
  control_service_ccc_t* control_service_ccc;

  // Policy service
  // Optional
  // 9.2.2.2.5
  policy_service_ccc_t* policy_service_ccc;

} supported_services_t;

void free_supported_services(supported_services_t* src);

bool eq_supported_services(supported_services_t const* m0, supported_services_t const* m1);

supported_services_t cp_supported_services(supported_services_t const* src);

#endif
