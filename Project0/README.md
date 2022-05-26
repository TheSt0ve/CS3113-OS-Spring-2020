Steven Johnson
CS3113 Project0


--------------------
The program can be compiled with:


	make

---------------------
The program can be executed any one of these two ways: 

	./project0 < filename
	
	
	cat filename | ./project0

---------------------
Bugs:
None that I know of. All of my test cases passed.

---------------------
Structs and Functions:

struct Character
	A structure containing bytes for an ASCII or Unicode character as well as counter for the frequency that character appears in the input
	
	
int findCharacter()
	Searches a given array of Character structs for the desired Character, returns the index if found or -1 if not found
	
	
struct Character sortCharacterArray()
	Uses a bubble sort algorithm to sort the Character array in descending order by their frequency
	
	
void printCharacter()
	Prints a single Character struct, mainly used for debugging purposes
	
	
void printAllCharacters()
	Prints the array of Character structs in the desired format
	
