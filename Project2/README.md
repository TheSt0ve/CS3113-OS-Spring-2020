Can be compiled with:
make
make all



Execute with:
./project2 $FitType$ $AllocationSize$ $InputFile$

$FitType$ can be one of four values
- BESTFIT
- FIRSTFIT
- NEXTFIT
- WORSTFIT

$AllocationSize$ can be any positive number denoting the amount of memory available for the processes
$InputFile$ is the name of the input file that the various commands will originate from



Functions:
void printProcessArray(int firstIndex, struct Process* arr)
- Prints all of the processes in the given array starting from the passed in firstIndex
- NOTE: This function is not used in the final product, it was used solely for debugging purposes



Additional Notes:
- This time around, I was running out of time on my implementaion, so I didn't take too much time to make the code readable. My main() is very bloated, and as such, there is a lot of repeated code that would likely be found in various functions if I took more time.

