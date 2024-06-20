add_executable(xapp_keysight_kpm_rc 
  xapp_keysight_kpm_rc.c
  ../../../../src/util/alg_ds/alg/defer.c
  )
 
target_link_libraries(xapp_keysight_kpm_rc
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

