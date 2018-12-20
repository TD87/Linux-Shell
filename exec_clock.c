#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include "cmd_struct.h"

void execute_clock(struct cmd * dec_cmd){
    if(!((dec_cmd->flagc==1 && dec_cmd->argc==1) || (dec_cmd->flagc==0 && dec_cmd->argc==0)) || (dec_cmd->flagc==1 && strcmp(dec_cmd->flags[0],"-t")!=0)){
        printf("\nPass only one argument and one flag, or pass neither\n");
        exit(1);
    }
    unsigned int stime=0;
    int temp=0;
    if(dec_cmd->argc==0)
        stime=1;
    for(int i=0;dec_cmd->argc==1 && i<strlen(dec_cmd->args[0]);i++){
        temp=(int)(dec_cmd->args[0][i]-'0');
        if(temp<0 || temp>9){
            printf("\nInvalid interval time\n");
            exit(1);
        }
        stime=(stime*10)+temp;
    }

    char buffer[42];
    int fd=open("/proc/driver/rtc",O_RDONLY);
    if(fd==-1){
        perror("error in open()");
        return;
    }
    if(read(fd,buffer,41)==-1){
        perror("error in read()");
        return;
    }

    buffer[41]='\0';
    int hrs,mins,secs;
    for(int i=11;i<19;i++)
        buffer[i]-='0';
    hrs=(buffer[11]*10)+buffer[12]+5;
    mins=(buffer[14]*10)+buffer[15]+30;
    secs=(buffer[17]*10)+buffer[18];
    while(1){
        if(secs>=60){
            secs-=60;
            mins+=1;
        }
        if(mins>=60){
            mins-=60;
            hrs+=1;
        }
        if(hrs>=24)
            hrs-=24;
        printf("\n%s, %d:%d:%d\n",buffer+31,hrs,mins,secs);
        sleep(stime);
        secs+=stime;
    }
}
