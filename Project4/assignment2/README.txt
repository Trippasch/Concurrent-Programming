---------------------------------------------------------
|                    Assignment 2                       |
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

Execute:

    To execute the program, run the command:

        ./main <int>

    where <int> is the number of threads to be created as
    workes from the main thread. The program requires input
    from standard input, so in our case we redirect stdin to
    the file "primes10.txt" which has 10 prime numbers from
    472,882,049 to 472,882,219. So the execution command should
    be:

        ./main <int> < primes10.txt > output.txt

    We also redirect stdout to the file "output.txt" to keep our
    terminal as clean as possible.
