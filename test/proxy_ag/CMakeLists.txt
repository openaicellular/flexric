add_executable(config_get 
  ../../src/proxy_ag/amarisoft/lib/ie/msg_config_get.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  $<TARGET_OBJECTS:cjson_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  config_get.c
  )

add_executable(msg_stats 
  ../../src/proxy_ag/amarisoft/lib/ie/msg_stats.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  $<TARGET_OBJECTS:cjson_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  msg_stats.c
  )

add_executable(msg_ue_get 
  ../../src/proxy_ag/amarisoft/lib/ie/msg_ue_get.c
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  $<TARGET_OBJECTS:cjson_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  msg_ue_get.c
  )



