#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "variante.h"
#include "readcmd.h"
#include "execution.h"

struct background_job *main_job;

void exec_cmd(struct cmdline *line){
   if(line->seq[0] == NULL ){
       return ;
   }
   char **seq = line->seq[0];
   pid_t pid_f = fork();

   if(pid_f == -1){
       perror("the process cannot be created \n");
       exit(EXIT_FAILURE);
   }
   else if( pid_f == 0){
       if(line->out != NULL ){
           FILE *s_out = fopen(line->out, "a");

           dup2(fileno(s_out), STDOUT_FILENO);
           dup2(fileno(s_out), STDERR_FILENO);
       }
       execvp(seq[0], seq);
   }
   else{
       if(line->bg == 0){
         wait(NULL);
       }
       
   }
}

void print_jobs(){
   printf("pid \t nom du processus \n");
   struct background_job *job_tmp =  main_job;

   while(job_tmp != NULL ){
       printf("%d \t %s", job_tmp->pid, job_tmp->cmd);
   }
}

void add_job(struct background_job *job){
   if(main_job == NULL ){
       main_job = job;
   }

   else{
       struct background_job *job_tmp = main_job;
       while(job_tmp->next == NULL){
           job_tmp = job_tmp->next;
       }

       job_tmp->next = job;
   }
}

void remove_job(struct background_job *job){
    struct background_job *job_tmp, *next;
    job_tmp = main_job;

    if(job_tmp == NULL ){
        while(job_tmp->next != job){
            job_tmp = job_tmp->next;
        }

        next = job_tmp->next->next;
        job_tmp->next = next;
    }
}
