#ifndef GLOBAL_GNB_ID_AMR_MIR_H
#define GLOBAL_GNB_ID_AMR_MIR_H 

typedef struct{

  // Mandatory. PLMN
  char* plmn;

  // Mandatory. gnb_id_bits
  int  gnb_id_bits;

  // Mandatory. gnb_id
  int gnb_id;

  // Mandatory. gnb_name
  char* gnb_name;

} global_gnb_id_amr_t;

void free_global_gnb_id_amr(global_gnb_id_amr_t* src);


#endif
