# Basic-Shell-Implementation
 - The program turtle.c that implements a simple command shell for Linux. 
 - Below are the the commands implemented in the shell, and the expected behavior of the shell for that command(Note: I used the fork and exec system calls to implement the shell. The idea is for the main
program to act as a parent process that accepts and parses commands and then instantiates child processes to execute the desired commands.)
You must not use the system function provided by the C-library. Also, you must execute Linux system programs wherever possible, instead of re-implementing functionality. For example: Given the following command
echo "The assignment is due on October 25" you should use the echo binary available rather than
implementing echo.
The following functionality should be supported:
• cd directory-name must cause the shell process to change its working directory. This command
should take one and only one argument; an incorrect number of arguments (e.g., commands such as cd,
cd dir1 dir2 etc.) should print an error in the shell. cd should also return an error if the change
directory command cannot be executed (e.g., because the directory does not exist). For this, and all
commands below, incorrect number of arguments or incorrect command format should print an error in
the shell. After such errors are printed by the shell, the shell should not crash. It must simply move on
and prompt the user for the next command.
• All simple standalone built-in commands of Linux e.g., (ls, cat, echo, sleep) should be executed, as
they would be in a regular shell. All such commands should execute in the foreground, and the shell should
wait for the command to finish before prompting the user for the next command. Any errors returned
during the execution of these commands must be displayed in the shell.
• Any simple Linux command followed by the output redirector ’>’ (e.g., echo hi > hi.txt) should cause
the output of the command to be redirected to the specified output file. The command should execute in
the foreground. An incorrect command format must print an error and prompt for the next command.
Note: This can achieved by manipulating the sequence of file open and close actions, and/or using the dup
system call.
• Any list of simple Linux commands separated by “;;” (e.g., sleep 100 ;; echo hi ;; ls -l) should all
be executed in the foreground and sequentially one after the other. The shell should start the execution of
the first command, and proceed to the next one only after the previous command completes (successfully
or unsuccessfully). The shell should prompt the user for the next input only after all the commands have
finished execution. An error in one of the commands should simply cause the shell to move on to the next
command in the sequence. An incorrect command format must print an error and prompt for the next
command.
Basics of Operating System Shells and Signal Handling – Assignment #03 - Sprint #02 6
Important Guidelines
• When a process completes its execution, all of the memory and resources associated with it are de-allocated
so they can be used by other processes. This cleanup has to be done by the parent of the process and is
called reaping the child process. The shell must also carefully reap all its children that have terminated.
For commands that must run in the foreground, the shell must wait for and reap its terminated foreground
child processes before it prompts the user for the next input.
• By carefully reaping all children (foreground and background), the shell must ensure that it does not leave
behind any zombies or orphans when it exits.
• You must implement all the commands above in your shell. Test your shell using several test cases.
Key Tips
• You are given a sample code make-tokens.c that takes a string of input, and “tokenizes” it (i.e.,
separates it into space-separated commands). You may find it useful to split the user’s input string into
individual commands.
• You may assume that the input command has no more than 1024 characters, and no more than 64 “tokens”.
Further, you may assume that each token is no longer than 64 characters.
• You may want to build a simple shell that runs simple Linux built-in commands first, before you go on to
tackle the more complicated cases.
• You will find the dup system call and its variants useful in implementing I/O redirection and pipes. When
you dup a file descriptor, be careful to close all unused, extra copies of the file descriptor. Also recall
that child processes will inherit copies of the parent file descriptors, so be careful and close extra copies
of inherited file descriptors also. Otherwise, your I/O redirection and pipe implementations will not work
correctly.
• You must catch the SIGINT signal in your shell and handle it correctly, so that your shell does not
terminate on a Ctrl+C, but only on receiving the exit command.
• You will find the chdir system call useful to implement the cd command.
• Carefully handle all error cases listed above for each command. For example, an incorrect command string
should always print an error.
