if(E2AP_VERSION STREQUAL "E2AP_V1")
  include_directories("../e2ap/v1_01/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
  include_directories("../e2ap/v2_03/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V3")
  include_directories("../e2ap/v3_01/ie/asn")
else()
  message(FATAL_ERROR "Unknown E2AP version")
endif()

add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)

add_library(3gpp_common_ie_obj OBJECT
                                $<TARGET_OBJECTS:3gpp_derived_ie_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
                                )
