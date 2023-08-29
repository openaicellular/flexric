if(KPM_VERSION STREQUAL "KPM_V2")
  add_subdirectory(kpm_sm_v02.03)
elseif(KPM_VERSION STREQUAL "KPM_V3")
  add_subdirectory(kpm_sm_v03.00)
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()





