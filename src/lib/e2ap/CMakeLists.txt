if(E2AP_VERSION STREQUAL "E2AP_V1")
  add_subdirectory(v1_01)
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
  add_subdirectory(v2_03)
else()
  message(FATAL_ERROR "E2AP V2 and V3 not implemented")
endif()


