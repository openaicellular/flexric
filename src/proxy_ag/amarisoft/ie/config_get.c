#include "config_get.h"

#include <assert.h>
#include <stdlib.h>

void free_config_get_amr(config_get_amr_t * src)
{
  assert(src != NULL);

  // Mandatory in our version
  //int msg_id;
  // Mandatory
  free(src->version); 
  // Mandatory
  free(src->type); 
  // Mandatory
  free(src->name);
  // Mandatory
  free(src->license_id);
  // Mandatory
  free(src->license_user);
  // Mandatory
  free_logs_amr(&src->logs); 
  // Optional
  free(src->locked);
  // Mandatory
  //float tai;
  // 4G Optional
  // global_enb_id_amr_t* global_enb_id; 
  // 5G Optional // We make it mandatory as only 5G supported
  free_global_gnb_id_amr(&src->global_gnb_id); 

  // 4G mandatory
  // cells
  // 4G NB-IoT 
  // nb_cells 

  // Mandatory
  free_arr_nr_cells_amr(&src->arr_nr_cells);
  // Mandatory
  free_arr_rx_channels_amr(&src->arr_rx_channels);
  // Mandatory
  free_arr_tx_channels_amr(&src->arr_tx_channels);
  // Mandatory
  free_arr_rf_ports_amr(&src->arr_rf_ports);

  // Mandatory to all messages?
  //float utc;
}

