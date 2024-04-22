#ifndef NCGI_AMR_MIR_H
#define NCGI_AMR_MIR_H

#include <stdbool.h>

typedef struct {

  // NR Cell Identity PLMN.
  char* plmn;
  
  // NR Cell Identity (36 bits)
  int nci;

} ncgi_amr_t;

typedef struct{
  ncgi_amr_t ncgi_amr;
  bool value;
} exp_ncgi_amr_t;


void free_ncgi_amr( ncgi_amr_t* src);

ncgi_amr_t parse_ncgi_amr(void* it);

exp_ncgi_amr_t parse_opt_ncgi_amr(void* it);

#endif
