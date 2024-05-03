set(SM_ENCODING_CCC "JSON" CACHE STRING "The CCC SM encoding to use")
set_property(CACHE SM_ENCODING_CCC PROPERTY STRINGS "PLAIN" "JSON" "FLATBUFFERS")
message(STATUS "Selected CCC SM_ENCODING: ${SM_ENCODING_CCC}")


if(SM_ENCODING_CCC STREQUAL "PLAIN")
  message(FATAL_ERROR "Not implemented PLAIN encoding")

elseif(SM_ENCODING_CCC STREQUAL "JSON")
  add_executable(test_ccc_sm
                      main.c 
                      ../../rnd/fill_rnd_data_ccc.c
                      ../../../src/util/time_now_us.c
                      ../../../src/util/alg_ds/alg/defer.c

                      ../../../src/sm/ccc_sm/enc/ccc_enc_plain.c

                      )

elseif(SM_ENCODING_CCC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")

endif()

target_compile_definitions(test_ccc_sm PUBLIC ${SM_ENCODING_CCC})
target_link_libraries(test_ccc_sm PUBLIC ccc_sm_static -pthread -lm)

enable_testing()
add_test(Unit_test_ccc test_ccc_sm)
