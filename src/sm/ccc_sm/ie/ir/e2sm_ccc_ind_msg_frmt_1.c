#include "e2sm_ccc_ind_msg_frmt_1.h"

#include <assert.h>
#include <stdlib.h>

static
void free_ind_msg_node_conf(ind_msg_node_conf_t* src)
{
  assert(src != NULL);
  assert(src->change_type == CHANGE_TYPE_ADDITION ||
          src->change_type == CHANGE_TYPE_DELETION ||
          src->change_type == CHANGE_TYPE_MODIFICATION ||
          src->change_type == CHANGE_TYPE_NONE);

  free_byte_array(src->ran_conf_name);

  free_byte_array(src->val_attribute);

  // Optional
  if (src->old_val_attribute.buf != NULL)
    free_byte_array(src->old_val_attribute);
}

static
bool eq_ind_msg_node_conf(ind_msg_node_conf_t const* m0, ind_msg_node_conf_t  const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->change_type != m1->change_type)
    return false;

  if(eq_byte_array(&m0->ran_conf_name, &m1->ran_conf_name) == false)
    return false;

  if(eq_byte_array(&m0->val_attribute, &m1->val_attribute) == false)
    return false;

  // Optional
  if(m0->old_val_attribute.buf != NULL && m1->old_val_attribute.buf != NULL)
    if(eq_byte_array(&m0->old_val_attribute, &m1->old_val_attribute) == false)
      return false;

  return true;
}

static
ind_msg_node_conf_t cp_ind_msg_node_conf(ind_msg_node_conf_t const* src)
{
  assert(src != NULL);
  assert(src->change_type == CHANGE_TYPE_ADDITION ||
         src->change_type == CHANGE_TYPE_DELETION ||
         src->change_type == CHANGE_TYPE_MODIFICATION ||
         src->change_type == CHANGE_TYPE_NONE);

  ind_msg_node_conf_t dst = {.change_type = src->change_type};

  dst.ran_conf_name = copy_byte_array(src->ran_conf_name);

  dst.val_attribute = copy_byte_array(src->val_attribute);

  if (src->old_val_attribute.buf != NULL)
    dst.old_val_attribute = copy_byte_array(src->old_val_attribute);

  return dst;
}

void free_e2sm_ccc_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const* src)
{
  assert(src != NULL);

  // List of Configuration Structures Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_node_conf > 0 && src->sz_ind_msg_node_conf < 65535);

  for(size_t i = 0; i < src->sz_ind_msg_node_conf; ++i){
    free_ind_msg_node_conf(&src->ind_msg_node_conf[i]);
  }

  assert(src->ind_msg_node_conf!= NULL);
  free(src->ind_msg_node_conf);
}


bool eq_e2sm_ccc_ind_msg_frmt_1( e2sm_ccc_ind_msg_frmt_1_t const* m0,  e2sm_ccc_ind_msg_frmt_1_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if(m0->sz_ind_msg_node_conf != m1->sz_ind_msg_node_conf)
    return false;

  for(size_t i = 0; i < m0->sz_ind_msg_node_conf; ++i){
    if(eq_ind_msg_node_conf(&m0->ind_msg_node_conf[i], &m1->ind_msg_node_conf[i]) == false){
      return false;
    }
  }

  return true;
}


e2sm_ccc_ind_msg_frmt_1_t cp_e2sm_ccc_ind_msg_frmt_1(e2sm_ccc_ind_msg_frmt_1_t const* src)
{
  assert(src != NULL);

  e2sm_ccc_ind_msg_frmt_1_t dst = {0};

  // List of Configuration Structures Reported
  // [1 - 65535]
  assert(src->sz_ind_msg_node_conf > 0 && src->sz_ind_msg_node_conf < 65535);
  dst.sz_ind_msg_node_conf = src->sz_ind_msg_node_conf;

  dst.ind_msg_node_conf = calloc(dst.sz_ind_msg_node_conf, sizeof(ind_msg_node_conf_t));
  assert(dst.ind_msg_node_conf != NULL && "Memory exhausted");

  for(size_t i = 0; i < src->sz_ind_msg_node_conf; ++i){
    dst.ind_msg_node_conf[i] = cp_ind_msg_node_conf(&src->ind_msg_node_conf[i]);
  }

  return dst;
}

