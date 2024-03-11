if(KPM_VERSION STREQUAL "KPM_V2_01")
  set(KPM_SRC ../../src/sm/kpm_sm/kpm_sm_v02.01/ie/kpm_data_ie.c)
elseif(KPM_VERSION STREQUAL "KPM_V2_03")
  set(KPM_SRC ../../src/sm/kpm_sm/kpm_sm_v02.03/ie/kpm_data_ie.c)
elseif(KPM_VERSION STREQUAL "KPM_V3_00")
  set(KPM_SRC ../../src/sm/kpm_sm/kpm_sm_v03.00/ie/kpm_data_ie.c)
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()

if(E2AP_ENCODING STREQUAL "ASN")
  add_executable(test_ag_ric_xapp 
    test_ag_ric_xapp.c
    ../../../test/rnd/fill_rnd_data_gtp.c                  
    ../../../test/rnd/fill_rnd_data_tc.c                  
    ../../../test/rnd/fill_rnd_data_mac.c                  
    ../../../test/rnd/fill_rnd_data_rlc.c                  
    ../../../test/rnd/fill_rnd_data_pdcp.c                  
    ../../../test/rnd/fill_rnd_data_kpm.c                  
    ../../../test/rnd/fill_rnd_data_rc.c                  
    ../../../test/rnd/fill_rnd_data_slice.c                  
    ../../../test/rnd/fill_rnd_data_e2_setup_req.c
    ${KPM_SRC} 
    ../../src/sm/mac_sm/ie/mac_data_ie.c
    ../../src/sm/rlc_sm/ie/rlc_data_ie.c
    ../../src/sm/pdcp_sm/ie/pdcp_data_ie.c
    ../../src/sm/slice_sm/ie/slice_data_ie.c
    ../../src/sm/tc_sm/ie/tc_data_ie.c
    ../../src/sm/gtp_sm/ie/gtp_data_ie.c
    ../../src/util/alg_ds/alg/defer.c
    ../../
    )

  target_link_libraries(test_ag_ric_xapp
    PUBLIC
    e2_agent
    near_ric
    e42_iapp
    e42_xapp
    -pthread
    -lsctp
    -ldl
    )

#####
## Ctest
#####
enable_testing()
add_test(Unit_test_Agent_RIC_xApp test_ag_ric_xapp)
set_tests_properties(Unit_test_Agent_RIC_xApp PROPERTIES DEPENDS "Unit_test_nearRT_RIC")

else()
  message(FATAL_ERROR "Only E2AP_ENCODING allowed ")
endif()
