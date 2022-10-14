cmake_minimum_required(VERSION 3.15) # setting this is required
project(rrc_sm)            # this sets the project name

set(SM_ENCODING_RRC_SRC
                    ../sm_proc_data.c 
                      rrc_sm_ric.c 
                      rrc_sm_agent.c 
                     ../../util/byte_array.c 
                     ../../util/alg_ds/alg/defer.c 
                     ../../util/alg_ds/alg/eq_float.c 
                     ../../util/alg_ds/ds/seq_container/seq_arr.c 
                     ../../util/alg_ds/ds/seq_container/seq_ring.c 
                     ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                     ../../util/alg_ds/ds/assoc_container/bimap.c
                      ie/rrc_data_ie.c 
                      )

if(SM_ENCODING_RRC STREQUAL "PLAIN")
  add_library(rrc_sm  SHARED
                      ${SM_ENCODING_RRC_SRC}
                      enc/rrc_enc_plain.c 
                      dec/rrc_dec_plain.c 
                      )
elseif(SM_ENCODING_RRC STREQUAL "ASN" )

  add_subdirectory(ie/asn)
  add_library(rrc_sm SHARED
                      $<TARGET_OBJECTS:e2sm_rrc_asn1_obj>
                      ${SM_ENCODING_RRC_SRC}
                      enc/rrc_enc_asn.c 
                      dec/rrc_dec_asn.c 
                      )
  target_link_libraries(rrc_sm PUBLIC -lm)
  target_include_directories(rrc_sm PRIVATE "ie/asn")
  target_compile_options(rrc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(rrc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

elseif(SM_ENCODING_RRC STREQUAL "FLATBUFFERS" )
  add_library(rrc_sm SHARED
                      ${SM_ENCODING_RRC_SRC}
                      enc/rrc_enc_fb.c 
                      dec/rrc_dec_fb.c 
                      )
else()
  message(FATAL_ERROR "Unknnown path")
endif()

target_compile_definitions(rrc_sm PUBLIC ${SM_ENCODING_RRC})

