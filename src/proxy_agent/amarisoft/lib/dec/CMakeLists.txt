add_library(dec_amr_obj OBJECT 
  dec_msg_amr_json.c
  dec_cjson.c
  #  quicktype.c
  #../../../../util/alg_ds/alg/defer.c
  $<TARGET_OBJECTS:cjson_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  )
