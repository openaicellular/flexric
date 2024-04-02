#ifndef RF_PORTS_MS_AMR_H
#define RF_PORTS_MS_AMR_H 


typedef struct{
  float min;
  float max;
  float sd;
} rf_ports_ms_amr_t;

rf_ports_ms_amr_t parse_rf_ports_ms_amr(void* it);


#endif
