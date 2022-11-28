# POSIX SHELL

We have developed a POSIX compatible shell which supports a subset of features of the default linux shell. Our shell creates its own .myrc file to store its environment variables. It supports various features which are mentioned below.

## Execution

**complie**: g++ main.cpp

**run**: ./a.out

## Commands Supported

1.  IO Redirection

    `>>` and `>`
    space between command, operator, and filename is necessary. eg. ls > out.txt is valid. ls>out.txt is not.

2.  Pipe

    `|`

3.  Root user prompt change

    `sudo -i`
    For exiting, use `exit`

4.  Background command execution

    `&` is passed as last token of command

5.  Make process foreground

    `fg`

6.  Environment variables supported
    - PATH
    - HOME
    - USER
    - HOSTNAME
    - PS1
    - HISTSIZE
7.  `~` is associated with HOME
8.  Alias support for commands

    `alias ll='ls -l'`

    `alias ll="ls -l"`

    `alias ll=" ls -l "`

    `alias d=ls`

9.  `$$` : shows pid of current process

    `echo $$` or

    `echo anytext$$anytext`

10. `$?`: shows status of last executed process

    `echo $?` or

    `echo anytext$?anytext`

11. `record start`: starts recording input and output of shell in "recording.txt" file

12. `record stop`: stops recording

13. `history` : shows history of commands upto size stored in HISTSIZE

14. `history keyword`: filters and shows all lines in history containing keyword

15. `TAB`: auto-completes / shows list of commands starting with the typed prefix

16. `export` : Used to access all variables, and add, modify and delete PS1, HISTSIZE, the user created environment variables and the aliases which are stored in .myrc.

    `export` or `export -p` to display
    `export varname=value` to add
    `export alias varname="value"` to add an alias
    `export existing_varname=value` to modify
    `export alias existing_varname="value"` to modify alias
    `export -n varname` to delete user created environment variables
    `export -n alias varname` to delete an alias

17. `alarm seconds message`
    to create an alarm after the mentioned seconds

18. `open file`: opens a file using the application specified in .myrc
19. `cd path`: changes the directory to given path. '~' support is available.
    `cd .` stays in current directory
    `cd ..` to go to parent directory
    `cd path` to go to a particular directory
    `cd ~` goes to the home directory
20. `pwd`: prints current working directory
21. `env` and `printenv`: displays environment variables stored in .myrc
    `printenv` and `env` to display the variables
    `printenv varname` to display the value of a particular environment variable
    `printenv alias name` to print the value of the alias
22. `echo`: to print in shell
23. `exit`: terminate shell

## Assumptions

- HISTSIZE is initialised as 2000
- TAB key is pressed only after typing some prefix of command
- Control keys such as arrows, Ctrl + C etc. are not functional
- The user created environment variables must not be space separated
- The `$$` and the `$?` can be accessed using the echo command eg. `echo $$`, otherwise they print the corresponding value along with a command not found message as in the normal shell.
- The aforementioned ways of using export have been implemented
- Environment variables are accessed via printenv, env and export
- Pipe symbol is space sensitive. It should not have spaces before and after
- while recording, output is not shown in terminal, stored in file instead.
