#ifndef G_GNB_ID_LST_H
#define G_GNB_ID_LST_H

#include <stdlib.h>
#include <stdbool.h>

// Conflict type between json schema and ie
// Following CCC json schema type
typedef struct {
  size_t sz_g_gnb_id_lst;
  char** g_gnb_id_lst;
} g_gnb_id_lst_t;

void free_g_gnb_id_lst(g_gnb_id_lst_t* src);

bool eq_g_gnb_id_lst(g_gnb_id_lst_t const* m0, g_gnb_id_lst_t const* m1);

g_gnb_id_lst_t cp_g_gnb_id_lst(g_gnb_id_lst_t const* src);

#endif

