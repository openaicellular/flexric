#ifndef NR_CGI_KPM_V3_H
#define NR_CGI_KPM_V3_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "plmn_identity.h"

//  6.2.3.7  NR CGI

typedef struct{
  plmn_t plmn_id;  // 6.2.3.1
  uint64_t nr_cell_id; // bit string of 36 bits

} nr_cgi_t;

bool eq_nr_cgi(nr_cgi_t const * m0, nr_cgi_t const * m1);

nr_cgi_t cp_nr_cgi(const nr_cgi_t * src);

#ifdef __cplusplus
}
#endif

#endif

