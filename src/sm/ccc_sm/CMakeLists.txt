cmake_minimum_required(VERSION 3.15) # setting this is required
project(ccc_sm)            # this sets the project name

set(SM_ENCODING_CCC_SRC
        ../sm_proc_data.c
        ccc_sm_agent.c
        ccc_sm_ric.c
        ../../util/byte_array.c
        ../../util/alg_ds/alg/defer.c
        ../../util/alg_ds/alg/eq_float.c
        ../../util/alg_ds/ds/seq_container/seq_arr.c
        ../../util/alg_ds/ds/seq_container/seq_ring.c
        ../../util/alg_ds/ds/assoc_container/assoc_rb_tree.c
        ../../util/alg_ds/ds/assoc_container/bimap.c
        ie/ccc_data_ie.c
  )

if(SM_ENCODING_CCC STREQUAL "PLAIN")
  message(FATAL_ERROR "CCC SM PLAIN not implemented")

  add_subdirectory(ie/ir)

  set(SM_ENCODING_CCC_SRC_PLAIN
          ${SM_ENCODING_CCC_SRC}
          enc/ccc_enc_plain.c
          dec/ccc_dec_plain.c

          $<TARGET_OBJECTS:e2ap_ran_func_obj>
          $<TARGET_OBJECTS:e2sm_ccc_ir_obj>
          $<TARGET_OBJECTS:e2_conv_obj>

          $<TARGET_OBJECTS:3gpp_derived_ie_obj>
          $<TARGET_OBJECTS:sm_common_ie_obj>
  )

  # Shared
  add_library(ccc_sm SHARED ${SM_ENCODING_CCC_SRC_PLAIN})
  target_compile_options(ccc_sm PRIVATE -fPIC -fvisibility=hidden)

  # Static
  add_library(ccc_sm_static STATIC ${SM_ENCODING_CCC_SRC_PLAIN})

elseif(SM_ENCODING_CCC STREQUAL "JSON")
  add_subdirectory(ie/json)
  add_subdirectory(ie/ir)

  set(SM_ENCODING_CCC_SRC_JSON
          ${SM_ENCODING_CCC_SRC}
          enc/ccc_enc_json.c
          dec/ccc_dec_json.c

          # Needed?
          enc/ccc_enc_plain.c
          dec/ccc_dec_plain.c

          $<TARGET_OBJECTS:e2sm_ccc_json_obj>
          $<TARGET_OBJECTS:e2sm_ccc_ir_obj>
          $<TARGET_OBJECTS:3gpp_derived_ie_obj>
          $<TARGET_OBJECTS:sm_common_ie_obj>
  )

  # Shared 
  add_library(ccc_sm SHARED ${SM_ENCODING_CCC_SRC_JSON})

  target_include_directories(ccc_sm PRIVATE "ie/json")
  target_compile_options(ccc_sm PRIVATE -fPIC -Wno-missing-field-initializers -Wno-unused-parameter -fvisibility=hidden)
  target_link_libraries(ccc_sm PRIVATE -lm -lpthread)


  # Static
  add_library(ccc_sm_static STATIC ${SM_ENCODING_CCC_SRC_JSON})
  target_include_directories(ccc_sm_static PRIVATE "ie/json")
  target_compile_options(ccc_sm_static PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter -fvisibility=hidden )
  target_link_libraries(ccc_sm_static PRIVATE -lm -lpthread)

elseif(SM_ENCODING_CCC STREQUAL "FLATBUFFERS" )
  message(FATAL_ERROR "CCC SM FB not implemented")
  add_library(ccc_sm SHARED
                     ${SM_ENCODING_CCC_SRC}
                      enc/ccc_enc_fb.c
                      dec/ccc_dec_fb.c
                      )
else()
  message(FATAL_ERROR "Unknown SM encoding type ")
endif()

target_compile_definitions(ccc_sm PUBLIC ${SM_ENCODING_CCC} ${E2AP_VERSION} ${KPM_VERSION})
target_compile_definitions(ccc_sm_static PUBLIC ${SM_ENCODING_CCC} ${E2AP_VERSION} ${KPM_VERSION})

