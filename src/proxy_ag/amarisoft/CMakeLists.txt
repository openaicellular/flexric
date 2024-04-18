add_subdirectory(ie)
add_subdirectory(sm_io)

add_library(e2_ag_amr STATIC  

  asio_agent_amr.c
  e2_agent_amr_api.c
  e2_agent_amr.c
  ep_amr.c
  kpm_pend_ds.c
  kpm_pend_msg.c
  kpm_msgs_amr.c
  msg_amr.c
  msg_dec_amr_ag.c
  msg_handle_amr_ag.c
  pending_event_proxy.c
  rc_pend_ds.c
  rc_pend_msg.c
  rc_msgs_amr.c
  send_msg_amr.c
  sm_io/io_proxy_ag.c
  sm_io/kpm_sm_amr.c
  sm_io/rc_sm_amr.c

  $<TARGET_OBJECTS:kpm_ric_info_common_obj>
  $<TARGET_OBJECTS:sm_common_ie_obj>
  $<TARGET_OBJECTS:e2ap_ds_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:cjson_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  $<TARGET_OBJECTS:ie_amr_obj>
  $<TARGET_OBJECTS:e2_conf_obj>
  $<TARGET_OBJECTS:sm_io_amr_obj>

  ../../util/time_now_us.c
  )

target_include_directories(e2_ag_amr PUBLIC "/usr/local/include/nopoll/")
target_compile_definitions(e2_ag_amr PUBLIC ${E2AP_VERSION} ${KPM_VERSION})


target_link_libraries(e2_ag_amr
                PUBLIC
                e2_agent 
                -pthread
                -lsctp
                -ldl
)


