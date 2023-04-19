cmake_minimum_required(VERSION 3.15) # setting this is required
project(ccc_sm)            # this sets the project name

add_subdirectory(dec)
add_subdirectory(enc)
add_subdirectory(ie)
add_subdirectory(test)


# CCC Service Model

set(SM_ENCODING_CCC_SRC
                      ../sm_proc_data.c 
                    #   ccc_sm_ric.c 
                    #   ccc_sm_agent.c 
                      ../../util/byte_array.c 
                      ../../util/alg_ds/alg/defer.c 
                      ../../util/alg_ds/alg/eq_float.c 
                      ../../util/alg_ds/ds/seq_container/seq_arr.c 
                      ../../util/alg_ds/ds/seq_container/seq_ring.c 
                      ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
                      ../../util/alg_ds/ds/assoc_container/bimap.c
                      )

if(SM_ENCODING_CCC STREQUAL "PLAIN")
  add_library(ccc_sm SHARED
                      ${SM_ENCODING_CCC_SRC}
                    #   enc/ccc_enc_plain.c 
                    #   dec/ccc_dec_plain.c 
                      )
elseif(SM_ENCODING_CCC STREQUAL "JSON")
  
  
  add_library(ccc_sm SHARED
                      ${SM_ENCODING_CCC_SRC}
                      enc/ccc_enc_json.c
                      dec/ccc_dec_json.c
                    #   dec/ccc_dec_asn.c 
                    #   $<TARGET_OBJECTS:e2sm_kpm_asn1_obj>
                    #   $<TARGET_OBJECTS:kpm_ric_info_common_obj>
                      $<TARGET_OBJECTS:ccc_ric_info_obj>

                    #   $<TARGET_OBJECTS:kpm_dec_common_asn_obj>
                      $<TARGET_OBJECTS:ccc_dec_ie_json_obj>

                    #   $<TARGET_OBJECTS:kpm_enc_common_asn_obj>
                      $<TARGET_OBJECTS:ccc_enc_ie_json_obj>

                    #   $<TARGET_OBJECTS:3gpp_derived_ie_obj>
                    #   $<TARGET_OBJECTS:3gpp_derived_ie_dec_asn_obj>
                    #   $<TARGET_OBJECTS:3gpp_derived_ie_enc_asn_obj>
                    #   $<TARGET_OBJECTS:sm_common_ie_obj>
                    #   $<TARGET_OBJECTS:sm_common_dec_asn_obj>
                    #   $<TARGET_OBJECTS:sm_common_enc_asn_obj>
                      # $<TARGET_OBJECTS:e2sm_common_asn1_obj>   # to be included when KPM ie/asn folder doesn't contain both KPM and E2SM asn1 types

                    #   ../../../test/encode_decode/sm_test/kpm_test/fill_rnd_data_kpm.c
                    #   ie/ccc_data_ie.c

                      )


  target_link_libraries(ccc_sm PUBLIC
                              cjson
                              -lm)
  
  target_include_directories(ccc_sm PRIVATE "ie/json")
  target_compile_options(ccc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT" -fPIC)
  target_compile_options(ccc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fPIC)


elseif(SM_ENCODING_CCC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "CCC SM FB not implemented")
else()
  message(FATAL_ERROR "Unknown CCC SM encoding type")
endif()

target_compile_definitions(ccc_sm PUBLIC ${SM_ENCODING_CCC})

