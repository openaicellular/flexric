set(SM_COMMON_ENC_ASN_SRCS
    enc_cell_global_id.c
    enc_ue_id.c
    enc_ran_function_name.c
    )

include_directories("/usr/local/share/asn1c/")
add_library(sm_common_enc_asn_obj OBJECT 
                                ${SM_COMMON_ENC_ASN_SRCS}
                                )

                              #$<TARGET_OBJECTS:e2sm_kpm_asn1_obj>  

                              #target_link_libraries(sm_common_enc_asn_obj PUBLIC
                              #  e2sm_kpm_asn1_obj)

              
                              target_compile_options(sm_common_enc_asn_obj PRIVATE "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(sm_common_enc_asn_obj PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)
