#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<dirent.h>
#include<grp.h>
#include<time.h>
#include<langinfo.h>
#include<pwd.h>
#include<limits.h>
#include<unistd.h>
#include<errno.h>
#include "cmd_struct.h"

void print_perms(mode_t perms){
    switch (perms & S_IFMT){
        case S_IFBLK:
            printf("b");
            break;
        case S_IFCHR:
            printf("c");
            break;
        case S_IFDIR:
            printf("d");
            break;
        case S_IFIFO:
            printf("p");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFREG:
            printf("-");
            break;
        case S_IFSOCK:
            printf("s");
            break;
        default:
            printf("?");
            break;
    }
    printf( (perms & S_IRUSR) ? "r" : "-");
    printf( (perms & S_IWUSR) ? "w" : "-");
    printf( (perms & S_IXUSR) ? "x" : "-");
    printf( (perms & S_IRGRP) ? "r" : "-");
    printf( (perms & S_IWGRP) ? "w" : "-");
    printf( (perms & S_IXGRP) ? "x" : "-");
    printf( (perms & S_IROTH) ? "r" : "-");
    printf( (perms & S_IWOTH) ? "w" : "-");
    printf( (perms & S_IXOTH) ? "x" : "-");
}

int check_ls(int argc, int flagc){
    if(argc>1){
        printf("ls takes only one argument\n");
        return 1;
    }
    else if(flagc>2){
        printf("ls doesn't takes more than 2 flags\n");
        return 1;
    }
    return 0;
}

int count_flag_ls(struct cmd * dec_cmd, int * l, int * a){
    for(int i=0;i<dec_cmd->flagc;i++){
        if(strcmp(dec_cmd->flags[i],"-l")==0)
            *l=1;
        else if(strcmp(dec_cmd->flags[i],"-a")==0)
            *a=1;
        else if(strcmp(dec_cmd->flags[i],"-al")==0 || strcmp(dec_cmd->flags[i],"-la")==0){
            *l=1, *a=1;
        }
        else{
            printf("Unsupported flag\n");
            return 1;
        }
    }
    return 0;
}

void gen_target_dir_ls(struct cmd * dec_cmd, char * home_dir, char * target_dir){
    char cur_dir[PATH_MAX];
    if(getcwd(cur_dir, PATH_MAX)==NULL){
        perror("Error in getcwd()");
        exit(errno);
    }
    if(dec_cmd->argc==1 && dec_cmd->args[0][0]=='~')
        sprintf(target_dir,"%s%s",home_dir,dec_cmd->args[0]+1);
    else if(dec_cmd->argc==1)
        strcpy(target_dir,dec_cmd->args[0]);
    else
        strcpy(target_dir,cur_dir);
    if(dec_cmd->argc==1 && target_dir[strlen(target_dir)-1]=='/')
        target_dir[strlen(target_dir)-1]='\0';
    return;
}

void print_long_ls(struct stat * statbuf, char * dname){
    struct passwd *pwd;
    struct group *grp;
    struct tm *tm;
    char datestring[256];
    print_perms(statbuf->st_mode);
    printf(" %4ld", statbuf->st_nlink);
    if ((pwd = getpwuid(statbuf->st_uid)) != NULL)
        printf(" %-8.8s", pwd->pw_name);
    else
        printf(" %-8d", statbuf->st_uid);
    if ((grp = getgrgid(statbuf->st_gid)) != NULL)
        printf(" %-8.8s", grp->gr_name);
    else
        printf(" %-8u", statbuf->st_gid);
    printf(" %9jd", statbuf->st_size);
    tm = localtime(&statbuf->st_mtime);
    strftime(datestring, sizeof(datestring), nl_langinfo(D_T_FMT), tm);
    printf(" %s %s\n", datestring, dname);
}

void execute_ls(struct cmd * dec_cmd, char * home_dir){
    int l=0,a=0;
    if(check_ls(dec_cmd->argc, dec_cmd->flagc) || count_flag_ls(dec_cmd,&l,&a))
        exit(1);
    DIR *dir;
    struct dirent *dp;
    struct stat statbuf;
    char target_dir[PATH_MAX], abs_path[PATH_MAX];
    gen_target_dir_ls(dec_cmd,home_dir,target_dir);

    if(dec_cmd->argc==0 && (dir=opendir("."))==NULL){
        perror("Error in opendir()");
        exit(errno);
    }
    else if(dec_cmd->argc==1 && (dir=opendir(target_dir))==NULL){
        perror("Error in opendir()");
        exit(errno);
    }

    while ((dp=readdir(dir))!=NULL){
        if(a==0 && dp->d_name[0]=='.')
            continue;
        else if(l==1){
            sprintf(abs_path,"%s/%s",target_dir,dp->d_name);
            if (stat(abs_path, &statbuf) == -1){
                perror("Error in stat()");
                continue;
            }
            print_long_ls(&statbuf, dp->d_name);
        }
        else
            printf("%s\n",dp->d_name);
    }
    closedir(dir);
    return;
}
