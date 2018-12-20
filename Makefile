objects =  shell.c exec_cd.c cmd_struct.c exec_pwd.c exec_echo.c exec_ls.c exec_pinfo.c exec_remindme.c exec_clock.c execute.c exec_env.c jobs.c 

shell : $(objects)
	gcc -o shell $(objects) -I .
