add_executable(test_bimap 
              tst-bimap.c
              # ../../src/proxy-agent/proxy_agent_conf.c
              # ../../src/proxy-agent/ws_ran_if.c
              # ../../src/proxy-agent/ws_msg_hdlr.c
              # ../../src/proxy-agent/e2_sm_agent.c
              # ../../src/proxy-agent/ringbuffer.c
              # ../../src/proxy-agent/notif_e2_ws.c
              )
target_compile_definitions(test_bimap PRIVATE ${E2AP_ENCODING})
add_dependencies(test_bimap pending_events_obj e2ap_alg_obj e2ap_types_obj e2ap_ds_obj)
target_link_libraries(test_bimap 
              PUBLIC
              # $<TARGET_OBJECTS:e2ap_ap_obj>
              # $<TARGET_OBJECTS:e2ap_ep_obj>
              $<TARGET_OBJECTS:e2ap_ds_obj>
              $<TARGET_OBJECTS:e2ap_alg_obj>
              # $<TARGET_OBJECTS:e2_conf_obj>
              $<TARGET_OBJECTS:pending_events_obj>
              $<TARGET_OBJECTS:e2ap_types_obj>
              # $<TARGET_OBJECTS:e2ap_msg_enc_obj>
              # $<TARGET_OBJECTS:e2ap_msg_dec_obj>
              # $<TARGET_OBJECTS:e2ap_msg_free_obj>
              # $<TARGET_OBJECTS:e2_ngran_obj>
              # $<TARGET_OBJECTS:e2ap_ie_obj>
              # $<TARGET_OBJECTS:e2ap_asn1_obj>
              # e2_proxy_agent 
              -pthread -lsctp -ldl -lwebsockets -ljson-c)
enable_testing()
add_test(Unit_test_bimap test_bimap)
