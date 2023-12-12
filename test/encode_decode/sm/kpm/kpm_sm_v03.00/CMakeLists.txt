project (test_enc_dec_kpm_sm)

add_executable(test_enc_dec_kpm_sm 
  ../../../../rnd/fill_rnd_data_kpm.c
  ../../../../../src/util/time_now_us.c
  ../../../../../src/util/alg_ds/alg/defer.c 
  main.c)



if(KPM_VERSION STREQUAL "KPM_V2")
  set(KPM_ASN_DIR  "../../../../../src/sm/kpm_sm/kpm_sm_v02.03/ie/asn")
elseif(KPM_VERSION STREQUAL "KPM_V3")
  set(KPM_ASN_DIR "../../../../../src/sm/kpm_sm/kpm_sm_v03.00/ie/asn")
else()
  message(FATAL_ERROR "Unknown KPM version")
endif()


target_include_directories(test_enc_dec_kpm_sm PUBLIC ${KPM_ASN_DIR})

target_link_libraries(test_enc_dec_kpm_sm 
                            PUBLIC 
                            kpm_sm_static
                            -lm
                            )

target_compile_options(test_enc_dec_kpm_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

