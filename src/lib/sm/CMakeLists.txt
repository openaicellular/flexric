
if(E2AP_VERSION STREQUAL "E2AP_V1")
  include_directories("../e2ap/v1_01/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
  include_directories("../e2ap/v2_03/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V3")
  include_directories("../e2ap/v3_01/ie/asn")
else()
  message(FATAL_ERROR "E2AP unknown version")
endif()

add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)

add_library(e2sm_common_ie_obj OBJECT
                                $<TARGET_OBJECTS:sm_common_ie_obj>
                                $<TARGET_OBJECTS:sm_common_dec_asn_obj>
                                $<TARGET_OBJECTS:sm_common_enc_asn_obj>
                                )

target_link_libraries(e2sm_common_ie_obj PUBLIC
                                        sm_common_ie_obj
                                        sm_common_dec_asn_obj
                                        sm_common_enc_asn_obj)
