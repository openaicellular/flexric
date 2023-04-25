set(CCC_DEC_COMMON_IE_JSON_SRCS
    dec_node_level_ran_conf_struct_report_et_ad.c
    dec_rrm_policy_ratio_attribute_name_list_report.c
    dec_cell_global_id.c
    dec_cell_level_ran_conf_struct_report_et_ad.c
)

add_library(ccc_dec_common_ie_json_obj  OBJECT  ${CCC_DEC_COMMON_IE_JSON_SRCS})

