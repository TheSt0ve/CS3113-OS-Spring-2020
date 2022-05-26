//Steven Johnson
//CS3113 Spring 2021

#include<stdio.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>


//A struct that holds the vital information pertaining to an instruction
struct Instruction{
	int Ipid;
	int IburstTime;
	int Ipriority;

	int IstartTime;
	int IwaitTime;
	int ItotalBurstTime;

	int PstartTime;
};

//Print out each of the instructions along with some handy information about them
//Used for debugging purposes
void printAllInstructions(struct Instruction* arr, int size){
	for(int i = 0; i < size; i++){
		fprintf(stderr, "%d %d %d - ", arr[i].Ipid, arr[i].IburstTime, arr[i].Ipriority);
		fprintf(stderr, "Start time: %d, Wait time: %d, Total Burst: %d\n", arr[i].IstartTime, arr[i].IwaitTime, arr[i].ItotalBurstTime);
	}
}

//Print out the last occurrence of each unique instruction along with some handy information about them
//Used for debugging purposes
void printUniqueInstructions(struct Instruction* arr, int* terminatedIndices, int size, int uniqueCount){
	for(int i = 0; i < uniqueCount; i++){
		fprintf(stderr, "%d - ", arr[terminatedIndices[i]].Ipid);
		fprintf(stderr, "Initial Start Time: %d\tTotal Wait Time: %d\tTotal Burst Time: %d\n", arr[terminatedIndices[i]].PstartTime, arr[terminatedIndices[i]].IwaitTime, arr[terminatedIndices[i]].ItotalBurstTime);
		if(i > 30){return;}
	}
}

//Checks if a given PID is already in the list
//Returns -1 if not found, otherwise return the index of the first occurrence of the PID
int findFirstOccurrence(struct Instruction* arr, int pid, int size){
	for(int i = 0; i < size; i++){
		if(arr[i].Ipid == pid){
			return i;
		}
	}
	return -1;
}

//Checks to see if the PID is repeated after the specified occurrence
//Used for incrementing the number of involuntary switches
//Returns -1 if it is not repeated, otherwise return the index of the PIDs next occurence
int findNextOccurrence(struct Instruction* arr, int size, int current){
	for(int i = current; i < size-1; i++){
		if(arr[i+1].Ipid == arr[current].Ipid){
			return i+1;
		}
	}
	return -1;
}

//Checks if a given PID is repeated before the specified occurrence
//Used for properly keeping track of the wait time and total burst time of each process
//Returns -1 if not found, otherwise return the index of the PIDs previous occurrence
int findPreviousOccurrence(struct Instruction* arr, int current){
	for(int i = current; i > 0; i--){
		if(arr[current].Ipid == arr[i-1].Ipid){
			return i-1;
		}
	}
	return -1;
}


//Finds the termination indices of all unique processes in the array
//Returns an array of indices for the last occurrence each unique PID is found at
int* findTerminatedIndices(struct Instruction* arr, int size, int uniqueCount){
	int numFound = 0;
	int uniquePIDs[uniqueCount];
	int* matchingIndices = calloc(uniqueCount, sizeof(int));

	//Loop from the end of the array to ensure the last index of each unique PID is the one recorded
	int j;
	int breakCon;
	for(int i = size-1; i >= 0; i--){
		j = 0;
		breakCon = 0;
		while(j < numFound && breakCon == 0){
			//If the PID is found, then it isn't unique, so break out of the while loop
			if(arr[i].Ipid == uniquePIDs[j]){
				breakCon = 1;
			}
			else{
				j++;
			}
		}
		//If the PID is unique, add it to the uniquePIDs array for future comparisons
		//Lastly, add its index to the matchingIndices array
		if(breakCon == 0){
			uniquePIDs[numFound] = arr[i].Ipid;
			matchingIndices[((uniqueCount - 1) - numFound)] = i;
			numFound++;
		}
	}

	return matchingIndices;
}

//Calculate the throughput
double calcThroughput(int totalThreadCount, int totalBurstTime){
	return ((double)totalThreadCount / totalBurstTime);
}

//Calculate average turnaround time within the array
double calcAveTurnaroundTime(struct Instruction* arr, int* terminatedIndices, int size, int uniqueCount){
	int sumWaitBurst = 0;
	for(int i = 0; i < uniqueCount; i++){
		sumWaitBurst += arr[terminatedIndices[i]].IwaitTime;
		sumWaitBurst += arr[terminatedIndices[i]].ItotalBurstTime;
	}
	return ((double)sumWaitBurst / uniqueCount);
}

//Calculate average waiting time within the array
double calcAveWaitingTime(struct Instruction* arr, int* terminatedIndices, int size, int uniqueCount){
	int sumWaitTime = 0;
	for(int i = 0; i < uniqueCount; i++){
		sumWaitTime += arr[terminatedIndices[i]].IwaitTime;
	}
	return ((double)sumWaitTime / uniqueCount);
}

//Calculate average response time within the array
double calcAveResponseTime(struct Instruction* arr, int* terminatedIndices, int size, int uniqueCount){
	int sumResponseTime = 0;
	int firstIndices[uniqueCount];
	int firstIndex;
	for(int i = 0; i < uniqueCount; i++){
		firstIndex = findFirstOccurrence(arr, arr[terminatedIndices[i]].Ipid, size);
		sumResponseTime += arr[firstIndex].IwaitTime;
	}
	return ((double)sumResponseTime / uniqueCount);
}




int main(int argc, char** argv){
	//If the user types a file name in as a command, create a file pointer to read input from that file
	//Otherwise, set the file pointer equal to stdin for a simpler implementation
	char filename[35];
	FILE* fp;
	if(argc > 1){
		if(argc > 2){
			perror("Too many arguments passed in");
			exit(-1);
		}
		else{
			strcpy(filename, argv[1]);
			fp = fopen(filename, "r");
			if(fp == NULL){
				perror("Unable to open input file");
				exit(-1);
			}
		}
	}
	else{
		fp = stdin;
	}


	int numOfProcesses;	//P = 1
	int numOfThreads;	//1 <= p <= 32768 (but in this project p <= 32)
	int numOfInstructions;	//1 <= N <= 32768


	int numOfVolSwitches = 0;	//Always <= N, increment when a process with a unique PID terminates

	int numOfInvolSwitches = 0;	//Always <= N, increment if the same PID pops up again non-consecutively,
					//i.e. if the process was interrupted rather than terminated

	double useOfCPU = 100;		//Percentage of the total time that a processor has been utilized
					//Only using one processor for this project so this value is always 100.00

	double throughput;		//(numOfThreads) / (totalBurstTime)

	double aveTurnaroundTime;	//The average time it takes a process from submission to completion
					//(wait time of uniques + execution time of uniques)/(# of unique PIDs)

	double aveWaitTime;		//The average time that elapses before a process starts executing
					//(wait time of uniques)/(# of uniques)

	double aveResponseTime;		//The average time before a process is first mentioned
					//If the same process is run more than once, response time is unaffected


	int size = 0;
	int numOfUniques = 0;
	int totalTime = 0;
	struct Instruction* structArr;

	fscanf(fp, "%d", &numOfProcesses);
	fscanf(fp, "%d %d", &numOfThreads, &numOfInstructions);
	structArr = calloc(numOfInstructions, sizeof(struct Instruction));


	//This while loop adds all of the Instructions into structArr
	//Keeps track of the array size, total time elapsed, and the # of unique PIDs
	//Also updates each process' wait time and total burst time as they are added
	while((fscanf(fp, "%d", &structArr[size].Ipid) != EOF) && (size < numOfInstructions)){
		fscanf(fp, "%d %d", &structArr[size].IburstTime, &structArr[size].Ipriority);
		structArr[size].IstartTime = totalTime;
		int foundIndex = findFirstOccurrence(structArr, structArr[size].Ipid, size);

		//At this point in the loop, the PID is unique
		if(foundIndex == -1){
			numOfUniques++;

			//Since its a unique PID, the start time and wait time are the same
			//Because its the first time the process is run, the process start time = instuction start time
			structArr[size].IwaitTime = structArr[size].IstartTime;
			structArr[size].ItotalBurstTime = structArr[size].IburstTime;
			structArr[size].PstartTime = totalTime;
		}
		//Not a unique PID, but also not a consecutive duplicate
		else if(foundIndex != (size-1)){
			//Subtract the PIDs previous burst time from this occurrence's start time to get the proper wait time
			//Because its a duplicate, the process start time is the same as all of the other occurences of it
			int prevIndex = findPreviousOccurrence(structArr, size);
			structArr[size].IwaitTime = structArr[size].IstartTime - structArr[prevIndex].ItotalBurstTime;
			structArr[size].ItotalBurstTime = structArr[size].IburstTime + structArr[prevIndex].ItotalBurstTime;
			structArr[size].PstartTime = structArr[prevIndex].PstartTime;
		}
		//Consecutive instructions with the same PID, no switch occurred
		else{
			//The current process' wait time is the same as the previous occurrence's
			//Because its a duplicate, the process start time is the same as all of the other occurences of it
			int prevIndex = findPreviousOccurrence(structArr, size);
			structArr[size].IwaitTime = structArr[prevIndex].IwaitTime;
			structArr[size].ItotalBurstTime = structArr[size].IburstTime + structArr[prevIndex].ItotalBurstTime;
			structArr[size].PstartTime = structArr[prevIndex].PstartTime;
		}
		totalTime += structArr[size].IburstTime;
		size++;
	}

	//Each unique process has exactly one termination point, therefore numOfVolSwitches = numOfUniques
	numOfVolSwitches = numOfUniques;

	//Use findNextOccurrence to increment numOfInvolSwitches, make sure to check for consecutive duplicates
	for(int i = 0; i < size; i++){
		int next = findNextOccurrence(structArr, size, i);
		if(next != -1 && next != i+1){
			numOfInvolSwitches++;
		}
	}

	int* termInd = findTerminatedIndices(structArr, size, numOfUniques);

	fprintf(stderr, "Total time elapsed: %d\n", totalTime);
	//printAllInstructions(structArr, size);
	printUniqueInstructions(structArr, termInd, size, numOfUniques);


	//Output Here
	printf("%d\n", numOfVolSwitches);

	printf("%d\n", numOfInvolSwitches);

	printf("%0.2f\n", useOfCPU);

	throughput = calcThroughput(numOfThreads, totalTime);
	printf("%0.2f\n", throughput);

	aveTurnaroundTime = calcAveTurnaroundTime(structArr, termInd, size, numOfUniques);
	printf("%0.2f\n", aveTurnaroundTime);

	aveWaitTime = calcAveWaitingTime(structArr, termInd, size, numOfUniques);
	printf("%0.2f\n", aveWaitTime);

	aveResponseTime = calcAveResponseTime(structArr, termInd, size, numOfUniques);
	printf("%0.2f\n", aveResponseTime);


	fclose(fp);
	free(structArr);
	free(termInd);
	return 0;
}

