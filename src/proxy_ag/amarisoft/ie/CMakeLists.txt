add_subdirectory(cJSON)
add_subdirectory(dec)
add_subdirectory(types)

add_library(ie_amr_obj OBJECT 
  config_get.c
  config_set.c
  msg_stats.c
  msg_ue_get.c
  msg_ho_ans_amr.c
            )

