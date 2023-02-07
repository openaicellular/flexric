# proxy agent for Websocket RAN interface
# 
# E2 agent library rebuilding with conditional compilation to enable specific communication mechanisms that we use just in WS interface                        
# PROXY_AGENT variable will be used as conditional compilation mechanism in the E2 agent library code
#
# Dependencies
# 
# 1. libwebsockets
# 
# For installing on Ubuntu, type 'sudo apt install libwebsockets-dev'.
# Integration has been tested with version 3.2.1-3 on Ubuntu 20 that is sensibly different from the latest 
# one (version 4.3) available at current time in master branch.
# 
# Good example of integration here:
# https://libwebsockets.org/git/libwebsockets/tree/minimal-examples-lowlevel/ws-client/minimal-ws-client/minimal-ws-client.c
# or with another API named Secure Streams/SS: https://libwebsockets.org/git/libwebsockets/tree/minimal-examples/client/hello_world/hello_world-ss.c
# The version 3.2.1 used in my Ubuntu20.04 examples are in: (no SS available)
# https://libwebsockets.org/git/libwebsockets/tree/minimal-examples/ws-client/minimal-ws-client-rx/minimal-ws-client.c?h=v3.2-stable
# Data need to be sent in JSON format with a special content that amarisoft understands; see lteenb.html file for details of the content accepted.
# 
# 2. Json-c 
# 
# For installing on Ubuntu, type 'sudo apt install libjson-c-dev'.
# Integration tested with version 0.13.1 on Ubuntu 20.
# Manuals: API https://github.com/json-c/json-c/wiki, examples: https://github.com/rbtylee/tutorial-jsonc/blob/master/tutorial/index.md 
# 
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
  )
if(E2AP_ENCODING STREQUAL "ASN")
  add_library(e2_proxy_agent 
              STATIC  
              ${E2_PROXY_AGENT_SRC} $<TARGET_OBJECTS:e2ap_ie_obj> $<TARGET_OBJECTS:e2ap_asn1_obj>)
else()
  message(FATAL_ERROR "Error, while compiling the agent" )
endif()
target_compile_definitions(e2_proxy_agent PRIVATE PROXY_AGENT)
target_compile_definitions(e2_proxy_agent PRIVATE ${E2AP_ENCODING})

add_executable(proxy_agent_ws
              proxy_agent.c
              proxy_agent_conf.c
              ws_ran_if.c
              ws_msg_hdlr.c
              e2_sm_agent.c
              msg_json_ws.c
              ringbuffer.c
              notif_e2_ws.c
              ${CMAKE_SOURCE_DIR}/src/util/time_now_us.c
              ${CMAKE_SOURCE_DIR}/test/sm/common/fill_ind_data.c # this is for emulating indication data. XXX: To be removed
              )

target_compile_definitions(proxy_agent_ws PRIVATE ${E2AP_ENCODING})
target_link_libraries(proxy_agent_ws  
                      PUBLIC
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
                      $<TARGET_OBJECTS:e2ap_ie_obj>
                      $<TARGET_OBJECTS:e2ap_asn1_obj>
                      e2_proxy_agent 
                      -pthread -lsctp -ldl -lwebsockets -ljson-c)