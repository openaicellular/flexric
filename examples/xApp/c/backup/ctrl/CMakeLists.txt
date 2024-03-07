add_executable(xapp_mac_ctrl
  mac_ctrl.c
  ../../../../src/util/alg_ds/alg/defer.c
  )

 target_link_libraries(xapp_mac_ctrl
 						PUBLIC
 						e42_xapp
 						-pthread
 						-lsctp
 						-ldl
 						)



