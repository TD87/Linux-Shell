#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<errno.h>
#include "cmd_struct.h"

void change_rel_path(char * home_dir, char * rel_path){
    char cur_dir[PATH_MAX];
    if(getcwd(cur_dir, PATH_MAX)==NULL){
        perror("Error in getcwd()");
        return;
    }
    if(strncmp(home_dir,cur_dir,strlen(home_dir))==0){
        rel_path[0]='~';
        strcpy(rel_path+1, cur_dir+strlen(home_dir));
    }
    else
        strcpy(rel_path,cur_dir);
    return;
}

void execute_cd(struct cmd * dec_cmd, char * home_dir, char * rel_path){
    char dest_path[PATH_MAX];
    if(dec_cmd->flagc>0){
        printf("cd takes no flags\n");
        return;
    }
    if(dec_cmd->argc==0)
        strcpy(dest_path,home_dir);
    else if(dec_cmd->argc>1){
        printf("cd takes only one argument\n");
        return;
    }
    else if(dec_cmd->args[0][0]=='~'){
        strcpy(dest_path,home_dir);
        strcpy(dest_path+strlen(dest_path),dec_cmd->args[0]+1);
    }
    else
        strcpy(dest_path,dec_cmd->args[0]);
    if(chdir(dest_path)==-1){
        perror("Error in chdir");
        return;
    }
    change_rel_path(home_dir, rel_path);
    return;
}
