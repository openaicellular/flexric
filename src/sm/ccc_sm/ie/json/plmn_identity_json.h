//
// Created by khai on 05/04/24.
//

#ifndef FLEXRIC_PLMN_IDENTITY_CCC_H
#define FLEXRIC_PLMN_IDENTITY_CCC_H

#include "cJSON.h"

typedef struct {
  char * mcc;
  char * mnc;
} plmn_identity_json_t;

plmn_identity_json_t * cJSON_Parseplmn_identity(const char * s);
plmn_identity_json_t * cJSON_Getplmn_identityValue(const cJSON * j);
cJSON * cJSON_Createplmn_identity(const plmn_identity_json_t * x);
char * cJSON_Printplmn_identity(const plmn_identity_json_t * x);
void cJSON_Deleteplmn_identity(plmn_identity_json_t * x);


#endif //FLEXRIC_PLMN_IDENTITY_CCC_H
