set(SM_ENCODING_RC "ASN" CACHE STRING "The RC SM encoding to use")
set_property(CACHE SM_ENCODING_RC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected RC SM_ENCODING: ${SM_ENCODING_RC}")


if(SM_ENCODING_RC  STREQUAL "PLAIN")
  message(FATAL_ERROR "Not implemented PLAIN encoding")

elseif(SM_ENCODING_RC STREQUAL "ASN")
  #  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/sm/rc_sm/ie/asn/ )
  # include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src/lib/ap/ie/asn/ )
  
  #file(GLOB asn_sources "../../../src/sm/rc_sm/ie/asn/*.c")
  #file(GLOB ir_sources "../../../src/sm/rc_sm/ie/ir/*.c")
  #file(GLOB 3gpp_derived_ie "../../../src/lib/sm/3gpp_derived_ie/*.c")
  #file(GLOB 3gpp_derived_ie_dec_asn "../../../src/lib/sm/3gpp_derived_ie_dec_asn/*.c")
  #file(GLOB 3gpp_derived_ie_enc_asn "../../../src/lib/sm/3gpp_derived_ie_enc_asn/*.c")
  #file(GLOB sm_common_ie "../../../src/lib/sm/sm_common_ie/*.c")

  #file(GLOB enc_sm_common_ie "../../../src/lib/sm/enc_asn_sm_common/*.c")
  #file(GLOB dec_sm_common_ie "../../../src/lib/sm/dec_asn_sm_common/*.c")

  #include_directories("/usr/local/share/asn1c/")
  add_executable(test_rc_sm
                      main.c 
                      ../../rnd/fill_rnd_data_rc.c
                      ../../../src/util/time_now_us.c
                      # ../common/fill_ind_data.c
                      #../../../src/sm/sm_proc_data.c 
                      #../../../src/sm/rc_sm/rc_sm_agent.c
                      #../../../src/sm/rc_sm/rc_sm_ric.c
                      #../../../src/sm/rc_sm/ie/rc_data_ie.c
                      #../../../src/sm/rc_sm/ie/rc_data_ie.c
                      #../../../src/sm/rc_sm/test/fill_rnd_data_rc.c                      
                      #../../../src/sm/rc_sm/enc/rc_enc_asn.c
                      #../../../src/sm/rc_sm/dec/rc_dec_asn.c
                      #../../../src/util/alg_ds/alg/defer.c
                      #../../../src/util/byte_array.c
                      #../../../src/util/conversions.c
                      #${ir_sources}
                      #${3gpp_derived_ie}          
                      #${3gpp_derived_ie_dec_asn}          
                      #${3gpp_derived_ie_enc_asn}          
                      #${sm_common_ie}
                      #${asn_sources}
                      #${enc_sm_common_ie}
                      #${dec_sm_common_ie}
                      )

                    #  target_compile_options(test_rc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT" )
                    #  target_compile_definitions(test_rc_sm  PRIVATE ${SM_ENCODING_RC})

elseif(SM_ENCODING_RC STREQUAL "FLATBUFFERS")
  message(FATAL_ERROR "Not implemented FLATBUFFERS encoding")

endif()

target_compile_definitions(test_rc_sm PUBLIC ${SM_ENCODING_RC})
target_link_libraries(test_rc_sm PUBLIC rc_sm -pthread -lm)


enable_testing()
add_test(Unit_test_RC test_rc_sm)
