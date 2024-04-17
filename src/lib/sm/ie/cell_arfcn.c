#include "cell_arfcn.h"
#include <assert.h>
#include <stdlib.h>

void free_cell_arfcn(cell_arfcn_t* src)
{
  assert(src != NULL);
  (void)src;
}

cell_arfcn_t cp_cell_arfcn(cell_arfcn_t const* src)
{
  assert(src != NULL);

  cell_arfcn_t dst = {.type = src->type}; 

  if(dst.type == NR_RAT_TYPE){
    // [0,3279165]
    assert(src->nr_arfcn < 3279166); 
    dst.nr_arfcn = src->nr_arfcn;
  } else if(dst.type == EUTRA_RAT_TYPE){
    // [0,65535] 6.2.3.33
    dst.eutra_arfcn = src->eutra_arfcn;
  } else {
    assert(0!=0 && "Unknown RAT type");
  }

  return dst;
}

bool eq_cell_arfcn(cell_arfcn_t const* m0, cell_arfcn_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->type != m1->type)
    return false;

  if(m0->type == NR_RAT_TYPE){
    // [0,3279165]  6.2.3.30  
    assert(m0->nr_arfcn < 3279166);
    assert(m1->nr_arfcn < 3279166);
    if(m0->nr_arfcn != m1->nr_arfcn)
      return false;
  } else if(m0->type == EUTRA_RAT_TYPE){
    // [0,65535] 6.2.3.33
    if(m0->eutra_arfcn != m1->eutra_arfcn)
      return false;
  } else {
    assert(0 != 0 && "Unknown RAT type");
  }

  return true;
}

