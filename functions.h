#ifndef FUNCTIONS
#define FUNCTIONS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#define STRING_LENGTH 1024

const char* getField(char* line, int num){

	const char* token;
	char* copy = (char*) malloc(STRING_LENGTH*sizeof(char));
	strcpy(copy, line);
	int count = 0;
	for(token = strtok(copy, ","); token && *token; token = strtok(NULL, ",")){
		if(count == num){
			free(copy);
			return token;
		}
		count++;
	}
	free(copy);
	return NULL;
}

void swap(char** string1, char** string2){
	char* temp = *string1;
	*string1 = *string2;
	*string2 = temp;
}

void insertionSort(char** array, int size){

	for(int i = 1; i < size; i++){
		int j = i -1;
		double key = atof(getField(array[i],1));
		//printf("%f\n", key);
		char* temp = (char*)malloc(STRING_LENGTH*sizeof(char));
		strcpy(temp, array[i]);
		while(j >= 0 && atof(getField(array[j],1)) > key){
			// strcpy(array[j+1], array[j]);
			swap(&array[j+1], &array[j]);
			j--;
		}

		strcpy(array[j+1], temp);

		free(temp);
	}
}

char** allocateArray(FILE* stream, int size, int skipLine){
	rewind(stream);
	int index = 0;
	char** array = (char**) malloc(size*sizeof(char*));
	for(int i = 0; i < size; i++){
		array[i] = (char*)malloc(STRING_LENGTH*sizeof(char));
	}

	char line[STRING_LENGTH];
	if(skipLine == 1) fgets(line, STRING_LENGTH, stream);//get rid of first line of file
	while(fgets(line, STRING_LENGTH, stream)){
		char* temp = strdup(line);
		strcpy(array[index++], temp);
		free(temp);
		
	}
	

	return array;
}

char** allocateArrayNoFile(int size){
	char** array = (char**) malloc(size*sizeof(char*));
	for(int i = 0; i < size; i++){
		array[i] = (char*)malloc(STRING_LENGTH*sizeof(char));
	}
	return array;
}

void freeArray(char** array, int size){
	for(int i = 0; i < size; i++){
		free(array[i]);
	}
}

int countLines(FILE* stream, int skipLine){
	rewind(stream);
	int count = 0;
	char line[STRING_LENGTH];
	if(skipLine == 1)fgets(line, STRING_LENGTH, stream);
	for(char c = getc(stream); c != EOF; c = getc(stream)){
		if(c == '\n') count++;
	}

	return count;
}


void writeToFile(FILE* file, char** array, int size){
	rewind(file);
	for(int i = 0; i < size; i++){
		fprintf(file, "%s", array[i]);
	}
}

void merge(char** array, FILE* file1, FILE* file2, int size1, int size2){
	char** array1 = allocateArray(file1, size1, 0);
	char** array2 = allocateArray(file2, size2, 0);
	int i = 0;
	int j = 0;
	int index = 0;

	while(i < size1 && j < size2){
		double key1 = atof(getField(array1[i], 1));
		double key2 = atof(getField(array2[j], 1));
		if(key1 <= key2){
			strcpy(array[index++], array1[i++]);
		}else strcpy(array[index++], array[j++]);
	}

	while(i < size1){
		strcpy(array[index++], array1[i++]);
	}

	while(j < size2){
		strcpy(array[index++], array2[j++]);
	}

	freeArray(array1, size1);
	freeArray(array2, size2);
	free(array1);
	free(array2);
}

void splitFile(int count, int partitions){
	char cmd[100];
	char buffer[20];
	strcpy(cmd, "split all_month.csv -l ");
	sprintf(buffer, "%d", (count/partitions)+1);
	strcat(cmd, buffer);
	system(cmd);
}

void oneProcessSort(){
	clock_t time = clock();
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int count = countLines(stream, 1);
	char** array = allocateArray(stream, count, 1);
	insertionSort(array, count);
	for(int i = 0; i < count ;i++){
		printf("%s", array[i]);
	}

	
	freeArray(array, count);
	free(array);
	fclose(stream);
	time = clock() - time;
	printf("Sorting with one process ran in %f seconds\n", (double)time/CLOCKS_PER_SEC);

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

	int count = countLines(stream, 1);
	splitFile(count,processCount);
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
			
		}else{
			wait(NULL);
		}
	}
	FILE* file1 = fopen("file1", "r+");
	if(file1 == NULL){
		perror("Failed to open file");
		exit(1);
	}
	merge(array, file1, xab, size1, size2);
	for(int i = 0; i < count; i++){
		printf("%s", array[i] );
	}
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
				char** array2 = allocateArray(xab, size2, 0);
				insertionSort(array2, size2);
				writeToFile(xab, array2, size2);
				freeArray(array2, size2);
				free(array2);				
			}
			if(id == 3){
				char** array2 = allocateArray(xab, size2, 0);
				insertionSort(array2, size2);
				writeToFile(xab, array2, size2);
				freeArray(array2, size2);
				free(array2);				
			}
			exit(0);
			
		}else{
			wait(NULL);
		}
	}


	fclose(xaa);
	fclose(xab);
	fclose(xac);
	fclose(xad);
	fclose(stream);
	time = clock() - time;
	printf("Sorting with %d processes ran in %f seconds\n", processCount, (double)time/CLOCKS_PER_SEC);
}


#endif //functions