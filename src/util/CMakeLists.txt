# From a very bad design of conversion taken from OAI, it only gets worse with time...
if(E2AP_VERSION STREQUAL "E2AP_V1")
  include_directories("../lib/e2ap/v1_01/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V2")
  include_directories("../lib/e2ap/v2_03/ie/asn")
elseif(E2AP_VERSION STREQUAL "E2AP_V3")
  include_directories("../lib/e2ap/v3_01/ie/asn")
else()
  message(FATAL_ERROR "E2AP V2 and V3 not implemented")
endif()

if (UNIT_TEST)
    add_subdirectory(alg_ds/ds/tsn_queue)
    enable_testing()
endif()


add_library(e2ap_ds_obj OBJECT
                        byte_array.c 
                        alg_ds/ds/seq_container/seq_arr.c
                        alg_ds/ds/seq_container/seq_ring.c
                        alg_ds/ds/assoc_container/assoc_rb_tree.c
                        alg_ds/ds/assoc_container/bimap.c
                        alg_ds/ds/assoc_container/assoc_reg.c
                        alg_ds/ds/tsn_queue/tsn_queue.c
                        alg_ds/ds/tsq/tsq.c
                        alg_ds/ds/task_man/task_manager.c
                        )

add_library(e2ap_alg_obj OBJECT 
                        alg_ds/alg/accumulate.c
                        alg_ds/alg/defer.c
                        alg_ds/alg/lower_bound.c
                        alg_ds/alg/eq_float.c
                        alg_ds/alg/find.c
                        alg_ds/alg/for_each.c
                        alg_ds/alg/iter_swap.c
                        alg_ds/alg/rotate.c
                        alg_ds/alg/string/search_naive.c
                        alg_ds/alg/string/booyer_moore.c
                        $<TARGET_OBJECTS:e2ap_ds_obj>
                        )

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibConfig REQUIRED libconfig)
add_library(e2_conf_obj OBJECT
                        conf_file.c
                        )
target_include_directories(e2_conf_obj PRIVATE ${LibConfig_INCLUDE_DIR})
target_link_libraries(e2_conf_obj PRIVATE ${LibConfig_LIBRARY})
target_compile_definitions(e2_conf_obj PRIVATE ${XAPP_DB})
if(PROXY_AGENT)
    target_compile_definitions(e2_conf_obj PRIVATE PROXY_AGENT)
endif()

add_library(e2_conv_obj OBJECT 
                        conversions.c
                        )




add_library(e2_time_obj OBJECT 
                        time_now_us.c
                        )

add_library(e2_ngran_obj OBJECT
                         e2ap_ngran_type.c
                         )

