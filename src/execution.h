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

void init_nb_cmd(char ***seq);
void exec_cmd(struct cmdline *line, int i);
void exec_line(struct cmdline *line);
void print_jobs();
void add_job(char *cmd, pid_t pid);
void remove_job(pid_t pid);
