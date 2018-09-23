# A Basic shell (ASP Shell) utility over the Linux Kernel.

## Compiling and running
1) Make sure you are connected to the internet
   to be able to install the dependencies (using libreadline for I/O). 

   NOTE: We have assumed debian based OS for installing dependencies in Makefile.
   To install dependencies on rpm based systems, invoke:
   $ sudo yum install libreadline-dev
   
2) run the following commands in order : 
   ```
   $ make dependencies # (for debian based systems only)
   $ make all
   $ ./bin/shell
   ```

## Project Tree / Organization
```
shell                - Project root     
|     
├── inc              - All .h files    
│   ├── handler.h    
│   ├── parse.h    
│   └── shell.h    
├── Makefile    
├── README.md    
└── src              - All .c files    
    ├── handler.c    - handles the parsed commands    
    ├── parse.c      - parses the commands passed     
    └── shell.c      - basic shell logic (taking input invoking parses and handler)  
```

## Makefile commands
make all # or link   - to compile and link   
make build           - to build only    
make clean           - to delete all output files   
make cleanall        - to delete output files as well as TAGS and cscope files   
make TAGS            - to create emacs TAGS   
make cscope          - to create cscope files   
make dependencies    - to install dependencies (debian system)   

## Commands Implemented 
1)  basic shell commands : ls (with -all), cd, rm (with -r -v -f), exit

2)  history n : Prints the most recent n commands issued by the numbers. If n is
				omitted, prints all commands issued by the user.

3)  issue n  : Issues the nth command in the history.

4)  <program_name> : Creates a child process to run <program_name>, also supports 
		     the redirection operators > and <  to redirect the input and 
		     ouput of the program to indicated files.

5)  <program_name> m  : creates a child process to execute program_name, aborts the
 			process if it does not complete its operation in m seconds

6)  rmexcept <list of files> : removes all files except those in <list_of_files> from the current directory.
            		       NOTE: Our implementation does not support exception of file
            		       present in a subdirectory of current working directory.
