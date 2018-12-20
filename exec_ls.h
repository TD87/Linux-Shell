void print_perms(mode_t perms);

int check_ls(int argc, int flagc);

int count_flag_ls(struct cmd * dec_cmd, int * l, int * a);

void gen_target_dir_ls(struct cmd * dec_cmd, char * home_dir, char * target_dir);

void print_long_ls(struct stat * statbuf, char * dname);

void execute_ls(struct cmd * dec_cmd, char * home_dir);
