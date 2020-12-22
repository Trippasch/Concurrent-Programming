---------------------------------------------------------
|                    Assignment 4                       |
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

    The execute commands are the same with assignments 3.1 and 3.2.

    -- 3.1
    
    To execute the program, run the command:

        $ ./prime_CCR <int>

    where <int> is the number of threads to be created as
    workes from the main thread. The program requires input
    from standard input, so in our case we redirect stdin to
    the file "primes100.txt" which has 100 prime numbers from
    472,882,049 to 472,884,059. So the execution command should
    be:

        $ ./prime_CCR <int> < primes100.txt > output.txt

    We also redirect stdout to the file "output.txt" to keep our
    terminal as clean as possible.

    -- 3.2

    To execute the program, run the command:

        $ ./bridge_CCR

    The program requires input
    from standard input, so in our case we redirect stdin to
    a file from the folder ./tests which has different cases of generated cars.
    So the execution command should be:

        ./bridge_CCR < tests/test1.txt