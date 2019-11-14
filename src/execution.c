#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "variante.h"
#include "readcmd.h"
#include "execution.h"

static struct background_job *main_job;
int process_pipe[2];
int data_pipe[2][2];
int nb_cmd;

void init_nb_cmd(char ***seq){
    int i =0;
    while (seq[i] != NULL) {
        i++;
    }

    nb_cmd = i;
}
void exec_cmd(struct cmdline *line, int i){
    char **seq = line->seq[i];
    int pipe_in, pipe_out ;
    
    pipe_in =  data_pipe[i%2][0] ;
    pipe_out = data_pipe[(i+1)%2][1];

    close(data_pipe[i%2][1]);

    if ( seq == NULL) {
        return ;
    }
    
    // on change la sortie standard si suivant les conditions
    if (line->seq[i+1] != NULL){
        dup2(pipe_out, STDOUT_FILENO);
        dup2(pipe_out, STDERR_FILENO);
           
    } else if (line->out != NULL && line->seq[i+1] == NULL) {
        FILE *s_out = fopen(line->out, "a");

        dup2(fileno(s_out), STDOUT_FILENO);
        dup2(fileno(s_out), STDERR_FILENO);
    }

    // on change l'entrée standard suivant les conditions
    if (i == 0 && line->in != 0){
        FILE *s_in = fopen(line->in, "r");
        dup2(fileno(s_in), STDIN_FILENO);
    } else if (i != 0){
        dup2(pipe_in, STDIN_FILENO);
    }

    if (strcmp(seq[0], "jobs")==0) {
        print_jobs();
        close(pipe_in);
        close(pipe_out);
    } else {
        execvp(seq[0], seq);
    }
   
}

void exec_line(struct cmdline *line){
    int i=0;

    init_nb_cmd(line->seq);
    
    if (pipe(data_pipe[0]) == -1 ) {
        perror("error while creating pipe \n");
        exit(EXIT_FAILURE);
    }

    while(line->seq[i] != NULL ) {
        if(pipe(data_pipe[(i+1)%2]) == -1){
            perror("error while creating pipe \n");
            exit(EXIT_FAILURE);
        }

        pid_t pid_f = fork();

        if(pid_f == -1){
            perror("the process cannot be created \n");
            exit(EXIT_FAILURE);
        }

        if (pid_f == 0) {
            exec_cmd(line, i);
            exit(EXIT_SUCCESS);
        }
        else{
            if(line->bg == 0 || i < nb_cmd - 1){
                wait(NULL);
            } else{
                add_job(line->seq[i][0], pid_f);
            }
            //int status;
            //pid_t pid = waitpid(-1, &status, WNOHANG);
            //remove_job(pid);
        }
        
        
        close(data_pipe[i%2][1]);
        i++;
    }
   
}

void print_jobs(){
   printf("pid \t nom du processus \n");
   struct background_job *job_tmp =  main_job;

   while(job_tmp != NULL ){
       printf("%d \t %s \n", job_tmp->pid, job_tmp->cmd);
       job_tmp = job_tmp->next;
   }
}

void add_job(char *cmd, pid_t pid){
   char *c;
   c = malloc(strlen(cmd)*sizeof(char));
   sprintf(c, "%s", cmd);
   struct background_job *job = malloc(sizeof(struct background_job ));
   job->cmd = c;
   job->pid = pid;
   job->next = NULL;
   if(main_job == NULL ){
       main_job = job;
   }

   else{
       struct background_job *job_tmp = main_job;
       while(job_tmp->next != NULL){
           /*if(job_tmp->pid == pid){
               break;
           }*/
           job_tmp = job_tmp->next;
       }

       job_tmp->next = job;
   }
}

void remove_job(pid_t pid){
    printf(" le pid est : %d \n", pid);
    struct background_job *job_tmp, *prev;
    job_tmp = main_job;

    if(job_tmp != NULL ){
        while(job_tmp != NULL && job_tmp->pid != pid){
            prev = job_tmp;
            job_tmp = job_tmp->next;
        }
      
        if(job_tmp == NULL){
            perror("Want to remove the process in background wich was not added \n");
        }
        else{
            prev->next = job_tmp->next;
            free(job_tmp->cmd);
            free(job_tmp);
        }
        
    }
}
