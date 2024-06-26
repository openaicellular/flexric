# xapp_rc_ho
add_executable(xapp_rc_ho_kpm
        xapp_rc_ho_kpm.c
        ${CMAKE_SOURCE_DIR}/src/util/alg_ds/alg/defer.c
)

target_link_libraries(xapp_rc_ho_kpm
        PUBLIC
        e42_xapp
        $<TARGET_OBJECTS:e2_time_obj>
        $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
        -pthread
        -lsctp
        -ldl
)
