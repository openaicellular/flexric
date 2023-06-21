set(SM_ENCODING_SLICE "PLAIN" CACHE STRING "The SLICE SM encoding to use")
set_property(CACHE SM_ENCODING_SLICE PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SLICE SM_ENCODING: ${SM_ENCODING_SLICE}")


if(SM_ENCODING_SLICE STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  add_executable(test_slice_sm
                      main.c 
                      ../../rnd/fill_rnd_data_slice.c
                      ../../../src/util/time_now_us.c
                      ../../../src/util/alg_ds/alg/defer.c

                      #../../../src/sm/sm_proc_data.c 
                      #../../../src/sm/slice_sm/slice_sm_agent.c 
                      #../../../src/sm/slice_sm/slice_sm_ric.c 
                      #../../../src/sm/slice_sm/enc/slice_enc_plain.c 
                      #../../../src/sm/slice_sm/dec/slice_dec_plain.c 
                      #../../../src/util/alg_ds/alg/defer.c
                      #../../../src/util/alg_ds/alg/eq_float.c
                      #../../../src/sm/slice_sm/ie/slice_data_ie.c
                      #../common/fill_ind_data.c
                      #../../../src/sm/rc_sm/test/fill_rnd_data_rc.c
                      #../../../test/encode_decode/sm/kpm/fill_rnd_data_kpm.c
              )
elseif(SM_ENCODING_SLICE STREQUAL "ASN")
  message(FATAL_ERROR "Not implemented ASN encoding")

elseif(SM_ENCODING_SLICE STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented ASN encoding")

endif()

#target_compile_options(test_slice_sm PRIVATE "-fno-omit-frame-pointer;-fsanitize=address;-pg;-g") # works even if -fno-rtti is set to CXX_FLAGS
target_compile_definitions(test_slice_sm PUBLIC ${SM_ENCODING_SLICE})
target_link_libraries(test_slice_sm PUBLIC slice_sm -pthread)

enable_testing()
add_test(Unit_test_SLICE test_slice_sm)
