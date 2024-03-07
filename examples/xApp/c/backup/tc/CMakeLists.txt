if(XAPP_DB STREQUAL "MYSQL_XAPP")
	include_directories(${LIBMYSQLCLIENT_INCLUDE_DIRS})
endif()

add_executable(xapp_tc_all
                xapp_tc_all.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                )

target_link_libraries(xapp_tc_all
                      PUBLIC
                      e42_xapp
                      $<TARGET_OBJECTS:e2_time_obj>
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_codel
                xapp_tc_codel.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                )

target_link_libraries(xapp_tc_codel
                      PUBLIC
                      e42_xapp
                      $<TARGET_OBJECTS:e2_time_obj>
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_partition
               xapp_tc_partition.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                )

target_link_libraries(xapp_tc_partition
                      PUBLIC
                      e42_xapp
                      $<TARGET_OBJECTS:e2_time_obj>
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                       )

add_executable(xapp_tc_ecn
                xapp_tc_ecn.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                )

target_link_libraries(xapp_tc_ecn
                      PUBLIC
                      e42_xapp
                      $<TARGET_OBJECTS:e2_time_obj>
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_tc_osi_codel
               xapp_tc_osi_codel.c
                ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
                )

target_link_libraries(xapp_tc_osi_codel
                      PUBLIC
                      e42_xapp
                      $<TARGET_OBJECTS:e2_time_obj>
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                      )
