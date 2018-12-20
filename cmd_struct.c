#include<string.h>
#include<stdlib.h>
#include<limits.h>
#include "cmd_struct.h"

char * copy_string(char * command, int i, int last, int offset){
    char * str=(char *)malloc(i-last-offset);
    strncpy(str,command+last+1+(offset/2),i-last-1-offset);
    str[i-last-1-offset]='\0';
    return str;
}

struct cmd * decode(char * command){
    struct cmd * dec_cmd=(struct cmd*)malloc(sizeof(struct cmd));
    int len=strlen(command);
    dec_cmd->argc=0;
    dec_cmd->flagc=0;
    dec_cmd->allc=0;
    dec_cmd->bg=0;
    dec_cmd->inp=0;
    dec_cmd->out=0;
    dec_cmd->pid=0;
    dec_cmd->next=NULL;
    dec_cmd->func=NULL;
    dec_cmd->inp_file=NULL;
    dec_cmd->out_file=NULL;
    dec_cmd->flags=(char **)malloc(50*sizeof(char *));
    dec_cmd->args=(char **)malloc(50*sizeof(char *));
    dec_cmd->all=(char **)malloc(102*sizeof(char *));

    for(int i=0,last=-1,quote=0;i<len;i++){
        if(command[i]=='"' && quote==0)
            quote=1;
        else if(command[i]=='"' && quote==1)
            quote=0;
        else if(command[i]=='\'' && quote==0)
            quote=2;
        else if(command[i]=='\'' && quote==2)
            quote=0;

        if((quote==0 && command[i]==' ') || i==len-1){
            if(i==len-1 && command[i]=='&'){
                dec_cmd->bg=1;
                continue;
            }
            else if(i==len-1)
                i++;

            if(last==-1){
                dec_cmd->func=copy_string(command,i,last,0);
            }
            else if(command[last+1]=='-'){
                dec_cmd->flags[dec_cmd->flagc]=copy_string(command,i,last,0);
                dec_cmd->flagc++;
            }
            else if(command[last+1]=='>' && command[last+2]=='>' && last+3==i){
                dec_cmd->out=2;
                last=i;
                continue;
            }
            else if(command[last+1]=='>' && last+2==i){
                dec_cmd->out=1;
                last=i;
                continue;
            }
            else if(command[last+1]=='<' && last+2==i){
                dec_cmd->inp=1;
                last=i;
                continue;
            }
            else if(dec_cmd->inp==1 && dec_cmd->inp_file==NULL){
                dec_cmd->inp_file=copy_string(command,i,last,0);
                last=i;
                continue;
            }
            else if(dec_cmd->out>0 && dec_cmd->out_file==NULL){
                dec_cmd->out_file=copy_string(command,i,last,0);
                last=i;
                continue;
            }
            else{
                if((command[last+1]=='"' && command[i-1]=='"') || (command[last+1]=='\'' && command[i-1]=='\''))
                    dec_cmd->args[dec_cmd->argc]=copy_string(command,i,last,2);
                else
                    dec_cmd->args[dec_cmd->argc]=copy_string(command,i,last,0);
                dec_cmd->argc++;
            }
            if((command[last+1]=='"' && command[i-1]=='"') || (command[last+1]=='\'' && command[i-1]=='\''))
                dec_cmd->all[dec_cmd->allc]=copy_string(command,i,last,2);
            else
                dec_cmd->all[dec_cmd->allc]=copy_string(command,i,last,0);
            dec_cmd->allc++;
            last=i;
        }
    }
    dec_cmd->all[dec_cmd->allc]=NULL;
    if(strcmp(dec_cmd->func,"remindme")==0)
        dec_cmd->bg=1;
    return dec_cmd;
}

void free_cmd(struct cmd * dec_cmd)
{
    free(dec_cmd->func);
    free(dec_cmd->inp_file);
    free(dec_cmd->out_file);
    for(int i=0;i<dec_cmd->argc;i++)
        free(dec_cmd->args[i]);
    for(int i=0;i<dec_cmd->flagc;i++)
        free(dec_cmd->flags[i]);
    for(int i=0;i<dec_cmd->allc;i++)
        free(dec_cmd->all[i]);
    free(dec_cmd);
}

int clean_command(char * command)
{
    if(command==NULL || command[0]=='\0' || (command[0]=='\t' && command[1]=='\0'))
        return 1;
    char temp[PATH_MAX]="";
    int leading=1, quote=0;
    if(command[strlen(command)-1]=='\n')
        command[strlen(command)-1]='\0';
    for(int i=0,j=0;i<=strlen(command);i++){
        if(quote==0 && command[i]=='\t')
            command[i]=' ';

        if(quote==0 && leading==1 && command[i]==' ')
            continue;
        else if(leading==1)
            leading=0;

        if((quote==1 && command[i]=='"') || (quote==2 && command[i]=='\''))
            quote=0;
        else if(quote==0 && command[i]=='"')
            quote=1;
        else if(quote == 0 && command[i]=='\'')
            quote=2;

        if(quote == 0 && command[i]==' ' && command[i-1]==' ')
            continue;
        else
            temp[j++]=command[i];
    }
    if(temp[strlen(temp)-1]==' ')
        temp[strlen(temp)-1]='\0';
    if(temp[0]=='\0')
        temp[0]='\n';
    strcpy(command,temp);
    if(command[0]==' ' || command[0]=='\n')
        return 1;
    return 0;
}
