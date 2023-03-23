if(NOT E2AP_ENCODING STREQUAL "ASN")
  message(FATAL_ERROR "Only ASN encoding allowed for E2AP protocol")
endif()

add_executable(test_ag_ric_xapp
                test_ag_ric_xapp.c
                ${CMAKE_SOURCE_DIR}/test/sm/common/fill_ind_data.c
                ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/rlc_sm/ie/rlc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/pdcp_sm/ie/pdcp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/slice_sm/ie/slice_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/tc_sm/ie/tc_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/sm/gtp_sm/ie/gtp_data_ie.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                )

# enable below directive if you want to debug ASN code: 
# target_compile_options(test_ag_ric_xapp PUBLIC -DASN_EMIT_DEBUG=1)
target_link_libraries(test_ag_ric_xapp
                      PUBLIC
                      e2_agent
                      near_ric
                      e42_iapp
                      e42_xapp
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                      )

enable_testing()
add_test(test-All-in-one test_ag_ric_xapp) 
