cmake_minimum_required(VERSION 3.15)

project (TEST_KPM_SM)
set(CMAKE_BUILD_TYPE Debug)
set (CMAKE_C_FLAGS "-Wall -Wextra --std=gnu11 -g -fno-omit-frame-pointer") 
set(SM_ENCODING_KPM "PLAIN" CACHE STRING "The SM encoding to use")
set_property(CACHE SM_ENCODING_KPM PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SM_ENCODING_KPM: ${SM_ENCODING_KPM} for tests")

if(SM_ENCODING_KPM STREQUAL "ASN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  file(GLOB asn_sources "${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/asn/*.c")
  add_executable(test_kpm_sm 
                      main.c 
                      ${CMAKE_SOURCE_DIR}/src/sm/sm_proc_data.c 
                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/kpm_sm_agent.c 
                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/kpm_sm_ric.c 
                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/enc/kpm_enc_asn.c 
                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/dec/kpm_dec_asn.c 
                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c 
                      ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                      ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/eq_float.c
                      ${CMAKE_SOURCE_DIR}/src/util/byte_array.c
                      ${CMAKE_SOURCE_DIR}/test/sm/common/fill_ind_data.c
                      ${asn_sources}
                      )
  add_dependencies(test_kpm_sm pending_events_obj e2ap_msg_enc_obj e2ap_msg_dec_obj)
  # XXX-IMPROVE: maybe we can avoid recompiling objects like asn_sources. 
  target_compile_options(test_kpm_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(test_kpm_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)
  target_include_directories(test_kpm_sm PRIVATE "${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/asn")
else()
  message(FATAL_ERROR "unsupported encoding type")
endif()

target_compile_definitions(test_kpm_sm PUBLIC ${SM_ENCODING_KPM})
target_link_libraries(test_kpm_sm PUBLIC -pthread)
target_link_libraries(test_kpm_sm PUBLIC m)

enable_testing()
add_test(Unit_test_KPM test_kpm_sm)
