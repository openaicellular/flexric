# proxy agent for Websocket RAN interface
# 
# E2 agent library has been rebuilt with conditional compilation to enable specific communication mechanisms that we use just in WS 
# interface. PROXY_AGENT variable will be used as conditional compilation mechanism in the E2 agent library code
#
# Dependencies
# 
# 1. libwebsockets
# 
# Integration has been tested with version 3.2.1-3 on Ubuntu 20 that is sensibly different from the latest 
# one (version 4.3) available at current time in master branch.
# Manual: https://libwebsockets.org/git/libwebsockets/tree/minimal-examples/ws-client/minimal-ws-client-rx/minimal-ws-client.c?h=v3.2-stable
# 
# 2. Json-c 
# 
# Integration tested with version 0.13.1 on Ubuntu 20.
# Manuals: API https://github.com/json-c/json-c/wiki, examples: https://github.com/rbtylee/tutorial-jsonc/blob/master/tutorial/index.md 

find_package(PkgConfig)
pkg_check_modules(LIBWEBSOCKETS REQUIRED libwebsockets>3.0.0 libwebsockets<4.0.0)
pkg_check_modules(LIBJSON-C REQUIRED json-c<1.0.0 json-c>0.13.0)

set(E2_AGENT_BLD_LIB  "STATIC")

set(E2_PROXY_AGENT_SRC 
    ${CMAKE_SOURCE_DIR}/src/agent/asio_agent.c
    ${CMAKE_SOURCE_DIR}/src/agent/e2ap_agent.c
    ${CMAKE_SOURCE_DIR}/src/agent/e2_agent.c
    ${CMAKE_SOURCE_DIR}/src/agent/e2_agent_api.c
    ${CMAKE_SOURCE_DIR}/src/agent/endpoint_agent.c
    ${CMAKE_SOURCE_DIR}/src/agent/msg_handler_agent.c
    ${CMAKE_SOURCE_DIR}/src/agent/e2_agent_api.c
    ${CMAKE_SOURCE_DIR}/src/agent/plugin_agent.c
    ${CMAKE_SOURCE_DIR}/src/agent/not_handler_agent.c
    ${CMAKE_SOURCE_DIR}/src/sm/sm_proc_data.c
    # we do below to avoid having a specific proxy_agent library for configuration parsing code
    ${CMAKE_SOURCE_DIR}/src/util/conf_file.c 
    # for testing
    ${CMAKE_SOURCE_DIR}/src/sm/mac_sm/ie/mac_data_ie.c
    ${CMAKE_SOURCE_DIR}/test/rnd/fill_rnd_data_kpm.c
    ${CMAKE_SOURCE_DIR}/test/rnd/fill_rnd_data_mac.c
    ${KPM_SRC}
  )
if(E2AP_ENCODING STREQUAL "ASN")
  add_library(e2_proxy_agent
              STATIC  
              ${E2_PROXY_AGENT_SRC} 
              $<TARGET_OBJECTS:e2ap_ie_obj> 
              $<TARGET_OBJECTS:e2ap_asn1_obj>
              $<TARGET_OBJECTS:e2ap_ap_obj>
              $<TARGET_OBJECTS:e2ap_ep_obj>
              $<TARGET_OBJECTS:e2ap_ds_obj>
              $<TARGET_OBJECTS:e2ap_alg_obj>
              $<TARGET_OBJECTS:e2_conf_obj>
              $<TARGET_OBJECTS:pending_events_obj>
              $<TARGET_OBJECTS:e2ap_types_obj>
              $<TARGET_OBJECTS:e2ap_msg_enc_obj>
              $<TARGET_OBJECTS:e2ap_msg_dec_obj>
              $<TARGET_OBJECTS:e2ap_msg_free_obj>
              $<TARGET_OBJECTS:e2_ngran_obj>    
              )
else()
  message(FATAL_ERROR "Error, while compiling the agent" )
endif()
target_compile_definitions(e2_proxy_agent PRIVATE PROXY_AGENT)
target_compile_definitions(e2_proxy_agent PRIVATE ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})


add_executable(proxy_agent_ws
              proxy_agent.c
              proxy_agent_conf.c
              ran_msg_hdlr.c
              e2_sm_agent.c
              e2_sm_mac_agent.c
              e2_sm_kpm_agent.c
              json_ser_ran.c
              ringbuffer.c
              notif_e2_ran.c
              ws_io_ran.c
              ${CMAKE_SOURCE_DIR}/src/util/time_now_us.c
              )

target_compile_definitions(proxy_agent_ws PRIVATE ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})
target_link_libraries(proxy_agent_ws
                      PUBLIC
                      e2_proxy_agent
                      -pthread -lsctp -ldl ${LIBWEBSOCKETS_LIBRARIES} ${LIBJSON-C_LIBRARIES})
target_include_directories(proxy_agent_ws PRIVATE "${CMAKE_SOURCE_DIR}/src/lib/e2ap/v1_01/ie/asn/")



