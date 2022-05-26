
//Steven Johnson CS3113 Spring 2021

#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>

#define maxBytes 5
#define lengthOfArray 1500000



//A struct meant to symbolize any character, including ASCII and Unicode
struct Character{
	//holds the representative bytes for the character
	unsigned char bytes[maxBytes];

	//the frequency that this Character appear in the input
	int count;
};



//This function is used to locate a given element in the array
//Returns the index of the located element or -1 if it is not in the array
int findCharacter(struct Character* arr, unsigned char Bytes[maxBytes], int size){
	for(int i = 0; i < size; i++){
		if(strcmp(arr[i].bytes, Bytes) == 0){
			return i;
		}
	}
	return -1;
}


//This function is used for ordering the array in descending order by each Character's count value
struct Character* sortCharacterArray(struct Character* var, int size){
	for(int outer = 0; outer < size-1; outer++){
		for(int inner = 0; inner < size-outer-1; inner++){
			if(var[inner].count < var[inner+1].count){
				struct Character temp = var[inner];
				var[inner] = var[inner+1];
				var[inner+1] = temp;
			}
		}
	}
	return var;
}


//This function is used for debugging purposes, prints one Character struct
void printCharacter(struct Character var){
	//printf("Called printCharacter\n");
	if(sizeof(var.bytes) == 0){
		perror("Error: cannot print an empty character");
		exit(-1);
	}

	int byteLength = sizeof(var.bytes) / sizeof(var.bytes[0]);
	for(int i = 0; i < maxBytes-1; i++){
		printf("%c", var.bytes[i]);
	}
	printf("->%d\n", var.count);

	//printf("%s->%d\n", var.bytes, var.count);
}


//This function prints the entire array
void printAllCharacters(struct Character* arr, int size){
	for(int i; i < size; i++){
		printf("%s->%d\n", arr[i].bytes, arr[i].count);
		//printCharacter(arr[i]);
	}
}



int main(int argc, char** argv){
	int arrPos;
	int size = 0;
	struct Character* structArr;
	structArr = calloc(lengthOfArray, sizeof(structArr));

	while(read(0, structArr[size].bytes, sizeof(char)) == sizeof(char)){
		//Check if it is a UTF character or not
		int numOfBytes;
		if(structArr[size].bytes[0] > 127){
			//printf("It is a UTF character\n");

			//Check how many bytes long this UTF character will be and add more bytes accordingly
			//After finding this info, see if the character is already in the list
			if(structArr[size].bytes[0] < 224){
				numOfBytes = 2;
				memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
				lseek(0, -1, SEEK_CUR);
				read(0, structArr[size].bytes, numOfBytes);
				arrPos = findCharacter(structArr, structArr[size].bytes, size);
				if(arrPos == -1){
					structArr[size].count++;
					size++;
				}
				else{
					memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
					structArr[arrPos].count++;
				}
			}
			else if(structArr[size].bytes[0] < 240){
				numOfBytes = 3;
				memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
				lseek(0, -1, SEEK_CUR);
				read(0, structArr[size].bytes, numOfBytes);
				arrPos = findCharacter(structArr, structArr[size].bytes, size);
				if(arrPos == -1){
					structArr[size].count++;
					size++;
				}
				else{
					memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
					structArr[arrPos].count++;
				}
			}
			else if(structArr[size].bytes[0] < 255){
				numOfBytes = 4;
				memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
				lseek(0, -1, SEEK_CUR);
				read(0, structArr[size].bytes, numOfBytes);
				arrPos = findCharacter(structArr, structArr[size].bytes, size);
				if(arrPos == -1){
					structArr[size].count++;
					size++;
				}
				else{
					memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
					structArr[arrPos].count++;
				}
			}
			else{
				perror("Error: Character is more than 4 bytes long");
				exit(-1);
			}
		}
		//The character is only 1 byte long and can be printed via ASCII
		else{
			numOfBytes = 1;
			arrPos = findCharacter(structArr, structArr[size].bytes, size);
			if(arrPos == -1){
				structArr[size].count++;
				size++;
			}
			else{
				memset(structArr[size].bytes, 0, sizeof(structArr[size].bytes));
				structArr[arrPos].count++;
			}
		}
	}

	fflush(stdout);
	structArr = sortCharacterArray(structArr, size);

	fflush(stdout);
	printAllCharacters(structArr, size);

	free(structArr);

	return 0;
}

