set(CCC_ENC_COMMON_IE_JSON_SRCS
    enc_node_level_ran_conf_struct_report_et_ad.c
    enc_rrm_policy_ratio_attribute_name_list_report.c
    enc_cell_global_id.c
    enc_cell_level_ran_conf_struct_report_et_ad.c
)

add_library(ccc_enc_common_ie_json_obj  OBJECT  ${CCC_ENC_COMMON_IE_JSON_SRCS})

