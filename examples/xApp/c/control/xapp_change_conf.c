#include "../../../../src/xApp/e42_xapp_api.h"
#include "../../../../src/util/time_now_us.h"
#include "../../../../src/util/alg_ds/ds/lock_guard/lock_guard.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <dirent.h>

int get_pid_gNB(char* const name){
  size_t taskNameSize = 1024;
  char* taskName = calloc(1, taskNameSize);
  defer({free(taskName);});

  const char* directory = "/proc";
  DIR* dir = opendir(directory);

  if (dir){
    struct dirent* de = 0;
    while ((de = readdir(dir)) != 0)
    {
//      printf("Current file name: %s \n", de->d_name);
      // Avoid . and .. fileName
      if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        continue;

      int pid = -1;
      // Read the int part of the string d_name
      int res = sscanf(de->d_name, "%d", &pid);

      if (res == 1){
        char cmdline_file[1024] = {0};
        // Assign cmdline file name
        sprintf(cmdline_file, "%s/%d/cmdline", directory, pid);

        FILE* cmdline = fopen(cmdline_file, "r");
        if (getline(&taskName, &taskNameSize, cmdline) > 0){
          // Compare substring
          // is it the process we care about?
          if (strstr(taskName, name) != 0)
          {
            fprintf(stdout, "A %s process, with PID %d, has been detected.\n", name, pid);
            return pid;
            break;
          }
        }
        fclose(cmdline);
      }
    }
    closedir(dir);
  }

  return -1;
}

void modify_conf_file(char* const file, char* const input){
  FILE *fptr;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  fptr = fopen(file, "r+");
  assert(fptr != NULL);

  while ((read = getline(&line, &len, fptr)) != -1) {
    if (strstr(line, "bandwidth: ") != 0) {
      printf("Retrieved line of len %zu:\n", read);
      printf("%s", line);

      // Find the indentation of the original line
      char *indent_start = line;
      while (*indent_start == ' ' || *indent_start == '\t') {
        indent_start++; // Increase the pointer
      }
      size_t indent_length = indent_start - line;

      // Prepare the new line with the same indentation
      char* const new_line = input;

      // Prepend the indentation to the new line
      char indented_new_line[1024];

      snprintf(indented_new_line, sizeof(indented_new_line), "%.*s%s", (int)indent_length, line, new_line);

      // Pad the new line with spaces if it is shorter than the original line
      size_t new_line_length = strlen(indented_new_line);
      if (new_line_length < read) {
        memset(indented_new_line + new_line_length, ' ', read - new_line_length - 1); // -1 to exclude newline
        indented_new_line[read - 1] = '\n'; // Ensure the new line ends with a newline character
        indented_new_line[read] = '\0'; // Null-terminate the string
      }

      // Move the file pointer back to the start of the line
      fseek(fptr, -read, SEEK_CUR);

      // Write the new line in place
      int rc = fputs(indented_new_line, fptr);
      if (rc == EOF) {
        printf("Failed to replace the line\n");
      }

      // Move the file pointer to the next line start
      fseek(fptr, 0, SEEK_CUR);
    }
  }

  free(line);
  fclose(fptr);
}

int main(int argc, char *argv[])
{
  int pid = get_pid_gNB("amr");
  // pid_t for the amr
  if (pid != -1){
    if (kill(pid, SIGKILL) == -1) {
      perror("kill");
      return 1;
    }
  }

  modify_conf_file("/var/snap/amr-ran/common/conf/gnb-two-nr-cells-hos.cfg", "test");

  char* command = "sudo amr-ran > /tmp/log.txt &";

  int rc = system(command);
  if (rc == -1){
    printf("Failed to restart GNB\n");
  }

  return 0;
}