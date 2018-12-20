#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<limits.h>
#include<errno.h>
#include<string.h>
#include "cmd_struct.h"

void execute_setenv(struct cmd * dec_cmd){
    if(dec_cmd->flagc>0){
        printf("setenv takes no flags\n");
        return;
    }
    else if(dec_cmd->argc<1 || dec_cmd->argc>2){
        printf("setenv takes either 1 or 2 arguments\n");
        return;
    }

    if(dec_cmd->argc==1)
        dec_cmd->args[1][0]='\0';
    if(setenv(dec_cmd->args[0],dec_cmd->args[1],1)==-1){
        perror("error in setenv");
        return;
    }
}

void execute_unsetenv(struct cmd * dec_cmd){
    if(dec_cmd->flagc>0){
        printf("unsetenv takes no flags\n");
        return;
    }
    else if(dec_cmd->argc!=1){
        printf("unsetenv takes only 1 arguments\n");
        return;
    }

    if(unsetenv(dec_cmd->args[0])==-1){
        perror("error in unsetenv");
        return;
    }
}

void execute_getenv(struct cmd * dec_cmd){
    if(dec_cmd->flagc>0){
        printf("getenv takes no flags\n");
        return;
    }
    else if(dec_cmd->argc!=1){
        printf("getenv takes only 1 arguments\n");
        return;
    }

    char * val = getenv(dec_cmd->args[0]);
    if(val!=NULL)
        printf("%s\n",val);
    else{
        printf("environment variable doesn't exist\n");
        return;
    }
}
