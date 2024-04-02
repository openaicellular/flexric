#ifndef AMARISOFT_CONFIG_GET_MSG_MIR_H
#define AMARISOFT_CONFIG_GET_MSG_MIR_H 

#include <stdbool.h>
#include "types/logs_amr.h"
#include "types/global_gnb_id_amr.h"
#include "types/nr_cells_amr.h"
#include "types/arr_nr_cells_amr.h"
#include "types/arr_rx_channels_amr.h"
#include "types/arr_tx_channels_amr.h"
#include "types/arr_rf_ports_amr.h"

typedef struct{
  // Mandatory in our version
  int msg_id;
  // Mandatory
  char* version;
  // Mandatory
  char* type;
  // Mandatory
  char* name;
  // Mandatory
  char* license_id;
  // Mandatory
  char* license_user;
  // Mandatory
  logs_amr_t logs;
  // Optional
  bool* locked;
  // Mandatory
  float tai;
  // 4G Optional
  // global_enb_id_amr_t* global_enb_id; 
  // 5G Optional // We make it mandatory as only 5G supported
  global_gnb_id_amr_t global_gnb_id; 

  // 4G mandatory
  // cells
  // 4G NB-IoT 
  // nb_cells 

  // Mandatory
  arr_nr_cells_amr_t arr_nr_cells;
  // Mandatory
  arr_rx_channels_amr_t arr_rx_channels;
  // Mandatory
  arr_tx_channels_amr_t arr_tx_channels;
  // Mandatory
  arr_rf_ports_amr_t arr_rf_ports;
  
  // Mandatory to all messages?
  float utc;
} config_get_amr_t;

void free_config_get_amr(config_get_amr_t * src);

#endif
