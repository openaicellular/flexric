#ifndef NR_CELLS_AMR_MIR_H
#define NR_CELLS_AMR_MIR_H 

#include "arr_ncell_list_amr.h"
#include "connected_mobility_amr.h"
#include "scell_list_amr.h"
#include "ncell_list_amr.h"
#include "plmn_list_amr.h"

#include "ncgi_amr.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct{

  // Mandatory. Downlink antenna count
  int n_antenna_dl;

  // Mandatory. Uplink antenna count 
  int n_antenna_ul;

  // Mandatory. Downlink layer count
  int n_layer_dl; 

  // Mandatory. Uplink layer count
  int n_layer_ul;

  // Mandatory. Cell gain in dB.
  float gain;

  // Mandatory. UL state
  bool ul_disabled;

  // Mandatory. RF port number index.
  int rf_port;

  // Optional. Label set in configuration file.
  char* label;

  // Mandatory. Maximum QAM size used in downlink.
  int dl_qam; // 64 or 256

  // Mandatory. Maximum QAM size used in uplink
  int ul_qam; // 64 or 256

  // Mandatory. NR Cell Identity + plmn
  ncgi_amr_t ncgi;

  // Mandatory. Cell barred status.
  bool cell_barred; 

  // Optional.
  //bool* cell_barred_redcap_1rx; 

  // Optional.
  //bool* cell_barred_redcap_2rx; 

  // Optional.
  //bool* cell_barred_ntn; 

  // Mandatory. Physical cell ID 
  int n_id_nrcell;
  
  // Mandatory. NR frequency band indicator
  int band;

  // Mandatory. Integer. Downlink ARFCN
  int dl_nr_arfcn; 

  // Mandatory. Uplink ARFCN.
  int ul_nr_arfcn; 

  // Mandatory. Number of downlink resource blocks
  int n_rb_dl;

  // Mandatory. Number of uplink resource blocks.
  int n_rb_ul;

  // Mandatory. SSB ARFCN.
  int ssb_nr_arfcn;

  // Mandatory. Downlink sub carrier spacing (15 * 2 ^ mu in KHz)
  int dl_mu;

  // Mandatory. Uplink sub carrier spacing (15 * 2 ^ mu in KHz)
  int ul_mu;

  // Mandatory. SSB sub carrier spacing (15 * 2 ^ mu in KHz).
  int ssb_mu;

  // Mandatory. Enumeration: FDD, TDD. Operation mode 
  char* mode;

  // Mandatory. Cell PRACH sequence index.
  int prach_sequence_index;

  // Optional. connected_mobility
  connected_mobility_amr_t* conn_mob;

  // Optional. NR secondary cells 
  scell_list_amr_t* scell_lst;

  // Optional ncell_list
  arr_ncell_list_amr_t* arr_ncell_lst;

  // Optional. Dual connectivity nr_dc_scg_cell_list

  // Mandatory. 
  plmn_list_amr_t* plmn_lst;
  size_t sz_plm_lst;

} nr_cells_amr_t;

void free_nr_cells_amr(nr_cells_amr_t* src);


#endif
