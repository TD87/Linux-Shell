#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<limits.h>
#include<errno.h>
#include "cmd_struct.h"

void execute_pwd(struct cmd * dec_cmd, int pipe_state, int i, int * pipefd){
    if(dec_cmd->flagc>0 || dec_cmd->argc>0){
        printf("pwd takes no flags or arguments\n");
        return;
    }
    char cur_dir[PATH_MAX];
    if(getcwd(cur_dir, PATH_MAX)==NULL){
        perror("Error in getcwd()");
        return;
    }
    if(pipe_state>=2){
        close(pipefd[2*(i-1)]);
    }
    if(pipe_state==1 || pipe_state==3){
        dprintf(pipefd[2*i+1],"%s\n",cur_dir);
        close(pipefd[2*i+1]);
    }
    else
        printf("%s\n",cur_dir);
    return;
}
