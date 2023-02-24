# Operating System Project

This is a project developed during the lab of Operative System subject in the second year of Computer Science at University of A Coruña.

This program consists of a shell similar to a bash with some included functions and the ability to run any other command on the Unix System.

## Basic Usage

You can get a list of all the included functions by running the command "ayuda", and a small description of each one by running "ayuda command". You can also run any command installed in your system. To exit the shell you can run salir,fin,bye or exit.

## How to run the program 

You need to have gcc installed in a Unix Operating System. You can compile it with:

```
make
```

And run it with:

```
./shell
```
## What is on each file 

The code is divided into different files to make it easier to work and understand 

# p3.c

Contains the main function plus the prompt and processing functions

# libraries

Contains all the headers for every function except the list specific ones with a small description. Includer all the libraries that we use. Defines structs and data types that are used in the code

# list.c and list.h

Contains the implementation of the list ADT to save different kinds of values. In this case we used a linked list with a header node to store the data.

# auxFunc.c

Contains the other functions that don't fit in the previous categories to separate a big part of the code into an indepent file in order to favor readability.




