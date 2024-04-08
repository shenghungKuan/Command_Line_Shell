# Command Line Shell

This is a program that simulates command line shell with some custom features.  
elist: store the commands  
history: implement the history functionality, including retrieving previous commands, listing history commands  
, and searching the command by prefix and command number  
shshsh: the main program of the shell named shshsh  
  


## Building

To compile and run:

```bash
make
./shshsh
```

## The prompt has the following items:  

* Item1: the current working directory  
* Item2: the number of the command  
* Item3: an emoji that indicates whether the last command is valid  
* Item4: the username   

## Running + Example Usage  

[skuan@skuan-vm P3-marcus0329]$ ./shshsh  
😍< 1 > [skuan@skuan-vm:\~/P3-marcus0329]$ ls  
shshsh.c:190:main(): Input command: ls  
Makefile   elist.c  elist.o  history.c  history.o    logger.h  shshsh    shshsh.o  vgcore.2379422  vgcore.2403318  
README.md  elist.h  head     history.h  libshell.so  outputs   shshsh.c  tests     vgcore.2402634  vgcore.2749093  
😍< 2 > [skuan@skuan-vm:\~/P3-marcus0329]$ history  
shshsh.c:190:main(): Input command: history  
1 ls  
2 history  
😍< 3 > [skuan@skuan-vm:~/P3-marcus0329]$  


## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'

# Run a test case in gdb:
make test run=4 debug=on
```
