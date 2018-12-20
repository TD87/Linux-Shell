#include<string.h>
#include<signal.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<fcntl.h>
#include "cmd_struct.h"
#include "exec_cd.h"
#include "exec_pwd.h"
#include "exec_echo.h"
#include "exec_ls.h"
#include "exec_pinfo.h"
#include "exec_remindme.h"
#include "exec_clock.h"
#include "exec_env.h"
#include "jobs.h"

void redirect_files(struct cmd * dec_cmd){
    if(dec_cmd->inp==1){
        int fd=open(dec_cmd->inp_file, O_RDONLY);
        if(fd==-1)
            perror("Failed to open file");
        else{
            close(0);
            if(dup2(fd,0)!=0)
                perror("dup2 fail");
            close(fd);
        }
    }
    if(dec_cmd->out>0){
        int fd;
        if(dec_cmd->out==1)
            fd=open(dec_cmd->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        else
            fd=open(dec_cmd->out_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if(fd==-1)
            perror("Failed to open file");
        else{
            close(1);
            if(dup2(fd,1)!=1)
                perror("dup2 fail");
            close(fd);
        }
    }
}

void restore_default(struct cmd * dec_cmd, int saved_stdin, int saved_stdout){
    if(dec_cmd->out>0){
        close(1);
        if(dup2(saved_stdout,1)!=1)
            perror("Error in restoring stdout");
    }
    if(dec_cmd->inp==1){
        close(0);
        if(dup2(saved_stdin,0)!=0)
            perror("Error in restoring stdin");
    }
}

int execute_builtin(struct cmd * dec_cmd, char * home_dir, char * rel_path, int pipe_state, int i, int * pipefd, struct cmd ** head, struct cmd ** tail){
    int saved_stdin=dup(0);
    int saved_stdout=dup(1);
    int return_val=0;
    redirect_files(dec_cmd);
    if(strcmp(dec_cmd->func,"pwd")==0){
        execute_pwd(dec_cmd, pipe_state, i, pipefd);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"echo")==0){
        execute_echo(dec_cmd, pipe_state, i, pipefd);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"cd")==0){
        execute_cd(dec_cmd, home_dir, rel_path);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"overkill")==0){
        execute_overkill(dec_cmd, *head);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"kjob")==0){
        execute_kjob(dec_cmd, *head);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"fg")==0){
        execute_fg(dec_cmd, head, tail);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"bg")==0){
        execute_bg(dec_cmd, *head);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"setenv")==0){
        execute_setenv(dec_cmd);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"unsetenv")==0){
        execute_unsetenv(dec_cmd);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"getenv")==0){
        execute_getenv(dec_cmd);
        return_val=1;
    }
    else if(strcmp(dec_cmd->func,"exit")==0 && dec_cmd->argc==0 && dec_cmd->flagc==0){
        execute_overkill(dec_cmd, *head);
        restore_default(dec_cmd,saved_stdin,saved_stdout);
        exit(0);
    }
    restore_default(dec_cmd,saved_stdin,saved_stdout);
    return return_val;
}

void execute(struct cmd * dec_cmd, char * home_dir, int ppid, struct cmd * head){
    redirect_files(dec_cmd);
    if(strcmp(dec_cmd->func,"ls")==0)
        execute_ls(dec_cmd, home_dir);
    else if(strcmp(dec_cmd->func,"pinfo")==0)
        execute_pinfo(dec_cmd, ppid);
    else if(strcmp(dec_cmd->func,"remindme")==0)
        execute_remindme(dec_cmd);
    else if(strcmp(dec_cmd->func,"clock")==0)
        execute_clock(dec_cmd);
    else if(strcmp(dec_cmd->func,"jobs")==0)
        execute_jobs(dec_cmd, head);
    else if(execvp(dec_cmd->func,dec_cmd->all)==-1){
        perror("Error in execvp()");
        exit(errno);
    }
    return;
}
