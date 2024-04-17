#include "nghbr_rel_info.h"

#include <assert.h>

void free_nghbr_rel_info(nghbr_rel_info_t* src)
{
  assert(src != NULL);

  // Serving Cell PCI
  // Mandatory
  // 9.3.39
  free_cell_pci(&src->pci);

  // Serving Cell ARFCN
  // Mandatory
  // 9.3.40
  free_cell_arfcn(&src->arfcn);

  // Neighbour Cell List
  // [1,65535]
  assert(src->sz_nghbr_cell > 0 && src->sz_nghbr_cell < 65536);
  for(size_t i = 0; i < src->sz_nghbr_cell; ++i){
    free_nr_nghbr_cell(&src->nghbr_cell[i]);
  }

  free(src->nghbr_cell);
}

bool eq_nghbr_rel_info(nghbr_rel_info_t const* m0, nghbr_rel_info_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Serving Cell PCI
  // Mandatory
  // 9.3.39
  if(eq_cell_pci(&m0->pci, &m1->pci) == false)
    return false;
  
  // Serving Cell ARFCN
  // Mandatory
  // 9.3.40
 if(eq_cell_arfcn(&m0->arfcn, &m1->arfcn) == false)
   return false;

  // Neighbour Cell List
  // [1,65535]
  assert(m0->sz_nghbr_cell > 0 && m0->sz_nghbr_cell < 65536);
  assert(m1->sz_nghbr_cell > 0 && m1->sz_nghbr_cell < 65536);

 if(m0->sz_nghbr_cell != m1->sz_nghbr_cell)
   return false;

 for(size_t i = 0; i < m0->sz_nghbr_cell; ++i){
    if(eq_nr_nghbr_cell(&m0->nghbr_cell[i], &m1->nghbr_cell[i] ) == false)
      return false;
 }

  return true;
}


nghbr_rel_info_t cp_nghbr_rel_info( nghbr_rel_info_t const* src)
{
  assert(src != NULL);

  nghbr_rel_info_t dst = {0};

  // Serving Cell PCI
  // Mandatory
  // 9.3.39
  dst.pci = cp_cell_pci(&src->pci);

  // Serving Cell ARFCN
  // Mandatory
  // 9.3.40
  dst.arfcn = cp_cell_arfcn(&src->arfcn);

  // Neighbour Cell List
  // [1,65535]
  assert(src->sz_nghbr_cell > 0 && src->sz_nghbr_cell < 65536);
  dst.sz_nghbr_cell = src->sz_nghbr_cell;
  dst.nghbr_cell = calloc(dst.sz_nghbr_cell, sizeof(nr_nghbr_cell_t));
  assert(dst.nghbr_cell != NULL && "Memory exhausted");

  for(size_t i = 0; i < dst.sz_nghbr_cell; ++i){
    dst.nghbr_cell[i] = cp_nr_nghbr_cell(&src->nghbr_cell[i]);
  }

  return dst;
}

