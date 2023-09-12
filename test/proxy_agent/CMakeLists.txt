if(E2AP_VERSION STREQUAL "E2AP_V1")
    include_directories("${CMAKE_SOURCE_DIR}/src/lib/e2ap/v1_01/ie/asn/")
    #    target_include_directories(test_perfmsg_json PRIVATE "${CMAKE_SOURCE_DIR}/src/lib/e2ap/v1_01/ie/asn/")
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
    include_directories("${CMAKE_SOURCE_DIR}/src/lib/e2ap/v2_03/ie/asn/")
    #    target_include_directories(test_perfmsg_json PRIVATE "${CMAKE_SOURCE_DIR}/src/lib/e2ap/v2_03/ie/asn/")
elseif(E2AP_VERSION STREQUAL "E2AP_V3")
    include_directories("${CMAKE_SOURCE_DIR}/src/lib/e2ap/v3_01/ie/asn/")
    #    target_include_directories(test_perfmsg_json PRIVATE "${CMAKE_SOURCE_DIR}/src/lib/e2ap/v3_01/ie/asn/")
else()
    message(FATAL_ERROR "E2AP Unknown version selected")
endif()

add_executable(test_msg_json
              tst-msg_json_ws.c
              ${CMAKE_SOURCE_DIR}/src/proxy-agent/json_ser_ran.c
              )
target_compile_definitions(test_msg_json PRIVATE ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})
add_dependencies(test_msg_json
        e2ap_ds_obj
        e2_conv_obj
        e2ap_alg_obj
        e2ap_types_obj
        3gpp_derived_ie_obj)
target_link_libraries(test_msg_json  
               PUBLIC
               $<TARGET_OBJECTS:e2ap_ep_obj>
               $<TARGET_OBJECTS:e2ap_ds_obj>
               $<TARGET_OBJECTS:e2_conv_obj>
               $<TARGET_OBJECTS:e2ap_alg_obj>
               $<TARGET_OBJECTS:e2ap_types_obj>
               $<TARGET_OBJECTS:3gpp_derived_ie_obj>
               -pthread -lsctp -ldl -lwebsockets -ljson-c)

add_executable(test_perfmsg_json
        tst-perf_json_ws.c
        ${CMAKE_SOURCE_DIR}/src/proxy-agent/json_ser_ran.c
        )
target_compile_definitions(test_perfmsg_json PRIVATE ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})
add_dependencies(test_perfmsg_json
        e2ap_ds_obj
        e2_conv_obj
        e2ap_alg_obj
        e2ap_types_obj
        3gpp_derived_ie_obj)
target_link_libraries(test_perfmsg_json
        PUBLIC
        $<TARGET_OBJECTS:e2ap_ep_obj>
        $<TARGET_OBJECTS:e2ap_ds_obj>
        $<TARGET_OBJECTS:e2_conv_obj>
        $<TARGET_OBJECTS:e2ap_alg_obj>
        $<TARGET_OBJECTS:e2ap_types_obj>
        $<TARGET_OBJECTS:3gpp_derived_ie_obj>
        -pthread -lsctp -ldl -lwebsockets -ljson-c)

enable_testing()
add_test(Unit_test_json test_msg_json)
add_test(Unit_test_json_perf test_perfmsg_json)
