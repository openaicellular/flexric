add_executable(config_get 
  config_get.c
  ../../src/proxy_ag/amarisoft/ie/config_get.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  )

add_executable(msg_stats 
  msg_stats.c
  ../../src/proxy_ag/amarisoft/ie/msg_stats.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  )

add_executable(msg_ue_get 
  msg_ue_get.c
  ../../src/proxy_ag/amarisoft/ie/msg_ue_get.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  )

