#include "g_enb_id_lst.h"

#include <assert.h>
#include <string.h>

void free_g_enb_id_lst(g_enb_id_lst_t* src)
{
  assert(src != NULL);

  for (size_t i = 0; i < src->sz_g_enb_id_lst; i++)
    free(src->g_enb_id_lst[i]);

  free(src->g_enb_id_lst);
}

bool eq_g_enb_id_lst(g_enb_id_lst_t const* m0, g_enb_id_lst_t const* m1)
{
  if(m0 == m1)
    return true;

  if(m0 == NULL || m1 == NULL)
    return false;

  if (m0->sz_g_enb_id_lst != m1->sz_g_enb_id_lst)
    return false;

  for (size_t i = 0; i < m0->sz_g_enb_id_lst; i++){
    if (strcmp(m0->g_enb_id_lst[i], m1->g_enb_id_lst[i]) != 0){
      return false;
    }
  }

  return true;
}

g_enb_id_lst_t cp_g_enb_id_lst(g_enb_id_lst_t const* src)
{
  assert(src != NULL);

  g_enb_id_lst_t dst = {0};

  if (src->sz_g_enb_id_lst > 0){
    dst.sz_g_enb_id_lst = src->sz_g_enb_id_lst;
    dst.g_enb_id_lst = calloc(src->sz_g_enb_id_lst, sizeof(char*));
    assert(dst.g_enb_id_lst != NULL);
    for (size_t i = 0; i < src->sz_g_enb_id_lst; ++i){
      dst.g_enb_id_lst[i] = strdup(src->g_enb_id_lst[i]);
    }
  }

  return dst;
}


