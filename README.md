# Myshell - A simplified Unix shell
___
## Name : _Subhadeep Bhattacharya_

## Directory Structure:

* src
    * myls
        * myls.h
        * myls.cpp  : Main cpp file for external command myls implementation.
    * myshell.h
    * myshell.cpp   : Entrypoint for the Myshell project.
    * commands.h
    * commands.cpp   : All command handling are done here.
    * utility.h
    * utility.cpp   : Various utilities required for this project are defined here.
* Makefile          : Makefile for building different modules of this project. The code is written using C++. '-Wall -ansi -pedantic' flags are maintained for compilation.
* README.md         : Information and instruction related to this project can be found here.
* build             : This file will get created after make. It will store the object files required for myshell.
    * myshell.o
    * commands.o
    * utility.o
* myls_build        : This file will get generated after make. It store the object files required for myls.
    * myls.o
* bin               : This directory will get generated after make. It will store two executables - myshell and myls.
    * myshell
    * myls

___

## Compilation:
    make            :: Compile the source code. Create required object files. Executables are stored inside 'CPDP_Assignment_1/bin'.
    make clean      :: Remove the object files and executables.

    ./bin/myshell   :: Start the shell. It prints a the username, hostname, current directory location along with a '$' sign and space. It waits for the input until a end                    of file is encountered.

    ./bin/myls      :: External command myls can be tested individually using this executable. myls can be run from myshell also. However, MYPATH environment variable                        need to set before executing this command

___

## Supported Commands:

### _Built-in commands:_
* exit  : Terminate the shell if eof is encountered.
* cd    : Change the directory. Current path, Absolute Path and handling for tilde('~') is also included.
* pwd   : Print the absolute path of the current working directory.
* set   : Set environment variables.

### _External Commands:_
* myls  : Output is same as ls -l.
* all other external commands available through PATH environment variable.

### _Additional Functionalities:_
* pipe                      : Multiple pipe is supported.
* input/output redirection  : Input redirection, output redirection and both input/output redirection is supported.
* background process        : Background process is handled without job control. Prints "Background Process Finished" after the background process is finished.

___
#### NOTE :: _Child process are handled properly. All zombie processes are handled properly. No system() call is used to implement any of the functionality.
___
