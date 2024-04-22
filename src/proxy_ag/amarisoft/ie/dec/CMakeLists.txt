add_library(dec_amr_obj OBJECT 
                      dec_config_get.c
                      dec_msg_stats.c
                      dec_msg_ue_get.c
                      dec_msg_ho_ans_amr.c
                      parse_cjson.c
                      ../cJSON/cJSON.c
                      ../../../../util/alg_ds/alg/defer.c
                      $<TARGET_OBJECTS:types_amr_obj>
                      )

