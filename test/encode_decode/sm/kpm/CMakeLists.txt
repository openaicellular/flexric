if(KPM_VERSION STREQUAL "KPM_V2_01")
  add_subdirectory(kpm_sm_v02.01)
elseif(KPM_VERSION STREQUAL "KPM_V2_03")
  add_subdirectory(kpm_sm_v02.03)
elseif(KPM_VERSION STREQUAL "KPM_V3_00")
  add_subdirectory(kpm_sm_v03.00)
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()

enable_testing()

