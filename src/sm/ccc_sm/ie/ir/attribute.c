#include "attribute.h"

#include <assert.h>

void free_attribute(attribute_t* src)
{
  assert(src != NULL);
}

bool eq_attribute(attribute_t const* m0, attribute_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false; 

  return true;
}

attribute_t cp_attribute(attribute_t const* src)
{
  assert(src != NULL);

  attribute_t dst = {0};

  return dst;
}


