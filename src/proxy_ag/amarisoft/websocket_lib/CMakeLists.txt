add_library(no_poll_obj OBJECT 
  nopoll-0.4.8.b429/src/nopoll.c
  nopoll-0.4.8.b429/src/nopoll_conn.c
  nopoll-0.4.8.b429/src/nopoll_conn_opts.c
  nopoll-0.4.8.b429/src/nopoll_ctx.c
  nopoll-0.4.8.b429/src/nopoll_decl.c
  nopoll-0.4.8.b429/src/nopoll_io.c
  nopoll-0.4.8.b429/src/nopoll_listener.c
  nopoll-0.4.8.b429/src/nopoll_log.c
  nopoll-0.4.8.b429/src/nopoll_loop.c
  nopoll-0.4.8.b429/src/nopoll_msg.c
  #nopoll-0.4.8.b429/src/nopoll_win32.c
  )
target_include_directories(no_poll_obj PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/nopoll-0.4.8.b429/src/")
target_compile_options(no_poll_obj PRIVATE "-Wno-unused-parameter;-Wno-sign-compare;-Wno-empty-body;")

