add_subdirectory(ie)

add_library(e2_ag_amr STATIC  
  sm_io/io_proxy_ag.c
  asio_agent_amr.c
  e2_agent_amr_api.c
  e2_agent_amr.c
  ep_amr.c
  msg_amr.c
  msg_dec_amr_ag.c
  msg_handle_amr_ag.c
  pending_event_proxy.c
  sm_io/io_proxy_ag.c
  $<TARGET_OBJECTS:e2ap_ds_obj>
  $<TARGET_OBJECTS:e2ap_alg_obj>
  $<TARGET_OBJECTS:dec_amr_obj>
  $<TARGET_OBJECTS:cjson_obj>
  $<TARGET_OBJECTS:types_amr_obj>
  $<TARGET_OBJECTS:ie_amr_obj>
  $<TARGET_OBJECTS:e2_conf_obj>
  )

target_include_directories(e2_ag_amr PUBLIC "/usr/local/include/nopoll/")
target_compile_definitions(e2_ag_amr PUBLIC ${E2AP_VERSION} ${KPM_VERSION})


target_link_libraries(e2_ag_amr
                PUBLIC
                -pthread
                -lsctp
                -ldl
)


