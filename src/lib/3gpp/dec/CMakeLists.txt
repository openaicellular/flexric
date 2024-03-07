set(3GPP_DERIVED_IE_DEC_ASN_SRCS
  dec_enb.c
  dec_en_gnb.c
  dec_global_enb_id.c
  dec_global_gnb_id.c
  dec_global_ng_enb.c
  dec_global_ng_ran.c
  dec_gnb.c
  dec_gnb_cu_up.c
  dec_gnb_du.c
  dec_ng_enb.c
  dec_ng_enb_du.c
  )

#####
### KPM
#####

add_library(3gpp_derived_ie_dec_asn_obj_kpm OBJECT
  ${3GPP_DERIVED_IE_DEC_ASN_SRCS} )

if(KPM_VERSION STREQUAL "KPM_V2_01")
  target_include_directories(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE "../../../sm/kpm_sm/kpm_sm_v02.01/ie/asn")
  target_compile_options(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE "-DKPM_V2_01")

elseif(KPM_VERSION STREQUAL "KPM_V2_03")
  target_include_directories(3gpp_derived_ie_dec_asn_obj_kpm  PRIVATE "../../../sm/kpm_sm/kpm_sm_v02.03/ie/asn")
  target_compile_options(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE "-DKPM_V2_03")

elseif(KPM_VERSION STREQUAL "KPM_V3_00")
  target_include_directories(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE "../../../sm/kpm_sm/kpm_sm_v03.00/ie/asn")
  target_compile_options(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE "-DKPM_V3_00")

else()
  message(FATAL_ERROR "Unknown KPM version")
endif()


target_compile_options(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE "-DASN_DISABLE_OER_SUPPORT" "-DASN_DISABLE_JER_SUPPORT")

target_compile_options(3gpp_derived_ie_dec_asn_obj_kpm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC  -fvisibility=hidden)


#####
### RC
#####

add_library(3gpp_derived_ie_dec_asn_obj_rc OBJECT ${3GPP_DERIVED_IE_DEC_ASN_SRCS})

target_include_directories(3gpp_derived_ie_dec_asn_obj_rc PRIVATE "../../../sm/rc_sm/ie/asn")

target_compile_options(3gpp_derived_ie_dec_asn_obj_rc PRIVATE "-DRC_SM -DASN_DISABLE_OER_SUPPORT" "-DASN_DISABLE_JER_SUPPORT")
target_compile_options(3gpp_derived_ie_dec_asn_obj_rc PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC  -fvisibility=hidden)

