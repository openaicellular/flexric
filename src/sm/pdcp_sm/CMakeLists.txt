cmake_minimum_required(VERSION 3.15) # setting this is required
project(pdcp_sm)            # this sets the project name

set(SM_ENCODING_PDCP_SRC
                    ../sm_proc_data.c 
                      pdcp_sm_ric.c 
                      pdcp_sm_agent.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                      ie/pdcp_data_ie.c 
                      )

if(SM_ENCODING_PDCP STREQUAL "PLAIN")
  add_library(pdcp_sm  SHARED
                       # This dependency sucks!
                      ../../lib/ap/e2ap_types/common/e2ap_ran_function.c
                     ${SM_ENCODING_PDCP_SRC}
                      enc/pdcp_enc_plain.c 
                      dec/pdcp_dec_plain.c 
                      )
elseif(SM_ENCODING_PDCP STREQUAL "ASN" )
  message(FATAL_ERROR "SM Encoding PDCP ASN not implemented")
  add_subdirectory(ie/asn)
  add_library(pdcp_sm SHARED
                      ${SM_ENCODING_PDCP_SRC}
                      enc/pdcp_enc_asn.c 
                      dec/pdcp_dec_asn.c 
                      )
  target_include_directories(pdcp_sm  
                            PRIVATE
                            "ie/asn")
  target_compile_options(pdcp_sm_lib PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(pdcp_sm_lib PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

elseif(SM_ENCODING_PDCP STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "SM Encoding PDCP FB not implemented")
  add_library(pdcp_sm SHARED
                      ${SM_ENCODING_PDCP_SRC}
                      enc/pdcp_enc_fb.c 
                      dec/pdcp_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknnown path")
endif()

target_compile_definitions(pdcp_sm PUBLIC ${SM_ENCODING_PDCP})

