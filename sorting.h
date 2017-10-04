#ifndef SORTING
#define SORTING
#include "functions.h"


void oneProcessSort(){
	clock_t time = clock();
	int processCount = 1;
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int count = countLines(stream, 1);
	char** array = allocateArray(stream, count, 1);
	insertionSort(array, count);
	// printArray(array, count);	
	freeArray(array, count);
	free(array);
	fclose(stream);
	time = clock() - time;
	printf("Sorting with %d process ran in %f seconds\n", processCount, (double)time/CLOCKS_PER_SEC);

}

void twoProcessSort(){
	clock_t time = clock();
	int processCount = 2;
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}
	pid_t child;
	pid_t child2;

	int count = countLines(stream, 1);
	char cmd[100];
	char buffer[20];
	strcpy(cmd, "split all_month.csv -l ");
	sprintf(buffer, "%d", (count/2) + 1);
	strcat(cmd, buffer);
	system(cmd);
	FILE* xaa = fopen("xaa", "r+");
	FILE* xab = fopen("xab", "r+");
	if(xaa == NULL || xab == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int size1 = countLines(xaa, 1);
	int size2 = countLines(xab, 0);
	char** array = allocateArrayNoFile(count);

	for(int id = 0; id < processCount; id++){
		child = fork();
		if(child== 0){//in child{
			if(id == 0){
				char** array1 = allocateArray(xaa, size1, 1);
				insertionSort(array1, size1);
				FILE* file1 = fopen("file1", "w");
				if(file1 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				writeToFile(file1, array1, size1);
				freeArray(array1, size1);
				free(array1);
				fclose(file1);
			}

			
			if(id == 1){
				char** array2 = allocateArray(xab, size2, 0);
				insertionSort(array2, size2);
				writeToFile(xab, array2, size2);
				freeArray(array2, size2);
				free(array2);				
			}
			exit(0);
			
		}else if(child < 0){
			perror("fork failed\n");
			exit(1);
		}else{
			continue;
		}
	}
	if(child > 0){//wait for both child processes to finish
		wait(NULL);
		wait(NULL);
	}

	FILE* file1 = fopen("file1", "r+");
	if(file1 == NULL){
		perror("Failed to open file");
		exit(1);
	}
	merge(array, file1, xab, size1, size2);
	// printArray(array, count);
	freeArray(array,count);
	free(array);
	fclose(xaa);
	fclose(xab);
	fclose(file1);
	system("rm xaa; rm xab; rm file1");
	fclose(stream);
	time = clock() - time;
	printf("Sorting with %d processes ran in %f seconds\n", processCount, (double)time/CLOCKS_PER_SEC);
}

void fourProcessSort(){
	clock_t time = clock();
	pid_t child;
	int processCount = 4;
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int count = countLines(stream, 1);
	splitFile(count, processCount);
	FILE* xaa = fopen("xaa", "r+");
	FILE* xab = fopen("xab", "r+");
	FILE* xac = fopen("xac", "r+");
	FILE* xad = fopen("xad", "r+");
	if(xaa == NULL || xab == NULL || xac == NULL || xad == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int size1 = countLines(xaa, 1);
	int size2 = countLines(xab, 0);
	int size3 = countLines(xac, 0);
	int size4 = countLines(xad, 0);
	char** array = allocateArrayNoFile(count);
	for(int id = 0; id < processCount; id++){
		child = fork();
		if(child== 0){//in child{
			if(id == 0){
				char** array1 = allocateArray(xaa, size1, 1);
				insertionSort(array1, size1);
				FILE* file1 = fopen("file1", "w");
				if(file1 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				writeToFile(file1, array1, size1);
				freeArray(array1, size1);
				free(array1);
				fclose(file1);
			}

			if(id == 1){
				char** array2 = allocateArray(xab, size2, 0);
				insertionSort(array2, size2);
				writeToFile(xab, array2, size2);
				freeArray(array2, size2);
				free(array2);				
			}
			if(id == 2){
				char** array3 = allocateArray(xac, size3, 0);
				insertionSort(array3, size3);
				writeToFile(xac, array3, size3);
				freeArray(array3, size3);
				free(array3);				
			}
			if(id == 3){
				char** array4 = allocateArray(xad, size4, 0);
				insertionSort(array4, size4);
				writeToFile(xad, array4, size4);
				freeArray(array4, size4);
				free(array4);				
			}
			exit(0);
			
		}else if(child < 0){
			perror("fork failed\n");
			exit(1);
		}else{
			continue;
		}

	}//end for loop

	if(child > 0){ //wait for 4 processes to finish
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
	}

	for(int id = 0; id < processCount/2; id++){//use two processes to merge files in pairs
		child = fork();
		if(child== 0){//in child{
			if(id == 0){
				char** arrayMerge = allocateArrayNoFile(size1 + size2);
				FILE* file1 = fopen("file1", "r+");
				if(file1 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				FILE* merge1 = fopen("merge1", "w+");
				if(merge1 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				merge(arrayMerge, file1, xab, size1, size2);
				// printArray(arrayMerge, size1+size2);
				writeToFile(merge1, arrayMerge, size1+size2);
				freeArray(arrayMerge, size1+size2);
				free(arrayMerge);
				fclose(file1);
				fclose(merge1);
			}

			if(id == 1){
				char** arrayMerge = allocateArrayNoFile(size3 + size4);
				merge(arrayMerge, xac, xad, size3, size4);
				FILE* merge2 = fopen("merge2", "w+");
				if(merge2 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				writeToFile(merge2, arrayMerge, size3 + size4);
				freeArray(arrayMerge, size3+size4);
				free(arrayMerge);
				fclose(merge2);					
			
			}

			exit(0);
		}
		else if(child < 0){
			perror("fork failed\n");
			exit(1);
		}else{
			continue;
		}
	}

	if(child > 0){
		wait(NULL);
		wait(NULL);
	}

	FILE* merge1 = fopen("merge1", "r+");
	FILE* merge2 = fopen("merge2", "r+");
	if(merge1 == NULL || merge2 == NULL){
		perror("Failed to open file");
		exit(1);
	}

	merge(array, merge1, merge2, size1+size2, size3+size4);
	// printArray(array, count);
	time = clock() - time;
	printf("Sorting with %d processes ran in %f seconds\n", processCount, (double)time/CLOCKS_PER_SEC);
	fclose(xaa);
	fclose(xab);
	fclose(xac);
	fclose(xad);
	fclose(stream);
	freeArray(array, count);
	free(array);
	system("rm xaa xab xac xad file1 merge1 merge2");
	
}

void tenProcessSort(){

}




#endif //SORTING