# Shell Version 1
Zainab Shahid
Linux Shell Program

This shell program allows a user to interact with the sytem. 
When the user enters a command, the shell checks if the command is a built-in or external command
by checking if the command is an absolute or relative path.
When commands are invoked, a child process is forked while the parent waits for the child process to terminate.
The user may prompt a command to run as a background process by adding an amperstand '&' at the end of the command line.
The shell informs the user whether the process is running in the background and when its complete - shows the pid of the process.

Built in commands are as follows:
'cd' - allows the user to change the directory with a given argyment. if no argument is detected, the directory is automatically changed to home.
'exit/logout/quit' - terminates the shell.
'kill' - when given a pid or signal number, the process is killed.
'list/ls' - with no arguments, lists the files in the CWD. With arguments, lists the files in each directory given
'pid' - prints the process id of the shell
'printenv' - prints all of the environment variables with their assigned values
'pwd' - prints the current working directory
'setenv' - lets user set the value of environment variables. when invoked with one argument, it sets the environment as an empty variable. When invoked with two arugments, the second one is the value of the first. When invoked with more than two arguments, an error message is printed and the user returns to the shell.
'which' - locates a file or program in a user's path. if passed the '-a' command arg, displays all of the locations that an executable is found.

Further impelementation is being added to incorporate pipes and threads within the shell and bring background processes into the foreground.

Some Test Input:
list
which ls
which -a ls
ls
ls - a
cd
cd /blah
cd /bin
cd /usr/bin
pwd
pid
kill [pid]
setenv
printenv
setenv TEST
printenv TEST
exit
