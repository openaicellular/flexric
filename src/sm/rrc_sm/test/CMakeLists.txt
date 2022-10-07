
set(SM_ENCODING_RRC "PLAIN" CACHE STRING "The RRC SM encoding to use")
set_property(CACHE SM_ENCODING_RRC PROPERTY STRINGS "PLAIN" "ASN" "FLATBUFFERS")
message(STATUS "Selected RRC SM_ENCODING: ${SM_ENCODING_RRC}")


if(SM_ENCODING_RRC  STREQUAL "PLAIN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR} )
  add_executable(test_rrc_sm
                      main.c 
                      ../../../src/sm/sm_proc_data.c 
                      ../../../src/sm/rrc_sm/rrc_sm_agent.c 
                     ../../../src/sm/rrc_sm/rrc_sm_ric.c 
                     ../../../src/sm/rrc_sm/enc/rrc_enc_plain.c 
                     ../../../src/sm/rrc_sm/dec/rrc_dec_plain.c 
                     ../../../src/util/alg_ds/alg/defer.c
                     ../../../src/util/alg_ds/alg/eq_float.c
                     ../../../src/sm/rrc_sm/ie/rrc_data_ie.c
                      ../common/fill_ind_data.c
              )
elseif(SM_ENCODING_RRC STREQUAL "ASN")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}../ie/asn/ )
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  file(GLOB asn_sources "../../../src/sm/rrc_sm/ie/asn/*.c")
  add_executable(test_rrc_sm 
                      main.c 
                      ../../../src/sm/sm_proc_data.c 
                      ../../../src/sm/rrc_sm/rrc_sm_agent.c 
                      ../../../src/sm/rrc_sm/rrc_sm_ric.c 
                      ../../../src/sm/rrc_sm/enc/rrc_enc_asn.c 
                      ../../../src/sm/rrc_sm/dec/rrc_dec_asn.c 
                      ../../../src/sm/rrc_sm/ie/rrc_data_ie.c 
                      ../../../src/util/alg_ds/alg/defer.c
                      ../../../src/util/alg_ds/alg/eq_float.c
                      ../../../src/util/byte_array.c
                      ../common/fill_ind_data.c
                      ${asn_sources}
                      )
                        
  # XXX-IMPROVE: maybe we can avoid recompiling objects like asn_sources. 
  target_compile_options(test_rrc_sm PUBLIC "-DASN_DISABLE_OER_SUPPORT")
  target_compile_options(test_rrc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)
  target_include_directories(test_rrc_sm PRIVATE "../../../src/sm/rrc_sm/ie/asn")
elseif(SM_ENCODING_RRC STREQUAL "FLATBUFFERS")
  find_library(FlatCC
    NAMES flatccrt_d
    HINTS ${FlatCC_LIB_DIR} 
    #"~/workspace/flatcc/lib/"
  )
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ie )
  add_executable(test_rrc_sm
                      main.c 
                      ../../../src/sm/sm_proc_data.c 
                      ../../../src/sm/rrc_sm/rrc_sm_agent.c 
                      ../../../src/sm/rrc_sm/rrc_sm_ric.c 
                      ../../../src/sm/rrc_sm/enc/rrc_enc_fb.c 
                      ../../../src/sm/rrc_sm/dec/rrc_dec_fb.c 
                      ../../../src/sm/rrc_sm/ie/rrc_data_ie.c 
                      ../../../src/util/alg_ds/alg/defer.c
                      ../../../src/util/alg_ds/alg/eq_float.c
                      ../../../src/util/byte_array.c
                      ../common/fill_ind_data.c
              )
  target_link_libraries(test_rrc_sm
              PUBLIC 
              ${FlatCC}
              )
endif()

target_compile_definitions(test_rrc_sm PUBLIC ${SM_ENCODING_RRC})
target_link_libraries(test_rrc_sm PUBLIC -pthread)

enable_testing()
add_test(Unit_test_RRC test_rrc_sm)