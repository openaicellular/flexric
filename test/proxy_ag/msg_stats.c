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
  char* str = read_file("msg_stats.json");

  msg_stats_amr_t out = {0};
  dec_msg_stats_amr(str, &out);

  free_msg_stats_amr(&out);
  free(str);
  return EXIT_SUCCESS;
}



