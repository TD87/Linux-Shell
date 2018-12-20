#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<limits.h>
#include<fcntl.h>
#include<errno.h>
#include "cmd_struct.h"

void execute_pinfo(struct cmd * dec_cmd, int ppid){
    if(dec_cmd->flagc>0){
        printf("Flags not supported");
        exit(1);
    }
    else if(dec_cmd->argc>1){
        printf("Only one argument is allowed");
        exit(1);
    }
    if(dec_cmd->argc==1){
        ppid=0;
        for(int i=0;i<strlen(dec_cmd->args[0]);i++)
            ppid=(ppid*10)+(int)(dec_cmd->args[0][i]-'0');
    }
    printf("pid -- %d\n",ppid);
    int fd;
    char buffer[PATH_MAX], lpath[PATH_MAX];
    sprintf(buffer,"/proc/%d/stat",ppid);
    fd=open(buffer,O_RDONLY);
    if(fd==-1){
        perror("error in open()");
        exit(errno);
    }
    if(read(fd,buffer,PATH_MAX)==-1){
        perror("error in read()");
        exit(errno);
    }
    char * token=strtok(buffer, " ");
    for(int i=0;token!=NULL;i++){
        if(i==2){
            printf("Process Status -- %s\n",token);
            break;
        }
        token=strtok(NULL, " ");
    }

    sprintf(buffer,"/proc/%d/statm",ppid);
    fd=open(buffer,O_RDONLY);
    if(fd==-1){
        perror("error in open()");
        exit(errno);
    }
    if(read(fd,buffer,PATH_MAX)==-1){
        perror("error in read()");
        exit(errno);
    }
    token=strtok(buffer, " ");
    if(token!=NULL)
        printf("- %s Pages {Virtual Memory}\n",token);
    sprintf(buffer,"/proc/%d/exe",ppid);
    int len=readlink(buffer,lpath,PATH_MAX);
    if(len==-1){
        perror("error in readlink");
        exit(errno);
    }
    lpath[len]='\0';
    printf("- Executable Path -- %s\n",lpath);
    return;
}
