#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include "cmd_struct.h"

void execute_echo(struct cmd * dec_cmd, int pipe_state, int i, int * pipefd){
    if(dec_cmd->flagc>0){
        printf("Flags not supported\n");
        return;
    }
    if(pipe_state>=2){
        close(pipefd[2*(i-1)]);
    }
    if(pipe_state==1 || pipe_state==3){
        for(int i=0;i<dec_cmd->argc;i++)
            dprintf(pipefd[2*i+1],"%s",dec_cmd->args[i]);
        dprintf(pipefd[2*i+1],"\n");
        close(pipefd[2*i+1]);
    }
    else{
        for(int i=0;i<dec_cmd->argc;i++)
            printf("%s",dec_cmd->args[i]);
        printf("\n");
    }
    return;
}
