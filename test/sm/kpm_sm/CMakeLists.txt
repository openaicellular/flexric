set(SM_ENCODING_KPM "ASN" CACHE STRING "The KPM SM encoding to use")
set_property(CACHE SM_ENCODING_KPM PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected KPM SM_ENCODING: ${SM_ENCODING_KPM}")

if(SM_ENCODING_KPM  STREQUAL "PLAIN")
  message(FATAL_ERROR "Not implemented PLAIN encoding")

elseif(SM_ENCODING_KPM STREQUAL "ASN")
  add_executable(test_kpm_sm
                      main.c 
#                      ${CMAKE_SOURCE_DIR}/src/sm/sm_proc_data.c
#                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/kpm_sm_agent.c
#                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/kpm_sm_ric.c
#                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/enc/kpm_enc_asn.c
#                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/dec/kpm_dec_asn.c
#                      ${CMAKE_SOURCE_DIR}/src/sm/kpm_sm_v2.02/ie/kpm_data_ie.c
#                      ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
#                      ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/eq_float.c
#                      ${CMAKE_SOURCE_DIR}/src/util/byte_array.c
#                      ${CMAKE_SOURCE_DIR}/test/sm/common/fill_ind_data.c
#                      ${asn_sources}
                      ../../rnd/fill_rnd_data_kpm.c
                      ../../../src/util/time_now_us.c
                      ../../../src/util/alg_ds/alg/defer.c
                      )

elseif(SM_ENCODING_KPM STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")
endif()

target_compile_definitions(test_kpm_sm PUBLIC ${SM_ENCODING_KPM})
target_link_libraries(test_kpm_sm PUBLIC kpm_sm_static -pthread -lm)

enable_testing()
add_test(Unit_test_KPM test_kpm_sm)
