struct cmd{
    char * func, *inp_file, *out_file;
    char ** flags, ** args, ** all;
    int argc, flagc, bg, allc, inp, out, pid;
    struct cmd * next;
};

struct cmd * decode(char * command);

void free_cmd(struct cmd * dec_cmd);

int clean_command(char * command);

char * copy_string(char * command, int i, int last, int offset);
