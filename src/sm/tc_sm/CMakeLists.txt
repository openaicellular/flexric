cmake_minimum_required(VERSION 3.15) # setting this is required
project(tc_sm)            # this sets the project name


set(SM_ENCODING_TC "PLAIN" CACHE STRING "The E2AP encoding to use")
set_property(CACHE SM_ENCODING_TC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected SM_ENCODING: ${SM_ENCODING_TC}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../util)


if(SM_ENCODING_TC STREQUAL "PLAIN")
  add_library(tc_sm SHARED
                      # This dependency sucks!
                      ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
                      ../sm_proc_data.c 
                      tc_sm_agent.c 
                      tc_sm_ric.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                      enc/tc_enc_plain.c 
                      dec/tc_dec_plain.c 
                      ie/tc_data_ie.c 
                      )

elseif(SM_ENCODING_TC STREQUAL "ASN" )

  add_subdirectory(ie/asn)

  add_library(tc_sm SHARED
                      ../sm_proc_data.c 
                      tc_sm_agent.c 
                      tc_sm_ric.c 
                      $<TARGET_OBJECTS:e2ap_ds_obj>
                      enc/tc_enc_asn.c 
                      dec/tc_dec_asn.c 
                      )

  target_include_directories(tc_sm  
                            PRIVATE
                            "ie/asn")

  target_compile_options(tc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(tc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

elseif(SM_ENCODING_TC STREQUAL "FLATBUFFERS" )
  add_library(tc_sm SHARED
                      ../sm_proc_data.c 
                      tc_sm_agent.c 
                      tc_sm_ric.c 
                      $<TARGET_OBJECTS:e2ap_ds_obj>
                      enc/tc_enc_fb.c 
                      dec/tc_dec_fb.c 
                      )
else()


endif()

target_compile_definitions(tc_sm PUBLIC ${SM_ENCODING_TC})


