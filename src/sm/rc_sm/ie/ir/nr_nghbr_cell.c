#include "nr_nghbr_cell.h"
#include <assert.h>
#include <string.h>

void free_nr_nghbr_cell(nr_nghbr_cell_t* src)
{
  // 9.3.41
  // NR CGI
  // 6.2.3.7
  // Mandatory
  free_nr_cgi(&src->cgi);

  // 9.3.42
  // NR PCI
  // Mandatory
  // uint16_t pci; //[0,1007]

  // 9.3.43
  // 5GS TAC
  // 6.2.3.31
  // Defined in TS 38.473
  // 9.3.1.29
  // Mandatory
  //uint8_t tac[3];

  // NR Mode Info
  // Mandatory
  // mode_nr_nghbr_cell_e mode_info;

  // 9.3.44
  // NR Frequency Info
  // 6.2.3.36.
  // Bug in the standard!!!
  // 6.2.3.36 should be 6.2.3.35
  // for O-RAN.WG3.E2SM-R003-v05.00
  // NR Frequency Info 
  // Mandatory
  free_nr_freq_info(&src->nr_freq_info);

  // Xn X2 Established
  // Mandatory
  // bool xn_x2_established;

  // HO Validated
  // Mandatory
  // bool ho_validated;

  // Version
  // Mandatory
  // [1-65535] 
  // uint16_t version;
}

bool eq_nr_nghbr_cell(nr_nghbr_cell_t const* m0, nr_nghbr_cell_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // 9.3.41
  // NR CGI
  // 6.2.3.7
  // Mandatory
  if(eq_nr_cgi(&m0->cgi, &m1->cgi) == false)
    return false;

  // 9.3.42
  // NR PCI
  // Mandatory
  // [0,1007]
  assert(m0->pci < 1008); 
  assert(m1->pci < 1008); 
  if(m0->pci != m1->pci)
    return false;

  // 9.3.43
  // 5GS TAC
  // 6.2.3.31
  // Defined in TS 38.473
  // 9.3.1.29
  // Mandatory
  if(memcmp(m0->tac, m1->tac, 3) != 0)
    return false;
  
  // NR Mode Info
  // Mandatory
  if(m0->mode_info != m1->mode_info)
    return false;

  // 9.3.44
  // NR Frequency Info
  // 6.2.3.36.
  // Bug in the standard!!!
  // 6.2.3.36 should be 6.2.3.35
  // for O-RAN.WG3.E2SM-R003-v05.00
  // NR Frequency Info 
  // Mandatory
  if(eq_nr_freq_info(&m0->nr_freq_info, &m1->nr_freq_info) == false)
    return false;

  // Xn X2 Established
  // Mandatory
  if(m0->xn_x2_established != m1->xn_x2_established) 
    return false;

  // HO Validated
  // Mandatory
  if(m0->ho_validated != m1->ho_validated)
    return false;

  // Version
  // Mandatory
  // [1-65535] 
  assert(m0->version > 0); 
  assert(m1->version > 0); 
  if(m0->version != m1->version)
    return false;

  return true;
}

nr_nghbr_cell_t cp_nr_nghbr_cell(nr_nghbr_cell_t const* src)
{
  assert(src != NULL);

  nr_nghbr_cell_t dst = {0}; 

  // 9.3.41
  // NR CGI
  // 6.2.3.7
  // Mandatory
  dst.cgi = cp_nr_cgi(&src->cgi);

  // 9.3.42
  // NR PCI
  // Mandatory
  assert(src->pci < 1008); 
  dst.pci = src->pci; //[0,1007]

  // 9.3.43
  // 5GS TAC
  // 6.2.3.31
  // Defined in TS 38.473
  // 9.3.1.29
  // Mandatory
  memcpy(dst.tac, src->tac, 3); 

  // NR Mode Info
  // Mandatory
  dst.mode_info = src->mode_info;

  // 9.3.44
  // NR Frequency Info
  // 6.2.3.36.
  // Bug in the standard!!!
  // 6.2.3.36 should be 6.2.3.35
  // for O-RAN.WG3.E2SM-R003-v05.00
  // NR Frequency Info 
  // Mandatory
  dst.nr_freq_info = cp_nr_freq_info(&src->nr_freq_info);

  // Xn X2 Established
  // Mandatory
  dst.xn_x2_established = src->xn_x2_established;

  // HO Validated
  // Mandatory
  dst.ho_validated = src->ho_validated;

  // Version
  // Mandatory
  // [1-65535] 
  assert(src->version > 0);
  dst.version = src->version;

  return dst;
}

