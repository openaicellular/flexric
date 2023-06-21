cmake_minimum_required(VERSION 3.15) # setting this is required
project(rlc_sm)            # this sets the project name

set(SM_ENCODING_RLC_SRC
                      ../sm_proc_data.c 
                      rlc_sm_agent.c 
                      rlc_sm_ric.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                      ie/rlc_data_ie.c 
                      )


if(SM_ENCODING_RLC STREQUAL "PLAIN")
  add_library(rlc_sm SHARED
                      # This dependency sucks!
                      ../../lib/ap/e2ap_types/common/e2ap_ran_function.c

                      ${SM_ENCODING_RLC_SRC}
                      enc/rlc_enc_plain.c 
                      dec/rlc_dec_plain.c 
                      )

elseif(SM_ENCODING_RLC STREQUAL "ASN" )
  message(FATAL_ERROR "RLC SM ASN not implemented")
  add_subdirectory(ie/asn)

  add_library(rlc_sm SHARED
                     ${SM_ENCODING_RLC_SRC}
                      enc/rlc_enc_asn.c 
                      dec/rlc_dec_asn.c 
                      )
  target_include_directories(rlc_sm PRIVATE "ie/asn")
  target_compile_options(rlc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(rlc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

elseif(SM_ENCODING_RLC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "RLC SM FB not implemented")
  add_library(rlc_sm SHARED
                     ${SM_ENCODING_RLC_SRC}
                      enc/rlc_enc_fb.c 
                      dec/rlc_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type ")
endif()

target_compile_definitions(rlc_sm PUBLIC ${SM_ENCODING_RLC})

