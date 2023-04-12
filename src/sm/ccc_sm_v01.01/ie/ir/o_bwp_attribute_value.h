#ifndef O_BWP_ATTRIBUTE_VALUE_H
#define O_BWP_ATTRIBUTE_VALUE_H


#include <stdint.h>

typedef enum {
    DL_BWP_CONTEXT,
    UL_BWP_CONTEXT,
    SUL_BWP_CONTEXT,

    END_BWP_CONTEXT

} bwp_context_e;


typedef enum {
    INITIAL_BWP,
    OTHER_BWP,

    END_IS_INITIAL_BWP

} is_initial_bwp_e;


typedef enum {
    KHZ_15_SUB_CARRIER_SPACING,
    KHZ_30_SUB_CARRIER_SPACING,
    KHZ_60_SUB_CARRIER_SPACING,
    KHZ_120_SUB_CARRIER_SPACING,

    END_KHZ_SUB_CARRIER_SPACING

} sub_carrier_spacing_e;


typedef enum {
    NORMAL_CYCLIC_PREFIX,
    EXTENDED_CYCLIC_PREFIX,

    END_CYCLIC_PREFIX

} cyclic_prefix_e;



// 8.8.2.3  O-BWP - REPORT & CONTROL services

typedef union {
    bwp_context_e bwp_context;  // It identifies whether the object is used for downlink, uplink or supplementary uplink.
    is_initial_bwp_e is_initial_bwp;  //  It identifies whether the object is used for initial or other BWP.
    sub_carrier_spacing_e sub_carrier_spacing;  //  Subcarrier spacing configuration for a BWP. See subclause 5 in TS 38.104 [12]. AllowedValues: [15, 30, 60, 120] depending on the frequency range FR1 or FR2.
    cyclic_prefix_e cyclic_prefix;  //  Cyclic prefix as defined in TS 38.211 [32], subclause 4.2.
    uint64_t start_rb;  //  Offset in common resource blocks to common resource block 0 for the applicable subcarrier spacing for a BWP. This corresponds to N_BWP_start, see subclause 4.4.5 in TS 38.211 [32]. 
    uint64_t number_of_rbs;  //  Number of physical resource blocks for a BWP. This corresponds to N_BWP_size, see subclause 4.4.5 in TS 38.211 [32].

} o_bwp_attribute_value_u;  // for indication message

#endif
