if(XAPP_DB STREQUAL "SQLITE3_XAPP")
  add_library(e42_xapp_db_obj OBJECT 
                                    db.c
                                    sqlite3/sqlite3.c
                                    sqlite3/sqlite3_wrapper.c
                                    ${CMAKE_SOURCE_DIR}/src/util/time_now_us.c)

elseif(XAPP_DB STREQUAL "MYSQL_XAPP")
  find_package(PkgConfig) 
  pkg_check_modules(LIBMYSQLCLIENT REQUIRED mysqlclient)
  foreach(FLAG ${LIBMYSQLCLIENT_CFLAGS_OTHER})
	  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
  endforeach()
  add_library(e42_xapp_db_obj OBJECT
                                    db.c
                                    ${CMAKE_SOURCE_DIR}/src/util/time_now_us.c
                                    mysql/mysql_wrapper.c)
  target_link_libraries(e42_xapp_db_obj PRIVATE ${LIBMYSQLCLIENT_LINK_LIBRARIES})

else()
  message(FATAL_ERROR "Unknown XAPP_DB selected")
endif()

target_compile_definitions(e42_xapp_db_obj PRIVATE ${XAPP_DB})

