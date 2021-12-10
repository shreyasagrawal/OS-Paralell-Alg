# Operating Systems and Parallel Algorithms class assignments
1. Basic N-Gram generator in C (ngram.c)
To compile: run this command in the terminal: gcc ngram.c -o ngram
To run: run this exemplar command in the terminal: echo -n "This is a test" | ./ngram 3
Can replace "This is a test" to any string. N can be changed too.

2. Sorted-List
This data structure behaves like a normal list, except it maintains all of the values in sorted order (lowest to highest).
Linked list approach
Run the makefile to compile.
run the program by "./sorted-list" and then give it numbers to add on to the list using the "insert" command, followed by a number, 
print them using the "print" command, and count how many instances of a number is in the list by using the "count" command followed by the number.

3. ls_command_implementation
This program is an implementation of the ls command on the command line
The program prints the names of all the files and directories in a specified directory, 
  along with the user and group that own the file, the file’s permissions, and its size.
myls command takes one optional parameter; if this parameter is provided, 
  it is the path to the directory that should be examined. 
If no parameter is provided myls lists the current directory.


4. Sudoku solver (Sudoku_parallel directory). Solves upto 1 million sudoku puzzles per second using threads and CUDA. 
To compile:
nvcc sudoku.cu -o sudoku
* must have a nvidia GPU on the machine. Might need to modify bashrc file to add path to gpu if not there already *

To run:
./sudoku <input file name>
Inputs in a directory inside sudoku


5. Peer to peer chat (p2pchat directory). A network where clients can connect to a server hosting, and other clients can connect to each other. Text messaging is supported between a network and messages are broadcasted to all the clients on the network. 

To compile:
type "make" in the terminal

To run:
TO HOST A SERVER: ./p2pchat <username>
TO JOIN A SERVER AS A CLIENT AND ALSO HOST: ./p2pchat <username> localhost (or computer name of the one hosting) server_port_number
