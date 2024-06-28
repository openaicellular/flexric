add_library(msg_hand_obj OBJECT
                          reg_e2_nodes.c 
                          e2_node_arr.c
                          )
target_compile_definitions(msg_hand_obj PRIVATE ${E2AP_VERSION} ${KPM_VERSION})
