# Small-Shell-
A shell that resembles a normal Linux shell, developed in C++ using OOP and OS principles.


i implement a “smash” (small shell) which will behave
like a real Linux shell but it will support only a limited subset of Linux shell commands.

Description:
you will find a base code for handling commands (command.cpp), a shell code for smash.cpp, and
a Makefile , and the functions that handle and define the signal handling routine will be in the file siganls.cpp.

The program will work as follows:
• The program waits for commands that will be typed by the user and executes them (and
back forth).
• The program can execute a small number of built-in commands, the commands will be
listed below.
• When the program receives a command that is not one of the built-in commands (we call
it an external command), it tries to run it like a normal shell. The way to run external
commands will be described later.
• If one of the built-in commands was typed but with invalid parameters, the following
error message should be printed:
smash error:> “command-line”
where command-line is the command line as typed by the user.
• Whenever an error occurs a proper error message should be printed, and the
program should get back to parse and execute the next typed commands.
• While the program is waiting for the next command to be entered, it should print the
following text (though it could be changed, see chprompt command):smash>

Examples of some commands: 
-chprompt command: chprompt command will allow the user to change the prompt displayed by the smash while waiting for the next command.

-showpid command: showpid command prints the smash pid.

-cd command: Change directory (cd) command receives a single argument <path> that describes the
relative or full path to change the current working directory to it. There is a special
argument that cd can get which is “-“. If “-“ was specified as the only argument of cd
command then it will change the current working directory to the last working
directory.

for more details and shell implementation check the h & cpp code files.


