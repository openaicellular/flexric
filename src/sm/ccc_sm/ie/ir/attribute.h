#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include "../../../../util/byte_array.h"

typedef struct {
  // Attribute name
  // Mandatory
  // 9.3.8
  byte_array_t attribute_name;
} attribute_t;

void free_attribute(attribute_t* src);

bool eq_attribute(attribute_t const* m0, attribute_t const* m1);

attribute_t cp_attribute(attribute_t const* src);

#endif

