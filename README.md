# Basic-Shell-Implementation
 - Below are the the commands implemented in the shell, and the expected behavior of the shell for that command *(Note: I used the fork and exec system calls to implement the shell. The idea is for the main program to act as a parent process that accepts and parses commands and then instantiates child processes to execute the desired commands)*
 - The following functionality are supported:
   - ***cd directory-name*** causes the shell process to change its working directory. This command takes one and only one argument; an incorrect number of arguments (e.g., commands such as cd, cd dir1 dir2 etc.) prints an error in the shell. 
   - ***All simple standalone built-in commands of Linux e.g., (ls, cat, echo, sleep)*** are executed, as they would be in a regular shell. All such commands execute in the foreground, and the shell waits for the command to finish before prompting the user for the next command. Any errors returned during the execution of these commands are displayed in the shell.
   - ***Any simple Linux command followed by the output redirector ’>’*** (e.g., echo hi > hi.txt) causes the output of the command to be redirected to the specified output file. An incorrect command format will print an error and prompt for the next command.
   - ***Any command in Linux with the symbol of piping ”|”*** causes the program on the left side write to STDOUT which can be read from the program on the right side through STDIN that lives in kernel space and allows processes to talk to each other continuously, which is called inter-process communication.
   - ***Any list of simple Linux commands separated by “;;” (e.g., sleep 100 ;; echo hi ;; ls -l)** are all executed in the foreground and sequentially one after the other. The shell starts the execution of the first command, and proceed to the next one only after the previous command completes (successfully or unsuccessfully). 

## How to run the program 
 - Clone this git repository
 - $ cd Basic-Shell-Implementation
 - $ ./turtle
 - ***sample-test-cases.txt*** contains some sample linux command test cases for your reference 
