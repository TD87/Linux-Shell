#include<unistd.h>
#include<stdio.h>
#include<limits.h>
#include<string.h>
#include<pwd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<errno.h>
#include<signal.h>
#include<fcntl.h>
#include "cmd_struct.h"
#include "execute.h"
struct cmd * head=NULL, * tail=NULL;
char host_name[HOST_NAME_MAX], rel_path[PATH_MAX]="~";
char * user_name;

void prompt_init(char * home_dir){
    if(gethostname(host_name, HOST_NAME_MAX)!=0){
        perror("Error in gethostname()");
        exit(errno);
    }
    if(getcwd(home_dir, PATH_MAX)==NULL){
        perror("Error in getcwd()");
        exit(errno);
    }
    user_name=getpwuid(getuid())->pw_name;
    if(user_name==NULL){
        perror("Error in getpwuid()");
        exit(errno);
    }
}

void proc_exit(){
    int status;
    int pid=waitpid(-1,&status,WNOHANG);
    if(pid==-1 || pid==0)
        return;
    struct cmd * cur=head, * actual=NULL;
    for(;cur!=NULL && cur->pid!=pid && cur->next!=NULL && cur->next->pid!=pid;cur=cur->next);
    if(cur->pid==pid){
        actual=cur;
        head=cur->next;
        if(actual==tail)
            tail=head;
    }
    else{
        actual=cur->next;
        cur->next=actual->next;
        if(actual==tail)
            tail=cur;
    }

    if(WEXITSTATUS(status)==0){
        fprintf(stderr, "\n%s with pid %d exited normally\n%s@%s:%s> ",actual->func,pid,user_name,host_name,rel_path);
        fflush(stderr);
    }
    else{
        fprintf(stderr, "\n%s with pid %d exited with status %d\n%s@%s:%s> ",actual->func,pid,WEXITSTATUS(status),user_name,host_name,rel_path);
        fflush(stderr);
    }
    return;
}

int main()
{
    char home_dir[PATH_MAX];
    prompt_init(home_dir);
    int ppid=getpid();
    signal(SIGINT, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCHLD, proc_exit);
    // struct sigaction sigchld_action;
    // sigchld_action.sa_handler = proc_exit;
    // sigemptyset(&sigchld_action.sa_mask);
    // sigchld_action.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    // sigaction(SIGCHLD, &sigchld_action, NULL);

    while(1){
        char * commands=NULL, * saveptr1;
        int pid, bg=0;
        size_t csize=0;
        printf("%s@%s:%s> ",user_name,host_name,rel_path);
        getline(&commands,&csize,stdin);
        clean_command(commands);
        for(char * pcommand=strtok_r(commands, ";", &saveptr1); pcommand!=NULL; pcommand=strtok_r(NULL, ";", &saveptr1)){
            char * saveptr2;
            char * next=strtok_r(pcommand, "|", &saveptr2);
            int init=1,i=0, pipefd[100];
            for(char * command=next; command!=NULL; command=next,init=0,i++){
                next=strtok_r(NULL, "|", &saveptr2);
                if(clean_command(command)==1)
                    continue;
                struct cmd * dec_cmd=decode(command);

                if(pipe(pipefd+(2*i))==-1){
                    perror("error in creating pipe");
                    continue;
                }
                int pipe_state=0;
                if(next!=NULL){
                    dec_cmd->out=0;
                    pipe_state+=1;
                }
                if(init==0){
                    dec_cmd->inp=0;
                    pipe_state+=2;
                }
                if(execute_builtin(dec_cmd, home_dir, rel_path, pipe_state, i, pipefd, &head, &tail)==1){
                    free(dec_cmd);
                    continue;
                }

                pid=fork();
                if(pid<0)
                    perror("Error in fork");
                else if(pid==0){
                    signal(SIGCHLD, SIG_DFL);
                    signal(SIGINT, SIG_DFL);
                    signal(SIGTSTP, SIG_DFL);
                    signal(SIGTTOU, SIG_DFL);
                    signal(SIGTTIN, SIG_DFL);
                    if(setpgid(0,0)!=0)
                        perror("setpgid");

                    if(pipe_state==1 || pipe_state==3)
                        dup2(pipefd[2*i+1],1);
                    if(pipe_state>=2)
                        dup2(pipefd[2*(i-1)],0);
                    close(pipefd[2*i+1]);
                    close(pipefd[2*(i-1)]);

                    execute(dec_cmd, home_dir, ppid, head);
                    exit(0);
                }
                else if(pid>0 && (dec_cmd->bg==0)){
                    if(setpgid(pid,pid)!=0)
                        perror("setpgid");
                    if(tcsetpgrp(0,pid)!=0)
                        perror("tcsetpgrp");
                    waitpid(pid,NULL,WUNTRACED);
                    // waitid(P_PID,pid,NULL,WUNTRACED|WNOWAIT);
                    if(tcsetpgrp(0,getpgid(0))!=0)
                        perror("tcsetpgrp");
                }
                else if(pid>0)
                    waitpid(pid,NULL,WNOHANG);
                if(pid>0){
                    if(pipe_state==1 || pipe_state==3)
                    close(pipefd[2*i+1]);
                    if(pipe_state>=2)
                    close(pipefd[2*(i-1)]);
                    dec_cmd->pid=pid;
                    if(kill(pid,0)!=-1){
                        if(head==NULL)
                            head=dec_cmd;
                        if(tail!=NULL)
                            tail->next=dec_cmd;
                        tail=dec_cmd;
                    }
                    else
                        free_cmd(dec_cmd);
                }
            }
        }
        free(commands);
    }
    return 0;
}
