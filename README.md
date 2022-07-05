# Systems-Programming-OS-Lab

A shell that will run as an application program on top of the Linux kernel. The shell will accept user commands (one line at a time), and execute the same. 

The following features are implemented:

a) <b>Run an external command</b>:
  The external commands refer to executables that are stored as files. 
  They have to be executed by spawning a child process and invoking execlp() or some similar system calls. Example user commands:

  ./a.out myprog.c
  
  cc –o myprog myprog.c
  
  ls -l


b) <b>Run an external command by redirecting standard input from a file</b>:
The symbol “<” is used for input redirection, where the input will be read from
the specified file and not from the keyboard. You need to use a system call like
dup() or dup2() to carry out the redirection. Example user command:

./a.out < infile.txt

sort < somefile.txt


c) <b> Run an external command by redirecting standard output to a file</b>:
The symbol “>” is used for output redirection, where the output will be written
to the specified file and not to the screen. You need to use a system call like dup()
or dup2() to carry out the redirection. Example user commands:

./a.out > outfile.txt

ls > abc


d) <b>Combination of input and output redirection</b>:
Here we use both “<” and “>” to specify both types of redirection. Example user
command:

./a.out < infile.txt > outfile.txt


e) <b>Run an external command in the background with possible input and
output redirections</b>:
We use the symbol “&” to specify running a command in the background. The
shell prompt will appear and the next command can be typed while the said
command is being executed in the background. Example user commands:

./a.out &
./myprog < in.txt > out.txt &


f) <b>Run several external commands in the pipe mode</b>:
The symbol “|” is used to indicate pipe mode of execution. Here, the standard
output of one command will be redirected to the standard input of the next
command, in sequence. You need to use the pipe() system call to implement this
feature. Example user commands:

ls | more

cat abc.c | sort | more


g) <b>Interrupting commands running in your shell (using signal call)</b>:
Implement a feature to halt a command running in your shell during
runtime. For instance, if the user presses "Ctrl - c" while a program is
executing, the program should stop executing, and the shell prompt
should reappear. Note that the shell should not stop if the user presses
"Ctrl - c".

Implement a feature to move a command in execution to the background.
If the user presses "Ctrl - z" while a program is executing, the program
execution should move to the background and the shell prompt should
reappear.


h) <b>Implementing a searchable shell history</b>:
Maintain a history of the last 10000 commands run in your shell (hint:
check how bash saves a history in a file). Implement a command “history” which will show the most recent 1000
commands.


i)<b> Implementing auto-complete feature for file names</b>:
Implement an auto-complete feature for the shell for the file names in the
working directory of the shell
