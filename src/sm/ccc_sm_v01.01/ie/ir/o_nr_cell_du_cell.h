#ifndef O_NR_CELL_DU_CELL_H
#define O_NR_CELL_DU_CELL_H

#include <stdint.h>

#include "plmn_info_list.h"
#include "bwp_list_cell.h"

typedef enum {
    ENABLED_OPERATIONAL_STATE,
    DISABLED_OPERATIONAL_STATE,

    ENB_OPERATIONAL_STATE

} operational_state_e;

typedef enum {
    LOCKED_ADMINISTRATIVE_STATE,
    SHUTTING_DOWN_ADMINISTRATIVE_STATE,
    UNLOCKED_ADMINISTRATIVE_STATE,

    END_ADMINISTRATIVE_STATE

} administrative_state_e;

typedef enum {
    IDLE_CELL_STATE,
    INACTIVE_CELL_STATE,
    ACTIVE_CELL_STATE,

    END_CELL_STATE

} cell_state_e;

typedef enum {
    MS_5_SSB_PERIODICITY,
    MS_10_SSB_PERIODICITY,
    MS_20_SSB_PERIODICITY,
    MS_40_SSB_PERIODICITY,
    MS_80_SSB_PERIODICITY,
    MS_160_SSB_PERIODICITY,

    END_MS_SSB_PERIODICITY

} ssb_periodicity_e;

typedef enum {
    KHZ_15_SSB_SUB_CARRIER_SPACING,
    KHZ_30_SSB_SUB_CARRIER_SPACING,
    KHZ_120_SSB_SUB_CARRIER_SPACING,
    KHZ_240_SSB_SUB_CARRIER_SPACING,

    END_KHZ_SSB_SUB_CARRIER_SPACING

} ssb_sub_carrier_spacing_e;

typedef enum {
    MS_1_SSB_DURATION,
    MS_2_SSB_DURATION,
    MS_3_SSB_DURATION,
    MS_4_SSB_DURATION,
    MS_5_SSB_DURATION,

    END_SSB_DURATION

} ssb_duration_e;


typedef enum {
    CELL_LOCAL_ID_ATTRIBUTE_O_NR_CELL_DU,
    OPERATIONAL_STATE_ATTRIBUTE_O_NR_CELL_DU,
    ADMINISTRATIVE_STATE_ATTRIBUTE_O_NR_CELL_DU,
    CELL_STATE_ATTRIBUTE_O_NR_CELL_DU,
    PLMN_INFO_LIST_ATTRIBUTE_O_NR_CELL_DU,
    N_RPCI_ATTRIBUTE_O_NR_CELL_DU,
    N_RTAC_ATTRIBUTE_O_NR_CELL_DU,
    ARFCN_DL_ATTRIBUTE_O_NR_CELL_DU,
    ARFCN_UL_ATTRIBUTE_O_NR_CELL_DU,
    ARFCN_SUL_ATTRIBUTE_O_NR_CELL_DU,
    BS_CHANNEL_BW_DL_ATTRIBUTE_O_NR_CELL_DU,
    SSB_FREQUENCY_ATTRIBUTE_O_NR_CELL_DU,
    SSB_PERIODICITY_ATTRIBUTE_O_NR_CELL_DU,
    SSB_SUB_CARRIER_SPACING_ATTRIBUTE_O_NR_CELL_DU,
    SSB_OFFSET_ATTRIBUTE_O_NR_CELL_DU,
    SSB_DURATION_ATTRIBUTE_O_NR_CELL_DU,
    BS_CHANNEL_BW_UL_ATTRIBUTE_O_NR_CELL_DU,
    BS_CHANNEL_BW_SUL_ATTRIBUTE_O_NR_CELL_DU,
    BWP_LIST_ATTRIBUTE_O_NR_CELL_DU,

    END_ATTRIBUTE_O_NR_CELL_DU

} o_nr_cell_du_attributes_e;  //  for event trigger and action definition


//  8.8.2.2  O-NRCellDU - REPORT service

typedef union {
    uint64_t cell_local_id;  //  It identifies a NR cell of a gNB. 
    operational_state_e operat_state;  //  It indicates the operational state of the network slice or the network slice subnet. It describes whether or not the resource is physically installed and working.
    administrative_state_e admin_state;  //  It indicates the administrative state of the network slice or the network slice subnet. It describes the permission to use or prohibition against using the managed object instance, imposed through the OAM services.
    cell_state_e cell_state;  //  It indicates the usage state of the NRCellDU instance. 
    plmn_info_list_t plmn_info_list;  //  9.3.15
    uint16_t n_rpci:9;  //  [0..503]  This holds the Physical Cell Identity (PCI) of the NR cell.
    uint32_t n_rtac:24;  //  [0..16777215]  This holds the identity of the common Tracking Area Code for the PLMNs. 
    uint64_t arfcn_dl;  // NR Absolute Radio Frequency Channel Number (NR-ARFCN) for downlink
    uint64_t arfcn_ul;  // NR Absolute Radio Frequency Channel Number (NR-ARFCN) for uplink
    uint64_t arfcn_sul;  // NR Absolute Radio Frequency Channel Number (NR-ARFCN) for supplementary uplink
    uint64_t bs_channel_bw_dl;  //  BS Channel BW in MHz. for downlink
    uint32_t ssb_frequency:22;  //  The frequency provided in this attribute identifies the position of resource element RE=#0 (subcarrier #0) of resource block RB#10 of the SS block. The frequency must be positioned on the NR global frequency raster, as defined in TS 38.101-1 [42] subclause 5.4.2. and within bSChannelBwDL.
    ssb_periodicity_e ssb_periodicity;  //  Indicates cell defined SSB periodicity in number of subframes (ms). The SSB periodicity in msec is used for the rate matching purpose. 
    ssb_sub_carrier_spacing_e ssb_sub_carrier_spacing;  //  This SSB is used for for synchronization. Note that the allowed values of SSB used for representing data, by e.g. a BWP, are: 15, 30, 60 and 120 in units of kHz.
    uint8_t ssb_offset;  //  Indicates cell defining SSB time domain position. Defined as the offset of the measurement window, in number of subframes (ms), in which to receive SS/PBCH blocks, where allowed values depend on the ssbPeriodicity.
            /* allowedValues: 
                ssbPeriodicity5 ms 0..4,
                ssbPeriodicity10 ms 0..9,
                ssbPeriodicity20 ms 0..19,
                ssbPeriodicity40 ms 0..39,
                ssbPeriodicity80 ms 0..79,
                ssbPeriodicity160 ms 0..159. */
    ssb_duration_e ssb_duration;  //  Duration of the measurement window in which to receive SS/PBCH blocks. It is given in number of subframes (ms) (see 38.213 [41], subclause 4.1.
    uint64_t bs_channel_bw_ul;  //  BS Channel BW in MHz.for uplink
    uint64_t bs_channel_bw_sul;  //  BS Channel BW in MHz.for supplementary uplink
    bwp_list_t bwp_list;  //  9.3.18

} o_nr_cell_du_cell_u;  //  for indication message



#endif
