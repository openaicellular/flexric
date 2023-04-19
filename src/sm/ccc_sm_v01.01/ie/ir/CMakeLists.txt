set(CCC_RIC_INFO_SRCS
    e2sm_ccc_event_trigger_frm_1.c
    e2sm_ccc_event_trigger_frm_2.c
    e2sm_ccc_event_trigger_frm_3.c
    e2sm_ccc_event_trigger.c
    node_level_ran_conf_struct_report_et_ad.c
    cell_level_ran_conf_struct_report_et_ad.c
)

add_library(ccc_ric_info_obj  OBJECT   ${CCC_RIC_INFO_SRCS})
