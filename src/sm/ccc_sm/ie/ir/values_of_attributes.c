#include "values_of_attributes.h"

#include <assert.h>
#include <stddef.h>

void free_values_of_attributes(values_of_attributes_t* src)
{
  assert(src != NULL);
}

bool eq_values_of_attributes(values_of_attributes_t const* m0, values_of_attributes_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  return true;
}


values_of_attributes_t cp_values_of_attributes(values_of_attributes_t const* src)
{
  assert(src != NULL);

  values_of_attributes_t dst = {0};

  return dst;
}



