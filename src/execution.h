#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

#include "variante.h"

struct background_job {
   char *cmd;
   pid_t pid;
   struct background_job *next;
};

void exec_cmd(struct cmdline *line);