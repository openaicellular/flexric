#ifndef ECGI_AMR_MIR_H
#define ECGI_AMR_MIR_H 

typedef struct{
  // Mandatory.  
  char* plmn;

  // Mandatory. Cell Identity. 28 bits
  int eci; 

} ecgi_amr_t;

void free_ecgi_amr(ecgi_amr_t* src);


#endif
