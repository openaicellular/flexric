add_executable(test_msg_json 
              tst-msg_json_ws.c
              ${CMAKE_SOURCE_DIR}/src/proxy-agent/json_ser_ran.c
              )
target_compile_definitions(test_msg_json PRIVATE ${E2AP_ENCODING})
add_dependencies(test_msg_json e2ap_ds_obj e2ap_alg_obj)
target_link_libraries(test_msg_json  
               PUBLIC
              $<TARGET_OBJECTS:e2ap_ds_obj>
              $<TARGET_OBJECTS:e2ap_types_obj>
              $<TARGET_OBJECTS:e2ap_alg_obj>
               -pthread -lsctp -ldl -lwebsockets -ljson-c)
target_include_directories(test_msg_json PRIVATE "${CMAKE_SOURCE_DIR}/src/lib/ap/ie/asn/")

add_executable(test_perfmsg_json 
              tst-perf_json_ws.c
              ${CMAKE_SOURCE_DIR}/src/proxy-agent/json_ser_ran.c
              )
target_compile_definitions(test_perfmsg_json PRIVATE ${E2AP_ENCODING})
add_dependencies(test_perfmsg_json e2ap_ds_obj e2ap_alg_obj)
target_link_libraries(test_perfmsg_json  
               PUBLIC
              $<TARGET_OBJECTS:e2ap_ds_obj>
              $<TARGET_OBJECTS:e2ap_types_obj>
              $<TARGET_OBJECTS:e2ap_alg_obj>
               -pthread -lsctp -ldl -lwebsockets -ljson-c)
target_include_directories(test_perfmsg_json PRIVATE "${CMAKE_SOURCE_DIR}/src/lib/ap/ie/asn/")

enable_testing()
add_test(Unit_test_json test_msg_json)
add_test(Unit_test_json_perf test_perfmsg_json)
