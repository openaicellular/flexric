add_library(io_sm_amr_obj OBJECT 
  init_sm_amr.c 
  kpm_sm_amr.c
  rc_sm_amr.c
  ccc_sm_amr.c
            )

target_compile_definitions(io_sm_amr_obj PUBLIC ${E2AP_VERSION} ${KPM_VERSION})

