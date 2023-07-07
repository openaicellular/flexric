if(E2AP_VERSION STREQUAL "E2AP_V1")
  include_directories("../e2ap/v1_01/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
  include_directories("../e2ap/v2_03/ie/asn")
else()
  message(FATAL_ERROR "E2AP V2 and V3 not implemented")
endif()

add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)

add_library(3gpp_common_ie_obj OBJECT
                                $<TARGET_OBJECTS:3gpp_derived_ie_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
                                $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
                                )

target_link_libraries(3gpp_common_ie_obj PUBLIC
                                        3gpp_derived_ie_obj
                                        3gpp_derived_ie_dec_asn_obj
                                        3gpp_derived_ie_enc_asn_obj
                                        )
