cmake_minimum_required(VERSION 3.15) # setting this is required
project(slice_sm)            # this sets the project name

set(SM_ENCODING_SLICE_SRC
                      ../sm_proc_data.c 
                      slice_sm_agent.c 
                      slice_sm_ric.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                      ie/slice_data_ie.c 
                      )

if(SM_ENCODING_SLICE STREQUAL "PLAIN")
  add_library(slice_sm SHARED
                       # This dependency sucks!
                      ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
                     ${SM_ENCODING_SLICE_SRC}
                      enc/slice_enc_plain.c 
                      dec/slice_dec_plain.c 
                      )

elseif(SM_ENCODING_SLICE STREQUAL "ASN" )

  message(FATAL_ERROR "SLICE SM ASN not implemented")
  add_subdirectory(ie/asn)

  add_library(slice_sm SHARED
                      ../sm_proc_data.c 
                      slice_sm_agent.c 
                      slice_sm_ric.c 
                      $<TARGET_OBJECTS:e2ap_ds_obj>
                      enc/slice_enc_asn.c 
                      dec/slice_dec_asn.c 
                      )

  target_include_directories(slice_sm  
                            PRIVATE
                            "ie/asn")

  target_compile_options(slice_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(slice_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

elseif(SM_ENCODING_SLICE STREQUAL "FLATBUFFERS" )

  message(FATAL_ERROR "SLICE SM FB not implemented")
  add_library(slice_sm SHARED
                      ../sm_proc_data.c 
                      slice_sm_agent.c 
                      slice_sm_ric.c 
                      $<TARGET_OBJECTS:e2ap_ds_obj>
                      enc/slice_enc_fb.c 
                      dec/slice_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type ")
endif()

target_compile_definitions(slice_sm PUBLIC ${SM_ENCODING_SLICE})

