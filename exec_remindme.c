#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include "cmd_struct.h"

void execute_remindme(struct cmd * dec_cmd){
    if(dec_cmd->flagc>0){
        printf("\nFlags not supported\n");
        exit(1);
    }
    else if(dec_cmd->argc!=2){
        printf("\nPlease pass only 2 arguments");
        exit(1);
    }
    unsigned int stime=0;
    int temp=0;
    for(int i=0;i<strlen(dec_cmd->args[0]);i++){
        temp=(int)(dec_cmd->args[0][i]-'0');
        if(temp<0 || temp>9){
            printf("\nInvalid sleep time\n");
            exit(1);
        }
        stime=(stime*10)+temp;
    }
    if(sleep(stime)>0){
        printf("\nInterrupted remindme: %s\n",dec_cmd->args[1]);
        exit(1);
    }
    else
        printf("\n%s\n",dec_cmd->args[1]);
    return;
}
