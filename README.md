# Command Line Shell

This is a program that simulates command line shell with some custom features.  
* Elist: the custom data structure for storing the commands history.  
* History: implement the history functionality, including retrieving previous commands, listing history commands, and searching the command by prefix and command number.  
* shshsh: the main program of the shell named shshsh  
  


## Building

To compile and run:

```bash
make
./shshsh
```

## The prompt has the following items:  

* Item1: an emoji that indicates whether the last command is valid  
* Item2: the number of the command  
* Item3: username, hostname, and current working directory  

## Running + Example Usage  

[username@hostname current_working_directory]$ make  
[username@hostname current_working_directory]$ ./shshsh  
😍< 1 > [username@hostname current_working_directory]$ ls  
Makefile &nbsp; elist.c &nbsp; history.c &nbsp; logger.h &nbsp; README.md &nbsp; elist.h &nbsp; head &nbsp; history.h &nbsp; shshsh.c  
😍< 2 > [username@hostname current_working_directory]$ history  
1 ls  
2 history  
😍< 3 > [username@hostname current_working_directory]$  
