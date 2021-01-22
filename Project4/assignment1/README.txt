---------------------------------------------------------
|                    Assignment 1                       |
|            Giannoukkos Panayiotis 2280                |
|            Choropanitis Paschalis 2453                |
|                       Group 6                         |
---------------------------------------------------------

Compile:

    To compile this assignment we included a Makefile to make
    the process much easier. All you hava to to is run the
    command:
        
        $ make all

    You can find each make target inside the Makefile.

Execution:
    
    To execute the program run the command:

        $ ./main <int> <input_file>

    where <int> is the size of the buffer and <input_file>
    is a file from which we get input. After execution, a
    file called `output.txt` will be created.

    At the end of execution the program uses the diff command
    to check if <input_file> and output.txt have the same contents.
    The return value of diff is printed on standard error and if it
    is 0, it means the two files are identical.

    For your convenience we include "mega_book.txt" and "mini.txt" to be used
    as <input_file>.