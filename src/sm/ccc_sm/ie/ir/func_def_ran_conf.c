#include "func_def_ran_conf.h"

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>

static
void free_lst_sp_attribute(lst_sp_attributes_t* src)
{
  assert(src != NULL);
  
  // Attribute name
  // Mandatory
  // 9.3.8
  free_byte_array(src->attribute_name);

  // Supported Services
  // Mandatory
  // 9.2.2.2
  free_supported_services(&src->supported_services);
}

static
bool eq_lst_sp_attribute(lst_sp_attributes_t const* m0, lst_sp_attributes_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // Attribute name
  // Mandatory
  // 9.3.8
  if(!eq_byte_array(&m0->attribute_name, &m1->attribute_name))
    return false;

  // Supported Services
  // Mandatory
  // 9.2.2.2
  if(!eq_supported_services(&m0->supported_services, &m1->supported_services))
    return false;

  return true;
}

//static
//lst_sp_attributes_t cp_lst_sp_attribute(lst_sp_attributes_t const* src)
//{
//  assert(src != NULL);
//
//  lst_sp_attributes_t dst = {0};
//
//  // Attribute name
//  // Mandatory
//  // 9.3.8
//  dst.attribute_name = copy_byte_array(src->attribute_name);
//
//  // Supported Services
//  // Mandatory
//  // 9.2.2.2
//  dst.supported_services = cp_supported_services(&src->supported_services);
//
//  return dst;
//}

void free_func_def_ran_conf(func_def_ran_conf_t* src)
{
  assert(src != NULL);

  // RAN configuration structure name
  // Mandatory
  free_byte_array(src->ran_conf_str_name);

  // List of supported attributes
  // [0..65535]
  assert(src->sz_lst_sp_attributes < 65536);
  for (size_t i = 0; i < src->sz_lst_sp_attributes; i++){
    free_lst_sp_attribute(&src->lst_sp_attributes[i]);
  }

  if (src->lst_sp_attributes != NULL)
    free(src->lst_sp_attributes);
}

bool eq_func_def_ran_conf(func_def_ran_conf_t const* m0, func_def_ran_conf_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  // RAN configuration structure name
  // Mandatory
  if(!eq_byte_array(&m0->ran_conf_str_name, &m1->ran_conf_str_name))
    return false;

  // List of supported attributes
  // [0..65535]
  assert(m0->sz_lst_sp_attributes < 65536);
  assert(m1->sz_lst_sp_attributes < 65536);
  for (size_t i = 0; i < m0->sz_lst_sp_attributes; i++){
    if(!eq_lst_sp_attribute(&m0->lst_sp_attributes[i], &m1->lst_sp_attributes[i]))
      return false;
  }

  return true;
}


func_def_ran_conf_t cp_func_def_ran_conf(func_def_ran_conf_t const* src)
{
  assert(src != NULL);

  func_def_ran_conf_t dst = {0};

  // RAN configuration structure name
  // Mandatory
  dst.ran_conf_str_name = copy_byte_array(src->ran_conf_str_name);

  // List of supported attributes
  // [0..65535]
  assert(src->sz_lst_sp_attributes < 65536);

  dst.sz_lst_sp_attributes = src->sz_lst_sp_attributes;
  if (src->lst_sp_attributes != NULL){
    dst.lst_sp_attributes = calloc(src->sz_lst_sp_attributes, sizeof(lst_sp_attributes_t));
    assert(dst.lst_sp_attributes != NULL && "Memory exhausted");
    memcpy(dst.lst_sp_attributes, src->lst_sp_attributes, sizeof(lst_sp_attributes_t) * src->sz_lst_sp_attributes);
  }

  return dst;
}



