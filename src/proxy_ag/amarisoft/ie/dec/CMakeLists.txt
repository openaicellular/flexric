
add_library(dec_amr_obj OBJECT 
                      dec_config_get.c
                      parse_cjson.c
                      ../cJSON/cJSON.c
                      ../../../../util/alg_ds/alg/defer.c
                      #$<TARGET_OBJECTS:cjson_obj>
                      #$<TARGET_OBJECTS:e2ap_alg_obj>
                      )

