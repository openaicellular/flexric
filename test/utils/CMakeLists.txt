add_executable(test_bimap tst-bimap.c)
target_compile_definitions(test_bimap PRIVATE ${E2AP_ENCODING})
add_dependencies(test_bimap pending_events_obj e2ap_alg_obj e2ap_types_obj e2ap_ds_obj)
target_link_libraries(test_bimap 
              PUBLIC
              $<TARGET_OBJECTS:e2ap_ds_obj>
              $<TARGET_OBJECTS:e2ap_alg_obj>
              $<TARGET_OBJECTS:pending_events_obj>
              $<TARGET_OBJECTS:e2ap_types_obj>
              -pthread -lsctp -ldl -lwebsockets -ljson-c)
enable_testing()
add_test(Unit_test_bimap test_bimap)
