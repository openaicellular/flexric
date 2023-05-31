project (test_enc_dec_kpm_sm)

add_executable(test_enc_dec_kpm_sm 
  ../../../rnd/fill_rnd_data_kpm.c
  ../../../../src/util/time_now_us.c
  main.c)

target_link_libraries(test_enc_dec_kpm_sm PUBLIC 
                            kpm_sm
                            -lm
                            )

target_compile_options(test_enc_dec_kpm_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

target_compile_options(test_enc_dec_kpm_sm PRIVATE "-DASN_DISABLE_OER_SUPPORT -DPDU")

