#include "../../src/proxy_ag/amarisoft/lib/dec/dec_msg_amr_json.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

char* read_file(char const* filename) 
{
  FILE *f = fopen(filename, "rt");
  assert(f && "Check that you are executing the program from the same place where the json file is located");
  fseek(f, 0, SEEK_END);
  long length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buffer = calloc(length + 1, sizeof(char));
  fread(buffer, 1, length, f);
  fclose(f);
  return buffer;
}

int main()
{
  char* str = read_file("ayo_config_get.json");
  char* str2 = read_file("roland_config_get.json");
  char* str3 = read_file("config_get.json");

  msg_config_get_amr_t out = {0};
  
  dec_config_get_amr(str, &out);
  free_msg_config_get_amr(&out);

  dec_config_get_amr(str2, &out);
  free_msg_config_get_amr(&out);

  dec_config_get_amr(str3, &out);
  free_msg_config_get_amr(&out);

  free(str);
  free(str2);
  free(str3);

  printf("Success running the config get decoding test.\n");

  return EXIT_SUCCESS;
}

