### Features

  - Supports multiple semi colon separated commands.
  - pwd, echo, cd, overkill, kjob, fg, bg, setenv, unsetenv & getenv, and exit are built-in commands.
  - ls, pinfo, remindme ,clock & jobs are non-built-in commands.
  - All the commands, with the exception built-in commands, can be run in the background.
  - ls has been implemented for -l, -a, and -la/-al.
  - Completed background processes print a message with their exit status, if they encountered an error, and their pid.
  - In pinfo, the unit for virtual memory is pages.
  - Foreground and background processes have been implemented.
  - Clock command can be run with or without arguments. If no argument is given, interval time is taken as 1.
  - User can exit from terminal using exit command. Ctrl-C does not terminate the shell process.
  - **~ : Home** is supported for paths.
  - Tabs, leading spaces, trailing spaces and multiple spaces are trimmed.
  - Quotation mark support is available for all text, ie, spaces or tabs are not trimmed if they are within quotes.
  - Error handling has been implemented for all system calls and incorrect syntax in any command.
  - Input and output redirection separately and together is supported. **>>** is also supported
  - Piping is supported. Can be used in conjunction with redirection.
  - Ctrl-Z to push foreground process to background is supported.

### Implementation

The shell has been implemented in a modular fashion. There are separate files for separate tasks.

  - **shell.c**         : The main file that prints the prompt, takes input, creates child processes for each
                          command and handles piping.
  - **execute.c**       : Calls the specific functions to execute the command and handles input output redirection.
  - **cmd_struct.c**    : Defines a struct, known as cmd, that is a decoded version of each command inputted by the
                          user, along with all it's related functions to the struct.
  - **exec_cd.c**       : Executes cd command.
  - **exec_clock.c**    : Executes clock command.
  - **exec_echo.c**     : Executes echo command.
  - **exec_env.c**      : Executes setenv, getenv, unsetenv commands.
  - **exec_ls.c**       : Executes ls command.
  - **exec_pinfo.c**    : Executes pinfo command.
  - **exec_pwd.c**      : Executes pwd command.
  - **exec_remindme.c** : Executes remindme command.
  - **jobs.c**          : Executes jobs, overkill, kjob, fg, bg commands.

### Usage

```sh
$ make shell
$ ./shell
```
