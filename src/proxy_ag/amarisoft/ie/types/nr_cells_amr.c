#include "nr_cells_amr.h"
#include <assert.h>
#include <stdlib.h>
#include "../cJSON/cJSON.h"
#include "../dec/parse_cjson.h"

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

nr_cells_amr_t parse_nr_cells_amr(void* it_void)
{
  assert(it_void != NULL);

  nr_cells_amr_t dst = {0}; 
  cJSON const* it = (cJSON const*)it_void;

  // Mandatory. Downlink antenna count
  dst.n_antenna_dl = parse_int(it, "n_antenna_dl");

  // Mandatory. Uplink antenna count 
  dst.n_antenna_ul = parse_int(it, "n_antenna_ul"); 

  // Mandatory. Downlink layer count
  dst.n_layer_dl = parse_int(it, "n_layer_dl");

  // Mandatory. Uplink layer count
  dst.n_layer_ul = parse_int(it, "n_layer_ul");

  // Mandatory. Cell gain in dB.
  dst.gain = parse_float(it, "gain");

  // Mandatory. UL state
  dst.ul_disabled = parse_bool(it, "ul_disabled");

  // Mandatory. RF port number index.
  dst.rf_port = parse_int(it, "rf_port");

  // Optional. Label set in configuration file.
  // char* label;

  // Mandatory. Maximum QAM size used in downlink.
  // 64 or 256
  dst.dl_qam = parse_int(it, "dl_qam");
  assert(dst.dl_qam == 64 || dst.dl_qam == 256); 

  // Mandatory. Maximum QAM size used in uplink
  // 64 or 256
  dst.ul_qam = parse_int(it, "ul_qam");
  assert(dst.ul_qam == 64 || dst.ul_qam == 256); 

  // Mandatory. NR Cell Identity + plmn
  dst.ncgi = parse_ncgi_amr((void*)it);

  // Mandatory. Cell barred status.
  dst.cell_barred = parse_bool(it, "cell_barred");

  // Optional.
  //bool* cell_barred_redcap_1rx; 

  // Optional.
  //bool* cell_barred_redcap_2rx; 

  // Optional.
  //bool* cell_barred_ntn; 

  // Mandatory. Physical cell ID 
  dst.n_id_nrcell = parse_int(it, "n_id_nrcell");

  // Mandatory. NR frequency band indicator
  dst.band = parse_int(it, "band");

  // Mandatory. Integer. Downlink ARFCN
  dst.dl_nr_arfcn = parse_int(it, "dl_nr_arfcn");

  // Mandatory. Uplink ARFCN.
  dst.ul_nr_arfcn= parse_int(it, "ul_nr_arfcn"); 

  // Mandatory. Number of downlink resource blocks
  dst.n_rb_dl = parse_int(it, "n_rb_dl");

  // Mandatory. Number of uplink resource blocks.
  dst.n_rb_ul = parse_int(it, "n_rb_ul");

  // Mandatory. SSB ARFCN.
  dst.ssb_nr_arfcn = parse_int(it, "ssb_nr_arfcn");

  // Mandatory. Downlink sub carrier spacing (15 * 2 ^ mu in KHz)
  dst.dl_mu = parse_int(it, "dl_mu");

  // Mandatory. Uplink sub carrier spacing (15 * 2 ^ mu in KHz)
  dst.ul_mu = parse_int(it, "ul_mu");

  // Mandatory. SSB sub carrier spacing (15 * 2 ^ mu in KHz).
  dst.ssb_mu = parse_int(it, "ssb_mu");

  // Mandatory. Enumeration: FDD, TDD. Operation mode 
  dst.mode = parse_string(it, "mode");

  // Mandatory. Cell PRACH sequence index.
  dst.prach_sequence_index = parse_int(it, "prach_sequence_index");

  // Optional. connected_mobility
  ans_cjson_t const cm = find_object(it, "connected_mobility" ); 
  if(cm.it != NULL){
    dst.conn_mob = calloc(1, sizeof(connected_mobility_amr_t));
    assert(dst.conn_mob != NULL && "Memory exhausted");
    *dst.conn_mob = parse_connected_mobility_amr((void*)cm.it);
  } 

  // Optional. NR secondary cells 
  ans_cjson_t const scl = find_object(it, "scell_list" ); 
  if(scl.it != NULL){
    dst.scell_lst = calloc(1, sizeof(scell_list_amr_t));
    assert(dst.scell_lst != NULL && "Memory exhausted");
    *dst.scell_lst = parse_scell_list_amr((void*)scl.it); 
  }

  // Optional ncell_list
  ans_cjson_t const ncl = find_object(it, "ncell_list"); 
  if(ncl.it != NULL){
    dst.arr_ncell_lst = calloc(1, sizeof(arr_ncell_list_amr_t)); 
    assert(dst.arr_ncell_lst != NULL && "Memory exhausted");
    *dst.arr_ncell_lst = parse_arr_ncell_list_amr((void*)ncl.it);
  }

  // Optional. Dual connectivity 
  // nr_dc_scg_cell_list

  // Mandatory. PLMN list 
  ans_cjson_t plst = find_object(it, "plmn_list");
  assert(plst.it != NULL);
 
  dst.sz_plm_lst = cJSON_GetArraySize(plst.it);
  
  if(dst.sz_plm_lst > 0){
    dst.plmn_lst = calloc(1 , sizeof(plmn_list_amr_t ));
    assert(dst.plmn_lst != NULL && "Memory exhausted");
  }

  cJSON* tmp_it = plst.it->child; 
  for(size_t i = 0; i < dst.sz_plm_lst; ++i){
    assert(tmp_it != NULL);
    dst.plmn_lst[i] = parse_plmn_list_amr(tmp_it);
    tmp_it = tmp_it->next;
  } 

  return dst;
}

