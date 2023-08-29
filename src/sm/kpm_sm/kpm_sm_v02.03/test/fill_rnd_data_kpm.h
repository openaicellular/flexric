#ifndef FILL_RANDOM_DATA_KPM_V2_H
#define FILL_RANDOM_DATA_KPM_V2_H 


#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_event_trigger_def.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_action_def.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_hdr.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ric_ind_msg.h"
#include "../ie/kpm_data_ie/kpm_ric_info/kpm_ran_function_def.h"


kpm_event_trigger_def_t fill_kpm_event_trigger_def(void);
  
kpm_act_def_t fill_kpm_action_def(void);

kpm_ind_hdr_t fill_kpm_ind_hdr(void);

kpm_ind_msg_t fill_kpm_ind_msg(void);

kpm_ran_function_def_t fill_kpm_ran_function(void);

#endif

