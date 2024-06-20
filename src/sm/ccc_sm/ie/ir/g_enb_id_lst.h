#ifndef G_ENB_ID_LST_H
#define G_ENB_ID_LST_H

#include <stdlib.h>
#include <stdbool.h>

// Conflict type between json schema and ie
// Following CCC json schema type
typedef struct {
  size_t sz_g_enb_id_lst;
  char** g_enb_id_lst;
} g_enb_id_lst_t;

void free_g_enb_id_lst(g_enb_id_lst_t* src);

bool eq_g_enb_id_lst(g_enb_id_lst_t const* m0, g_enb_id_lst_t const* m1);

g_enb_id_lst_t cp_g_enb_id_lst(g_enb_id_lst_t const* src);

#endif

