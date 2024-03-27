add_executable(test_config_get 
  config_get.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  )

