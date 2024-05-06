#ifndef KPM_SERVICE_MODEL_AMARISOFT_H
#define KPM_SERVICE_MODEL_AMARISOFT_H 

#include <stdbool.h>

// KPM SM
void init_kpm_sm_amr(void);

void free_kpm_sm_amr(void);

bool read_kpm_sm_amr(void* data);

void read_kpm_setup_sm_amr(void* e2ap);

#endif

