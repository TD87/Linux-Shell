void execute_jobs(struct cmd * dec_cmd, struct cmd * head);

void execute_overkill(struct cmd * dec_cmd, struct cmd * head);

void execute_kjob(struct cmd * dec_cmd, struct cmd * head);

void execute_fg(struct cmd * dec_cmd, struct cmd ** head, struct cmd ** tail);

void execute_bg(struct cmd * dec_cmd, struct cmd * head);
