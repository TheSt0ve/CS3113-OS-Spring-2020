Steven Johnson
CS3113 Spring 2021

The program can be run using
./project1
./project1 < filename
cat filename | ./project1

One assumption I made(documented in my code) is that the number of voluntary switches is equal to the number of unique PIDs. This is because each unique process has one unique termination point, so although they would typically be incremented at different times, the values will end up the same once all processes have terminated.

No known bugs, Gradescope gave me full marks. I have called my printUniqueInstructions function near the end of main, but it prints to stderr so it should not affect anything.

Functions
printAllInstructions: Print out each of the instructions passed in, along with some handy information about them
printUniqueInstructions: Print out the last occurrence of each unique process, along with the metadata used to calculate the different values
findFirstOccurrence: Checks if a given PID is in the array, if it is, return the first index it is found at, if it is not found return -1 
findNextOccurence: Starting from a given position, checks if a given PID is found later in the array, if it is return that "next" index, if not one return -1
findPreviousOccurrence: Starting from a given position, checks if a given PID is found earlier in the array, if it is found returns that "previous" index, if it is not return -1
findTerminatedIndices: Finds the last occurrence of each unique process and stores their indices in an array
calcThroughput: Calculates the throughput and returns that value as an unrounded double
calcAveTurnaroundTime: Calculates the average turnaround time and returns that value as an unrounded double
calcAveWaitingTime: Calculates the average turnaround time and returns that value as an unrounded double
calcAveResponseTime: Calculates the average response time and returns that value as an unrounded double

Sources
overiq.com/c-programming-101/fscanf-function-in-c
log2base2.com/c-questions/io/how-to-print-double-value-in-c.html#:~:text=We%20can%20print%20the%20double,to%20print%20a%20double%20value.
