project (test_enc_dec_rc_sm)

add_executable(test_enc_dec_rc_sm 
  ../../../rnd/fill_rnd_data_rc.c
  main.c)

target_link_libraries(test_enc_dec_rc_sm PUBLIC 
  rc_sm
  -lm
  )

target_compile_options(test_enc_dec_rc_sm PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

target_compile_options(test_enc_dec_rc_sm  PRIVATE "-DASN_DISABLE_OER_SUPPORT -DPDU")

