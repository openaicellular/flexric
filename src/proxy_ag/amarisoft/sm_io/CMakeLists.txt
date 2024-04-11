add_library(sm_io_amr_obj OBJECT 
  io_proxy_ag.c
  kpm_sm_amr.c
  rc_sm_amr.c
            )

target_compile_definitions(sm_io_amr_obj PUBLIC ${E2AP_VERSION} ${KPM_VERSION})

