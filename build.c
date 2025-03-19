#include "libxtr.h"
#include <stdio.h>

char *PROGRAM_NAME = "game-of-life";
char *CC = "gcc";

int main(int argc, char *argv[])
{
  printf("INFO: Start building: %s \n", PROGRAM_NAME);

  StrBld cmd = {0};
  StrBld_new(&cmd);

  StrBld_add_many(&cmd, CC, "main.c", "-o", PROGRAM_NAME);
}
