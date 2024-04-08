add_executable(proxy_agent
  gnb_amari_emu.c 
  )

target_link_libraries(proxy_agent
                      PUBLIC
                      e2_ag_amr
                      kpm_sm_static 
                      )

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibConfig REQUIRED libconfig)

target_include_directories(proxy_agent PRIVATE ${LibConfig_INCLUDE_DIR})
target_link_libraries(proxy_agent PRIVATE ${LibConfig_LIBRARY})


target_link_libraries(proxy_agent PUBLIC "/usr/local/lib/libnopoll.so" )
