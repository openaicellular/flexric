cmake_minimum_required (VERSION 3.0)
project (test_ccc C)





add_executable(test_ccc main.c
                        fill_rnd_data_ccc.c)

target_link_libraries(test_ccc PUBLIC
                               ccc_sm
                                -lm)


target_compile_options(test_ccc PRIVATE -Wno-missing-field-initializers -Wno-unused-parameter)
