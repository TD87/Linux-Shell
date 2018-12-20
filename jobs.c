#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#include<limits.h>
#include<errno.h>
#include<string.h>
#include<signal.h>
#include "cmd_struct.h"

void execute_jobs(struct cmd * dec_cmd, struct cmd * head){
    if(dec_cmd->flagc>0){
        printf("jobs takes no flags\n");
        exit(1);
    }
    else if(dec_cmd->argc>0){
        printf("jobs takes no arguments\n");
        exit(1);
    }

    struct cmd * cur=head;
    int fd;
    char buffer[PATH_MAX], lpath[PATH_MAX];
    for(int i=1;cur!=NULL;cur=cur->next,i++){
        sprintf(buffer,"/proc/%d/status",cur->pid);
        fd=open(buffer,O_RDONLY);
        if(fd==-1){
            perror("error in open()");
            exit(errno);
        }
        if(read(fd,buffer,PATH_MAX)==-1){
            perror("error in read()");
            exit(errno);
        }
        char * token=strtok(buffer, " \n");
        for(int j=0;token!=NULL;j++){
            if(j==3){
                printf("[%d] %s %s [%d]\n",i,token,cur->func,cur->pid);
                break;
            }
            token=strtok(NULL, " \n");
        }
    }
}

void execute_overkill(struct cmd * dec_cmd, struct cmd * head){
    if(dec_cmd->flagc>0){
        printf("overkill takes no flags\n");
        return;
    }
    else if(dec_cmd->argc>0){
        printf("overkill takes no arguments\n");
        return;
    }

    struct cmd * cur=head;
    for(;cur!=NULL;cur=cur->next)
        kill(- cur->pid,SIGKILL);
}

void execute_kjob(struct cmd * dec_cmd, struct cmd * head){
    if(dec_cmd->flagc>0){
        printf("kjob takes no flags\n");
        return;
    }
    else if(dec_cmd->argc!=2){
        printf("kjob takes only 2 arguments\n");
        return;
    }

    struct cmd * cur=head;
    for(int i=1;cur!=NULL && i<atoi(dec_cmd->args[0]);cur=cur->next,i++);
    if(cur!=NULL && kill(- cur->pid,atoi(dec_cmd->args[1]))==-1)
        perror("kill error");
}

void execute_fg(struct cmd * dec_cmd, struct cmd ** head, struct cmd ** tail){
    if(dec_cmd->flagc>0){
        printf("fg takes no flags\n");
        return;
    }
    else if(dec_cmd->argc!=1){
        printf("fg takes only 1 argument\n");
        return;
    }
    else if(atoi(dec_cmd->args[0])<1){
        printf("Enter a number greater than or equal to 1\n");
        return;
    }

    struct cmd * cur=*head, * actual=NULL;
    for(int i=1;cur!=NULL && i+1<atoi(dec_cmd->args[0]);cur=cur->next,i++);
    if(cur==NULL || (atoi(dec_cmd->args[0])!=1 && cur->next==NULL)){
        printf("Job not found\n");
        return;
    }
    if(atoi(dec_cmd->args[0])!=1)
        actual=cur->next;
    else
        actual=cur;
    if(kill(actual->pid,SIGCONT)==-1){
        perror("kill error");
        return;
    }

    if(actual==*head){
        *head=actual->next;
        if(actual==*tail)
        *tail=*head;
    }
    else{
        cur->next=actual->next;
        if(actual==*tail)
        *tail=cur;
    }
    actual->next=NULL;

    if(tcsetpgrp(0,actual->pid)!=0)
        perror("tcsetpgrp");
    waitpid(actual->pid,NULL,WUNTRACED);
    if(tcsetpgrp(0,getpgid(0))!=0)
        perror("tcsetpgrp");

    if(kill(actual->pid,0)!=-1){
        if(*head==NULL)
            *head=actual;
        if(*tail!=NULL)
            (*tail)->next=actual;
        *tail=actual;
    }
}

void execute_bg(struct cmd * dec_cmd, struct cmd * head){
    if(dec_cmd->flagc>0){
        printf("bg takes no flags\n");
        return;
    }
    else if(dec_cmd->argc!=1){
        printf("bg takes only 1 argument\n");
        return;
    }
    else if(atoi(dec_cmd->args[0])<1){
        printf("Enter a number greater than or equal to 1\n");
        return;
    }

    struct cmd * cur=head;
    for(int i=1;cur!=NULL && i<atoi(dec_cmd->args[0]);cur=cur->next,i++);
    if(cur!=NULL && kill(- cur->pid, SIGCONT)==-1)
        perror("kill error");
}
