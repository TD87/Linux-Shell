int execute_builtin(struct cmd * dec_cmd, char * home_dir, char * rel_path, int pipe_state, int i, int * pipefd, struct cmd ** head, struct cmd ** tail);

void execute(struct cmd * dec_cmd, char * home_dir, int ppid, struct cmd * head);

void redirect_files(struct cmd * dec_cmd);

void restore_default(struct cmd * dec_cmd, int saved_stdin, int saved_stdout);
