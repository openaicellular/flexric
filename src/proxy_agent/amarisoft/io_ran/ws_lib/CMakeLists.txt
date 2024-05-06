add_library(no_poll_obj OBJECT 
  nopoll/src/nopoll.c
  nopoll/src/nopoll_conn.c
  nopoll/src/nopoll_conn_opts.c
  nopoll/src/nopoll_ctx.c
  nopoll/src/nopoll_decl.c
  nopoll/src/nopoll_io.c
  nopoll/src/nopoll_listener.c
  nopoll/src/nopoll_log.c
  nopoll/src/nopoll_loop.c
  nopoll/src/nopoll_msg.c
  #nopoll-0.4.8.b429/src/nopoll_win32.c
  )

target_include_directories(no_poll_obj PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/nopoll/src/")
target_compile_options(no_poll_obj PRIVATE "-Wno-unused-parameter;-Wno-sign-compare;-Wno-empty-body;")

