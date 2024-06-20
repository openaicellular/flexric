#include "nr_cells_amr.h"
#include <assert.h>
#include <stdlib.h>

void free_nr_cells_amr(nr_cells_amr_t* src)
{
  assert(src != NULL);

  // Mandatory. Downlink antenna count
  // int n_antenna_dl;

  // Mandatory. Uplink antenna count 
  // int n_antenna_ul;

  // Mandatory. Downlink layer count
  // int n_layer_dl; 

  // Mandatory. Uplink layer count
  // int n_layer_ul;

  // Mandatory. Cell gain in dB.
  // float gain;

  // Mandatory. UL state
  // bool ul_disabled;

  // Mandatory. RF port number index.
  // int rf_port;

  // Optional. Label set in configuration file.
  free(src->label);

  // Mandatory. Maximum QAM size used in downlink.
  //int dl_qam; // 64 or 256

  // Mandatory. Maximum QAM size used in uplink
  //int ul_qam; // 64 or 256

  // Mandatory. NR Cell Identity + plmn
  free_ncgi_amr(&src->ncgi);

  // Mandatory. Cell barred status.
  // bool cell_barred; 

  // Optional.
  //bool* cell_barred_redcap_1rx; 

  // Optional.
  //bool* cell_barred_redcap_2rx; 

  // Optional.
  //bool* cell_barred_ntn; 

  // Mandatory. Physical cell ID 
  // int n_id_nrcell;
  
  // Mandatory. NR frequency band indicator
  // int band;

  // Mandatory. Integer. Downlink ARFCN
  // int dl_nr_arfcn; 

  // Mandatory. Uplink ARFCN.
  // int ul_nr_arfcn; 

  // Mandatory. Number of downlink resource blocks
  // int n_rb_dl;

  // Mandatory. Number of uplink resource blocks.
  // int n_rb_ul;

  // Mandatory. SSB ARFCN.
  // int ssb_nr_arfcn;

  // Mandatory. Downlink sub carrier spacing (15 * 2 ^ mu in KHz)
  // int dl_mu;

  // Mandatory. Uplink sub carrier spacing (15 * 2 ^ mu in KHz)
  // int ul_mu;

  // Mandatory. SSB sub carrier spacing (15 * 2 ^ mu in KHz).
  // int ssb_mu;

  // Mandatory. Enumeration: FDD, TDD. Operation mode 
  free(src->mode);

  // Mandatory. Cell PRACH sequence index.
  // int prach_sequence_index;

  // Optional. connected_mobility
  if(src->conn_mob != NULL){
    free_connected_mobility_amr(src->conn_mob);
    free(src->conn_mob);
  }


  // Optional. NR secondary cells 
  if(src->scell_lst != NULL){
    free_scell_list_amr(src->scell_lst);
    free(src->scell_lst);
  }

  // Optional ncell_list
  if(src-> arr_ncell_lst != NULL){
    free_arr_ncell_list_amr(src->arr_ncell_lst);
    free(src->arr_ncell_lst);
  }

  // Optional. Dual connectivity nr_dc_scg_cell_list

  // Mandatory. 
  for(size_t i = 0; i < src->sz_plm_lst; ++i){
    free_plmn_list_amr(&src->plmn_lst[i]);
  }
  free(src->plmn_lst);
}


