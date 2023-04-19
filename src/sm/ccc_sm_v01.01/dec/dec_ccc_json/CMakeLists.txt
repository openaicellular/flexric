set(CCC_DEC_IE_JSON_SRCS
    dec_ccc_ric_event_trigger_frm_1.c
    # dec_ccc_ric_event_trigger_frm_2.c
    dec_ccc_ric_event_trigger_frm_3.c
)

add_library(ccc_dec_ie_json_obj  OBJECT  ${CCC_DEC_IE_JSON_SRCS})

