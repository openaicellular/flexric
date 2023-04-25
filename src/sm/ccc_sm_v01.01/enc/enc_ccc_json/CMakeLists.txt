set(CCC_ENC_IE_JSON_SRCS
    enc_ccc_ric_event_trigger_frm_1.c
    enc_ccc_ric_event_trigger_frm_2.c
    enc_ccc_ric_event_trigger_frm_3.c
    enc_ccc_ric_action_def_frm_1.c
    enc_ccc_ric_action_def_frm_2.c
)

add_library(ccc_enc_ie_json_obj  OBJECT  ${CCC_ENC_IE_JSON_SRCS})

