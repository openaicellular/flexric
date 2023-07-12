if(E2AP_VERSION STREQUAL "E2AP_V1")
  add_subdirectory(v1_01)
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
  add_subdirectory(v2_03)
elseif(E2AP_VERSION STREQUAL "E2AP_V3")
  add_subdirectory(v3_01)
else()
  message(FATAL_ERROR "Unknown E2AP version")
endif()


