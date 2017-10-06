#ifndef THREADS
#define THREADS
#include "functions.h"

struct mergeStruct{
	char* argv[10];
	int id;
};

void* sort(void* stream){

	FILE* file = fopen((char*)stream, "r+");
	if(file == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int size = countLines(file, 0);
	char** array = allocateArray(file, size, 0);
	insertionSort(array, size);
	writeToFile(file, array, size);
	freeArray(array, size);
	free(array);
	fclose(file);
}

void oneThreadSort(){
	clock_t time = clock();
	int threadCount = 1;
	FILE* stream = fopen("all_month.csv", "r");
	FILE* sortedFile = fopen("oneThreadSorted", "w");
	if(stream == NULL || sortedFile == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int count = countLines(stream, 1);
	char** array = allocateArray(stream, count, 1);
	insertionSort(array, count);
	// printArray(array, count);	
	writeToFile(sortedFile, array, count);					
	freeArray(array, count);
	free(array);
	fclose(stream);
	fclose(sortedFile);
	time = clock() - time;
	printTimeThreads(threadCount, time);
}

void twoThreadSort(){
	clock_t time = clock();
	int threadCount = 2;
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int count = countLines(stream, 1);
	splitFile(count, threadCount);
	FILE* xaa = fopen("xaa", "r+");
	FILE* xab = fopen("xab", "r+");
	FILE* file1 = fopen("file1", "w"); //creating new file to get rid of first line of xaa
	FILE* sortedFile = fopen("twoProcessSorted", "w");
	if(xaa == NULL || xab == NULL || file1 == NULL || sortedFile == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int size1 = countLines(xaa, 1);
	int size2 = countLines(xab, 0);
	char** array = allocateArrayNoFile(count);
	char** arrayFile = allocateArray(xaa, size1, 1);
	writeToFile(file1, arrayFile, size1);//writing data to file1
	freeArray(arrayFile, size1);
	free(arrayFile);
	fclose(file1);



	pthread_t tid[threadCount];
	char* fileNames[2] = {"file1", "xab"};

	for(int i = 0; i < threadCount; i++){
		pthread_create(&tid[i], NULL, sort, (void*)fileNames[i]);
	}

	for(int i = 0; i < threadCount; i++){
		pthread_join(tid[i], NULL);
	}


	file1 = fopen("file1", "r+");
	if(file1 == NULL){
		perror("Failed to open file");
		exit(1);
	}
	merge(array, file1, xab, size1, size2);
	writeToFile(sortedFile, array, count);
	fclose(xaa);
	fclose(xab);
	fclose(file1);
	fclose(sortedFile);
	fclose(stream);
	freeArray(array, count);
	free(array);
	system("rm xaa; rm xab; rm file1");
	time = clock() - time;
	printTimeThreads(threadCount, time);
}

#endif //THREADS