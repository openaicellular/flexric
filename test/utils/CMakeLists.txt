add_executable(test_bimap_bug tst-bimap.c)
target_compile_definitions(test_bimap_bug PRIVATE ${E2AP_ENCODING} ${E2AP_VERSION} ${KPM_VERSION})
add_dependencies(test_bimap_bug pending_events_obj e2ap_alg_obj e2ap_types_obj e2ap_ds_obj)
target_link_libraries(test_bimap_bug 
              PUBLIC
              $<TARGET_OBJECTS:e2ap_ds_obj>
              $<TARGET_OBJECTS:e2ap_alg_obj>
              $<TARGET_OBJECTS:pending_events_obj>
              $<TARGET_OBJECTS:e2ap_types_obj>
              $<TARGET_OBJECTS:3gpp_derived_ie_obj>
              -pthread -lsctp -ldl)
enable_testing()
add_test(Unit_test_bimap_bug test_bimap_bug)
