add_executable(xapp_kpm_rc 
  xapp_kpm_rc.c
  ../../../../src/util/alg_ds/alg/defer.c
  )
 
target_link_libraries(xapp_kpm_rc
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(cu_du_xapp_kpm_rc 
  cu_du_xapp_kpm_rc.c
  ../../../../src/util/alg_ds/alg/defer.c
  )
 
target_link_libraries(cu_du_xapp_kpm_rc
                      PUBLIC
                      e42_xapp
                      -pthread
                      -lsctp
                      -ldl
                      )

add_executable(mono_gnb_xapp_kpm_rc 
                      mono_gnb_xapp_kpm_rc.c
                      ../../../../src/util/alg_ds/alg/defer.c
                      )
                     
target_link_libraries(mono_gnb_xapp_kpm_rc
                                          PUBLIC
                                          e42_xapp
                                          -pthread
                                          -lsctp
                                          -ldl
                                          )
