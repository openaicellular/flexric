add_library(fill_rnd_data_obj OBJECT 
  #  ../../../test/rnd/fill_rnd_data_gtp.c                  
  #  ../../../test/rnd/fill_rnd_data_tc.c                  
  #  ../../../test/rnd/fill_rnd_data_mac.c                  
  #  ../../../test/rnd/fill_rnd_data_rlc.c                  
  #  ../../../test/rnd/fill_rnd_data_pdcp.c                  
  ../../../test/rnd/fill_rnd_data_kpm.c                  
  ../../../test/rnd/fill_rnd_data_rc.c                  
  #  ../../../test/rnd/fill_rnd_data_slice.c                  
  ../../../test/rnd/fill_rnd_data_e2_setup_req.c
  #  ../../../src/util/time_now_us.c
  )

target_compile_definitions(fill_rnd_data_obj PRIVATE ${E2AP_VERSION}  ${KPM_VERSION})

add_executable(proxy_agent
  gnb_amari_emu.c 
  $<TARGET_OBJECTS:fill_rnd_data_obj>
  )

target_link_libraries(proxy_agent
                      PUBLIC
                      e2_ag_amr
                      )

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibConfig REQUIRED libconfig)

target_include_directories(proxy_agent PRIVATE ${LibConfig_INCLUDE_DIR})
target_link_libraries(proxy_agent PRIVATE ${LibConfig_LIBRARY})


target_link_libraries(proxy_agent PUBLIC "/usr/local/lib/libnopoll.so" )
