set(SM_ENCODING_TC "PLAIN" CACHE STRING "The TC SM encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected TC SM_ENCODING: ${SM_ENCODING_TC}")


if(SM_ENCODING_TC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_tc_sm
                      main.c 
                      ../../../src/sm/sm_proc_data.c 
                      ../../../src/sm/tc_sm/tc_sm_agent.c 
                      ../../../src/sm/tc_sm/tc_sm_ric.c 
                      ../../../src/sm/tc_sm/enc/tc_enc_plain.c 
                      ../../../src/sm/tc_sm/dec/tc_dec_plain.c 
                      ../../../src/util/alg_ds/alg/defer.c
                      ../../../src/util/alg_ds/alg/eq_float.c
                      ../../../src/sm/tc_sm/ie/tc_data_ie.c
                      ../common/fill_ind_data.c
              )
elseif(SM_ENCODING_TC STREQUAL "ASN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
  file(GLOB asn_sources "../ie/asn/*.c")
  #include_directories("/usr/local/share/asn1c/")
  target_compile_options(test_tc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  add_executable(test_tc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_server.c 
                      ../enc/tc_enc_asn.c 
                      ../dec/tc_dec_asn.c 
                      ../../util/alg_ds/alg/defer.c
                      ../../util/alg_ds/alg/eq_float.c
                      ../ie/tc_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ${asn_sources} 
                      ../../../../test/common/fill_ind_data.c
              )
elseif(SM_ENCODING_TC STREQUAL "FLATBUFFERS")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
  add_executable(test_tc_sm
                      main.c 
                      ../../sm_proc_data.c 
                      ../tc_sm_agent.c 
                      ../tc_sm_server.c 
                      ../enc/tc_enc_fb.c 
                      ../dec/tc_dec_fb.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ../ie/tc_data_ie.c
                      ../../if_sm/read/sm_rd_if.c
                      ../../../../test/common/fill_ind_data.c
              )
endif()

target_compile_definitions(test_tc_sm PUBLIC ${SM_ENCODING_TC})
target_link_libraries(test_tc_sm PUBLIC -pthread)


if(FALSE)

cmake_minimum_required(VERSION 3.15)

project (TC_SM)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_C_FLAGS "-Wall -Wextra --std=gnu11 -g -fsanitize=address -fno-omit-frame-pointer") 


set(SM_ENCODING_TC "PLAIN" CACHE STRING "The E2AP encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SM_ENCODING_TC: ${SM_ENCODING_TC}")


include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/fb/ )
include_directories("/usr/local/share/asn1c/")


file(GLOB asn_sources "../ie/asn/*.c")

add_executable(test_tc_sm main.c 
                      ../../../src/sm/sm_proc_data.c 
                      ../../../src/sm/tc_sm/tc_sm_agent.c 
                      ../../../src/sm/tc_sm/tc_sm_ric.c 
                      ../../../src/sm/tc_sm/ ../enc/tc_enc_plain.c 
                       ../../../src/sm/tc_sm/../enc/tc_enc_fb.c 
                       ../../../src/sm/tc_sm/../enc/tc_enc_asn.c 
                      ../../../src/sm/tc_sm/ ../dec/tc_dec_plain.c 
                      ../dec/tc_dec_fb.c 
                      ../dec/tc_dec_asn.c 
                      ../ie/tc_data_ie.c 
                      ../../../util/alg_ds/alg/defer.c
                      ../../agent_if/read/sm_ag_if_rd.c
                      # Horrible design. Fix It.
                      ../../mac_sm/ie/mac_data_ie.c
                      ../../rlc_sm/ie/rlc_data_ie.c
                      ../../pdcp_sm/ie/pdcp_data_ie.c
                      ../../rrc_sm/ie/mac_data_ie.c
                      ../../../util/alg_ds/alg/eq_float.c
                      ${asn_sources} 
                      )

target_compile_definitions(test_tc_sm PUBLIC ${SM_ENCODING_TC})
target_compile_options(test_tc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
target_link_libraries(test_tc_sm PUBLIC -pthread)

endif()


enable_testing()
add_test(Unit_test_TC test_tc_sm)
