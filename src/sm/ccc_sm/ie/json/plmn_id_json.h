//
// Created by khai on 05/04/24.
//

#ifndef FLEXRIC_PLMN_IDENTITY_CCC_H
#define FLEXRIC_PLMN_IDENTITY_CCC_H

#include "cJSON.h"

typedef struct {
  char * mcc;
  char * mnc;
} plmn_id_json_t;

plmn_id_json_t * cJSON_Parseplmn_id(const char * s);
plmn_id_json_t * cJSON_Getplmn_idValue(const cJSON * j);
cJSON * cJSON_Createplmn_id(const plmn_id_json_t * x);
char * cJSON_Printplmn_id(const plmn_id_json_t * x);
void cJSON_Deleteplmn_id(plmn_id_json_t * x);


#endif //FLEXRIC_PLMN_IDENTITY_CCC_H
