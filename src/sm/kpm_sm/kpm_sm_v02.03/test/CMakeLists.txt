cmake_minimum_required (VERSION 3.0)
project (test_kpm_mir C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wno-unknown-pragmas  -g -std=gnu11 -fsanitize=address -fno-omit-frame-pointer")

include_directories("/usr/local/share/asn1c/")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}../../ie/asn/)

option(CODE_COVERAGE "Code coverage" ON)
if(CODE_COVERAGE)
  add_compile_options("-fprofile-arcs;-ftest-coverage")
  add_link_options("-lgcov;-coverage")
  message(STATUS "Code Coverage ON. Example usage: lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out && cd out && firefox index.html")

  message(STATUS "Code Coverage ON. Example usage: lcov --capture --directory . --output-file coverage.info && genhtml coverage.info --output-directory out && cd out && firefox index.html")
  message(STATUS "To merge different coverages: lcov --add-tracefile coverage1.info -a coverage2.info ...coverageN -o merged.info")

endif()

add_executable(test_kpm_mir  main.c)

target_link_libraries(test_kpm_mir PUBLIC 
                            kpm_sm
                            -lm
                            )

target_compile_options(test_kpm_mir PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)

target_compile_options(test_kpm_mir PUBLIC "-DASN_DISABLE_OER_SUPPORT -DPDU")
