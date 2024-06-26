add_executable(xapp_kpm_rc 
  xapp_kpm_rc.c
  ../../../../src/util/alg_ds/alg/defer.c
  )
 
target_link_libraries(xapp_kpm_rc
                      PUBLIC
                      e42_xapp
                      $<TARGET_OBJECTS:e2_time_obj>
                      $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(xapp_kpm_rc_ntust
        xapp_kpm_rc_ntust.c
        ../../../../src/util/alg_ds/alg/defer.c
)

target_link_libraries(xapp_kpm_rc_ntust
        PUBLIC
        e42_xapp
        $<TARGET_OBJECTS:e2_time_obj>
        $<$<STREQUAL:${XAPP_DB},MYSQL_XAPP>:${LIBMYSQLCLIENT_LIBRARIES}>
        -pthread
        -lsctp
        -ldl
)

