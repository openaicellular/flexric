add_subdirectory(ws_lib)

add_library(io_ran_amr_obj OBJECT 
  ep_amr.c
  kpm_msgs_amr.c
  msg_amr.c
  rc_msgs_amr.c
  send_msg_amr.c
  ccc_msgs_amr.c
  )

target_include_directories(io_ran_amr_obj PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/ws_lib/nopoll/src/")
