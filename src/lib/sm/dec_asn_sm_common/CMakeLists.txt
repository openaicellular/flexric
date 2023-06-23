set(SM_COMMON_DEC_ASN_SRCS
    dec_cell_global_id.c
    dec_ran_func_name.c
    dec_ue_id.c)

add_library(sm_common_dec_asn_obj OBJECT
                                ${SM_COMMON_DEC_ASN_SRCS}
                                )
target_compile_options(sm_common_dec_asn_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(sm_common_dec_asn_obj PRIVATE "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(sm_common_dec_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
