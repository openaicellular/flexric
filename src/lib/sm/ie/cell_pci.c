#include "cell_pci.h"
#include <assert.h>
#include <stdlib.h>

bool eq_cell_pci(cell_pci_t const* m0, cell_pci_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type)
    return false;

  if(m0->type == NR_RAT_TYPE){
    // [0,1007]  6.2.3.29 
    assert(m0->nr_pci < 1008);
    assert(m1->nr_pci < 1008);
    if(m0->nr_pci != m1->nr_pci)
      return false;
  } else if(m0->type == EUTRA_RAT_TYPE){
    // [0,503] // 6.2.3.32
    assert(m0->eutra_pci < 504);
    assert(m1->eutra_pci < 504);
    if(m0->eutra_pci != m1->eutra_pci)
      return false;
  } else {
    assert(0 != 0 && "Unknown type");
  }

  return true;
}

void free_cell_pci(cell_pci_t* src)
{
  assert(src != NULL);
  (void)src;
}

cell_pci_t cp_cell_pci(cell_pci_t const* src)
{
  assert(src != NULL);

  cell_pci_t dst = {.type = src->type}; 

  if(dst.type == NR_RAT_TYPE){
    assert(dst.nr_pci < 1008);
    dst.nr_pci = src->nr_pci;
  } else if(dst.type == EUTRA_RAT_TYPE){
    assert(dst.eutra_pci < 504);
    dst.eutra_pci = src->eutra_pci;
  } else {
    assert(0 != 0 && "Not implemented");
  }

  return dst;
}


