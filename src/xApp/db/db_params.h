//
// Created by modelx on 18/07/23.
//

#ifndef BR_FLEXRIC_DB_PARAMS_H
#define BR_FLEXRIC_DB_PARAMS_H

#include "util/conf_file.h"

typedef struct{
    bool enable;
    char* ip;
    char* dir;
    char* db_name;
#ifdef MYSQL_XAPP
    char* user;
    char* pass;
#endif
} db_params_t;

void get_db_params(fr_args_t const* args, db_params_t* params);

void free_db_params(db_params_t* params);

#endif //BR_FLEXRIC_DB_PARAMS_H
