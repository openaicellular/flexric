
if(XAPP_DB STREQUAL "SQLITE3_XAPP")

  #  add_subdirectory(sqlite3)
  add_library(e42_xapp_db_obj OBJECT 
                         db.c
                         sqlite3/sqlite3.c
                         sqlite3/sqlite3_wrapper.c
                         ../../util/time_now_us.c
                         mysql/mysql_wrapper.c)

elseif(XAPP_DB STREQUAL "MYSQL_XAPP")
  find_package(PkgConfig) 
  pkg_check_modules(LIB_MYSQLCLIENT REQUIRED mysqlclient)
  add_library(e42_xapp_db_obj OBJECT
          db.c
          ../../util/time_now_us.c
          mysql/mysql_wrapper.c)
else()
  message(FATAL_ERROR "Unknown XAPP_DB selected")
endif()

target_compile_definitions(e42_xapp_db_obj PRIVATE ${XAPP_DB})

