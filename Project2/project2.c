#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define nameSize 35
#define commandSize 12
#define arraySize 100

struct Process{
	char id[nameSize];
	int size;
	int start;

	int previousProcess;
	int nextProcess;

	int order;
};


void printProcessArray(int firstIndex, struct Process* arr){
		int i = firstIndex;
		if(i == -1){
			return;
		}
		while(i != -1){
			printf("ID: %s Start: %d\n", arr[i].id, arr[i].start);
			i = arr[i].nextProcess;
		}
}



int main(int argc, char** argv){

	char* fitType;
	int memAllocation;
	FILE* fp;
	char fileName[nameSize];

	//Series of if-else statements that handle the various parameters passed in
	if(argc < 4){
		perror("Not enough arguments passed in");
		exit(-1);
	}
	else if(argc > 4){
		perror("Too many arguments passed in");
		exit(-1);
	}
	else{
		//Identify the allocation algorithm specified by the algorithm parameter
		if(strcmp(argv[1], "BESTFIT") == 0 || strcmp(argv[1], "FIRSTFIT") == 0
		 || strcmp(argv[1], "WORSTFIT") == 0 || strcmp(argv[1], "NEXTFIT") == 0){
			fitType = argv[1];
		}
		else{
			perror("Invalid allocation algorithm");
			exit(-1);
		}

		//Record the amount of memory specified by the N parameter
		memAllocation = atoi(argv[2]);

		//Attempt to open the file specified by the script parameter
		strcpy(fileName, argv[3]);
		fp = fopen(fileName, "r");
		if(fp == NULL){
			perror("Unable to open input file");
			exit(-1);
		}
	}


	char* command = NULL;
	ssize_t len = 0;

	struct Process* structArr;
	structArr = calloc(arraySize, sizeof(struct Process));
	char inputOne[commandSize];
	char inputTwo[nameSize];
	int inputThree = 0;

	int index = 0;
	int firstIndex = -1;
	int location = 0;
	//int endingLocation;
	int gapSize;
	int loopIndex;

	while(getline(&command, &len, fp) != -1){
		sscanf(command, "%s %s %d", inputOne, inputTwo, &inputThree);
		if(strcmp(inputOne, "REQUEST") == 0){
			if(firstIndex == -1){
				structArr[index].previousProcess = -1;
				structArr[index].nextProcess = -1;
				structArr[index].start = 0;
				if(memAllocation >= inputThree){
					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					firstIndex = index;
					index++;
				}
				else{
					printf("FAIL REQUEST %s %d\n", inputTwo, inputThree);
				}
			}
			else if(strcmp(fitType, "BESTFIT") == 0){
				int wasAdded = 0;
				int smallestWorkingGapSize = memAllocation + 1;
				int gapIndex = -2;

				//check for a gap before the first index
				if(structArr[firstIndex].start != 0){
					gapSize = structArr[firstIndex].start;
					if(inputThree <= gapSize){
						smallestWorkingGapSize = gapSize;
						gapIndex = -1;
					}
				}
				//check the array for the smallest working gap
				loopIndex = firstIndex;
				while(structArr[loopIndex].nextProcess != -1){
					gapSize = (structArr[structArr[loopIndex].nextProcess].start) - (structArr[loopIndex].start + structArr[loopIndex].size);
					if(gapSize > 0){
						if(inputThree <= gapSize && gapSize < smallestWorkingGapSize){
							smallestWorkingGapSize = gapSize;
							gapIndex = loopIndex;
						}
					}
					loopIndex = structArr[loopIndex].nextProcess;
				}
				//check if there is a gap after the last index
				gapSize = memAllocation - (structArr[loopIndex].start + structArr[loopIndex].size);
				if(gapSize > 0 && gapSize < smallestWorkingGapSize && inputThree <= gapSize){
					smallestWorkingGapSize = gapSize;
					gapIndex = loopIndex;
				}

				if(gapIndex == -2){
					//then no gap was found
					printf("FAIL REQUEST %s %d\n", inputTwo, inputThree);
				}
				else if(gapIndex == -1){
					//the gap comes before the first index
					structArr[index].previousProcess = -1;
					structArr[index].nextProcess = firstIndex;
					structArr[firstIndex].previousProcess = index;
					structArr[index].start = 0;
					firstIndex = index;

					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
				else if(structArr[gapIndex].nextProcess == -1){
					//the gap comes after the last index
					structArr[gapIndex].nextProcess = index;
					structArr[index].previousProcess = gapIndex;
					structArr[index].nextProcess = -1;
					structArr[index].start = (structArr[gapIndex].start + structArr[gapIndex].size);

					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
				else{
					//its a standard gap
					structArr[index].nextProcess = structArr[gapIndex].nextProcess;
					structArr[index].previousProcess = gapIndex;
					structArr[structArr[gapIndex].nextProcess].previousProcess = index;
					structArr[gapIndex].nextProcess = index;
					structArr[index].start = (structArr[gapIndex].start + structArr[gapIndex].size);

					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
			}
			else if(strcmp(fitType, "FIRSTFIT") == 0){
				int wasAdded = 0;
				if(structArr[firstIndex].start != 0){
					gapSize = structArr[firstIndex].start;
					if(inputThree <= gapSize){
						structArr[index].previousProcess = -1;
						structArr[index].nextProcess = firstIndex;
						structArr[firstIndex].previousProcess = index;
						structArr[index].start = 0;
						firstIndex = index;
						wasAdded = 1;
					}
				}

				if(wasAdded == 0){
					loopIndex = firstIndex;
					while(structArr[loopIndex].nextProcess != -1){
						gapSize = (structArr[structArr[loopIndex].nextProcess].start) - (structArr[loopIndex].start + structArr[loopIndex].size);
						if(gapSize > 0){
							if(inputThree <= gapSize){
								int tempIndex = structArr[loopIndex].nextProcess;
								structArr[structArr[loopIndex].nextProcess].previousProcess = index;
								structArr[loopIndex].nextProcess = index;

								structArr[index].previousProcess = loopIndex;
								structArr[index].nextProcess = tempIndex;
								structArr[index].start = (structArr[loopIndex].start + structArr[loopIndex].size);
								wasAdded = 1;
								break;
							}
						}
						loopIndex = structArr[loopIndex].nextProcess;
					}
				}
				if(wasAdded == 0){
					if(inputThree <= (memAllocation - (structArr[loopIndex].start + structArr[loopIndex].size))){
						structArr[loopIndex].nextProcess = index;
						structArr[index].previousProcess = loopIndex;
						structArr[index].nextProcess = -1;
						structArr[index].start = (structArr[loopIndex].start + structArr[loopIndex].size);
						wasAdded = 1;
					}
					else{
						printf("FAIL REQUEST %s %d\n", inputTwo, inputThree);
					}
				}
				if(wasAdded == 1){
					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
			}
			else if(strcmp(fitType, "NEXTFIT") == 0){
				//The starting index is the last process added
					//loop through the list backwards(i = (index - 1) -> 0) checking if id != "" and size != 0
					//if they are equal to those values, then we know that the index was removed, so keep looping
					//Exit the loop once a starting index is found

				//Once you find the most recent addition, loopIndex = startIndex
				//paste the code from the firstfit algorithm
				//after the check for a gap flollowing the last index, if wasAdded still == 0,
					//then loop through the array again from firstIndex until firstIndex == startIndex

				//If wasAdded still == 0, then no working gaps were found, print FAIL
				//Otherwise, allocate the data and print ALLOCATED


				int startIndex = index-1;
				int wasAdded = 0;
				int foundNext = 0;

				while(foundNext == 0 && startIndex >= 0){
					if(strcmp(structArr[startIndex].id, "") == 0 || structArr[startIndex].size == 0){
						startIndex--;
					}
					else{
						foundNext = 1;
					}
				}

				loopIndex = startIndex;
				while(structArr[loopIndex].nextProcess != -1){
					gapSize = (structArr[structArr[loopIndex].nextProcess].start) - (structArr[loopIndex].start + structArr[loopIndex].size);
					if(gapSize > 0){
						if(inputThree <= gapSize){
							int tempIndex = structArr[loopIndex].nextProcess;
							structArr[structArr[loopIndex].nextProcess].previousProcess = index;
							structArr[loopIndex].nextProcess = index;

							structArr[index].previousProcess = loopIndex;
							structArr[index].nextProcess = tempIndex;
							structArr[index].start = (structArr[loopIndex].start + structArr[loopIndex].size);
							wasAdded = 1;
							break;
						}
					}
					loopIndex = structArr[loopIndex].nextProcess;
				}

				if(wasAdded == 0){
					if(inputThree <= (memAllocation - (structArr[loopIndex].start + structArr[loopIndex].size))){
						structArr[loopIndex].nextProcess = index;
						structArr[index].previousProcess = loopIndex;
						structArr[index].nextProcess = -1;
						structArr[index].start = (structArr[loopIndex].start + structArr[loopIndex].size);
						wasAdded = 1;
					}
				}

				if(wasAdded == 0){
				if(structArr[firstIndex].start != 0){
					gapSize = structArr[firstIndex].start;
					if(inputThree <= gapSize){
						structArr[index].previousProcess = -1;
						structArr[index].nextProcess = firstIndex;
						structArr[firstIndex].previousProcess = index;
						structArr[index].start = 0;
						firstIndex = index;
						wasAdded = 1;
					}
				}
				}

				if(wasAdded == 0){
				loopIndex = firstIndex;
				while(structArr[loopIndex].nextProcess != -1 && loopIndex != startIndex){
					gapSize = (structArr[structArr[loopIndex].nextProcess].start) - (structArr[loopIndex].start + structArr[loopIndex].size);
					if(gapSize > 0){
						if(inputThree <= gapSize){
							int tempIndex = structArr[loopIndex].nextProcess;
							structArr[structArr[loopIndex].nextProcess].previousProcess = index;
							structArr[loopIndex].nextProcess = index;

							structArr[index].previousProcess = loopIndex;
							structArr[index].nextProcess = tempIndex;
							structArr[index].start = (structArr[loopIndex].start + structArr[loopIndex].size);
							wasAdded = 1;
							break;
						}
					}
					loopIndex = structArr[loopIndex].nextProcess;
				}
				}


				if(wasAdded == 0){
					printf("FAIL REQUEST %s %d\n", inputTwo, inputThree);
				}
				if(wasAdded == 1){
					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
			}
			else if(strcmp(fitType, "WORSTFIT") == 0){
				int wasAdded = 0;
				int largestWorkingGapSize = 0;
				int gapIndex = -2;

				//check for a gap before the first index
				if(structArr[firstIndex].start != 0){
					gapSize = structArr[firstIndex].start;
					if(inputThree <= gapSize){
						largestWorkingGapSize = gapSize;
						gapIndex = -1;
					}
				}
				//check the array for the largest working gap
				loopIndex = firstIndex;
				while(structArr[loopIndex].nextProcess != -1){
					gapSize = (structArr[structArr[loopIndex].nextProcess].start) - (structArr[loopIndex].start + structArr[loopIndex].size);
					if(gapSize > 0){
						if(inputThree <= gapSize && gapSize > largestWorkingGapSize){
							largestWorkingGapSize = gapSize;
							gapIndex = loopIndex;
						}
					}
					loopIndex = structArr[loopIndex].nextProcess;
				}
				//check if there is a gap after the last index
				gapSize = memAllocation - (structArr[loopIndex].start + structArr[loopIndex].size);
				if(gapSize > 0 && gapSize > largestWorkingGapSize && inputThree <= gapSize){
					largestWorkingGapSize = gapSize;
					gapIndex = loopIndex;
				}

				if(gapIndex == -2){
					//then no gap was found
					printf("FAIL REQUEST %s %d\n", inputTwo, inputThree);
				}
				else if(gapIndex == -1){
					//the gap comes before the first index
					structArr[index].previousProcess = -1;
					structArr[index].nextProcess = firstIndex;
					structArr[firstIndex].previousProcess = index;
					structArr[index].start = 0;
					firstIndex = index;

					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
				else if(structArr[gapIndex].nextProcess == -1){
					//the gap comes after the last index
					structArr[gapIndex].nextProcess = index;
					structArr[index].previousProcess = gapIndex;
					structArr[index].nextProcess = -1;
					structArr[index].start = (structArr[gapIndex].start + structArr[gapIndex].size);

					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
				else{
					//its a standard gap
					structArr[index].nextProcess = structArr[gapIndex].nextProcess;
					structArr[index].previousProcess = gapIndex;
					structArr[structArr[gapIndex].nextProcess].previousProcess = index;
					structArr[gapIndex].nextProcess = index;
					structArr[index].start = (structArr[gapIndex].start + structArr[gapIndex].size);

					strcpy(structArr[index].id, inputTwo);
					structArr[index].size = inputThree;
					printf("ALLOCATED %s %d\n", structArr[index].id, structArr[index].start);
					index++;
				}
			}

		}
		else if(strcmp(inputOne, "RELEASE") == 0){
			int wasRemoved = 0;
			loopIndex = firstIndex;
			while(loopIndex > -1 && wasRemoved == 0){
				if(strcmp(inputTwo, structArr[loopIndex].id) == 0){
					wasRemoved = 1;
					printf("FREE %s %d %d\n", structArr[loopIndex].id, structArr[loopIndex].size, structArr[loopIndex].start);
					if(structArr[loopIndex].previousProcess == -1 && structArr[loopIndex].nextProcess == -1){
						firstIndex = -1;
						strcpy(structArr[loopIndex].id, "");
					}
					else if(structArr[loopIndex].previousProcess == -1){
						firstIndex = structArr[loopIndex].nextProcess;
						structArr[structArr[loopIndex].nextProcess].previousProcess = -1;
						strcpy(structArr[loopIndex].id, "");
					}
					else if(structArr[loopIndex].nextProcess == -1){
						structArr[structArr[loopIndex].previousProcess].nextProcess = -1;
						strcpy(structArr[loopIndex].id, "");
					}
					else{
						structArr[structArr[loopIndex].nextProcess].previousProcess = structArr[loopIndex].previousProcess;
						structArr[structArr[loopIndex].previousProcess].nextProcess = structArr[loopIndex].nextProcess;
						strcpy(structArr[loopIndex].id, "");
					}
					structArr[loopIndex].size = 0;
					structArr[loopIndex].previousProcess = -1;
					structArr[loopIndex].nextProcess = -1;
				}
				loopIndex = structArr[loopIndex].nextProcess;
			}
			if(wasRemoved == 0){
				printf("FAIL RELEASE %s\n", inputTwo);
			}
		}
		else if(strcmp(inputOne, "LIST") == 0){
			if(strcmp(inputTwo, "AVAILABLE") == 0){
				if(firstIndex != -1){
					int isFull = 1;
					if(structArr[firstIndex].start != 0){
						isFull = 0;
						gapSize = structArr[firstIndex].start;
						printf("(%d, %d) ", gapSize, 0);
					}
					loopIndex = firstIndex;
					while(structArr[loopIndex].nextProcess != -1){
						gapSize = (structArr[structArr[loopIndex].nextProcess].start) - (structArr[loopIndex].start + structArr[loopIndex].size);
						if(gapSize > 0){
							isFull = 0;
							printf("(%d, %d) ", gapSize, structArr[loopIndex].start + structArr[loopIndex].size);
						}
						loopIndex = structArr[loopIndex].nextProcess;
					}
					if(structArr[loopIndex].nextProcess == -1){
						gapSize = (memAllocation - (structArr[loopIndex].start + structArr[loopIndex].size));
						if(gapSize > 0){
							isFull = 0;
							printf("(%d, %d)", gapSize, structArr[loopIndex].start + structArr[loopIndex].size);
						}
					}
					if(isFull == 1){
						printf("FULL\n");
					}
					else{
						printf("\n");
					}
				}
				else{
					printf("(%d, 0)\n", memAllocation);
				}
			}
			else if(strcmp(inputTwo, "ASSIGNED") == 0){
				if(firstIndex == -1){
					printf("NONE\n");
				}
				else{
					loopIndex = firstIndex;
					while(loopIndex != -1){
						printf("(%s, %d, %d) ", structArr[loopIndex].id, structArr[loopIndex].size, structArr[loopIndex].start);
						loopIndex = structArr[loopIndex].nextProcess;
					}
					printf("\n");
				}
			}
		}
		else if(strcmp(inputOne, "FIND") == 0){
			loopIndex = firstIndex;
			int found = 0;
			while(loopIndex != -1){
				if(strcmp(inputTwo, structArr[loopIndex].id) == 0){
					found = 1;
					printf("(%s, %d, %d)\n", structArr[loopIndex].id, structArr[loopIndex].size, structArr[loopIndex].start);
				}
				loopIndex = structArr[loopIndex].nextProcess;
			}
			if(found == 0){
				printf("FAULT\n");
			}
		}
	}

	//printProcessArray(firstIndex, structArr);

	fclose(fp);
	if(command){
		free(command);
	}
	free(structArr);
	return 0;
}
