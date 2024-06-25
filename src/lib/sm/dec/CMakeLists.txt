set(SM_COMMON_DEC_ASN_SRCS
    dec_cell_global_id.c
    dec_ran_func_name.c
    dec_ue_id.c)

#####
### KPM
#####

add_library(sm_common_dec_asn_obj_kpm OBJECT ${SM_COMMON_DEC_ASN_SRCS} )

if(KPM_VERSION STREQUAL "KPM_V2_01")
  target_include_directories(sm_common_dec_asn_obj_kpm PUBLIC "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn")
  target_compile_options(sm_common_dec_asn_obj_kpm PRIVATE "-DKPM_V2_01")
elseif(KPM_VERSION STREQUAL "KPM_V2_03")
  target_include_directories(sm_common_dec_asn_obj_kpm PUBLIC "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn")
  target_compile_options(sm_common_dec_asn_obj_kpm PRIVATE "-DKPM_V2_03")
elseif(KPM_VERSION STREQUAL "KPM_V3_00")
  target_include_directories(sm_common_dec_asn_obj_kpm PUBLIC "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn")
  target_compile_options(sm_common_dec_asn_obj_kpm PRIVATE "-DKPM_V3_00")
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()

target_compile_options(sm_common_dec_asn_obj_kpm PRIVATE  "-DASN_DISABLE_JER_SUPPORT" "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(sm_common_dec_asn_obj_kpm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC -fvisibility=hidden)


#####
### RC
#####

add_library(sm_common_dec_asn_obj_rc OBJECT
                                ${SM_COMMON_DEC_ASN_SRCS} )

target_include_directories(sm_common_dec_asn_obj_rc PUBLIC "../../../sm/rc_sm/ie/asn")

target_compile_options(sm_common_dec_asn_obj_rc PRIVATE "-DRC_SM")

target_compile_options(sm_common_dec_asn_obj_rc PRIVATE  "-DASN_DISABLE_JER_SUPPORT" "-DASN_DISABLE_OER_SUPPORT")
target_compile_options(sm_common_dec_asn_obj_rc PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC -fvisibility=hidden)

