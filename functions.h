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
		printf("%f\n", key);
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

char** allocateArray(FILE* stream, int count){
	rewind(stream);
	int index = 0;
	char** array = (char**) malloc(count*sizeof(char*));
	for(int i = 0; i < count; i++){
		array[i] = (char*)malloc(STRING_LENGTH*sizeof(char));
	}

	char line[STRING_LENGTH];
	fgets(line, STRING_LENGTH, stream);//get rid of first line of file
	while(fgets(line, STRING_LENGTH, stream)){
		char* temp = strdup(line);
		strcpy(array[index++], temp);
		free(temp);
		
	}
	

	return array;
}

void freeArray(char** array, int size){
	for(int i = 0; i < size; i++){
		free(array[i]);
	}
}

int countLines(FILE* stream){
	rewind(stream);
	int count = 0;
	char line[STRING_LENGTH];
	fgets(line, STRING_LENGTH, stream);
	for(char c = getc(stream); c != EOF; c = getc(stream)){
		if(c == '\n') count++;
	}

	return count;
}


void oneProcessSort(){
	clock_t time = clock();
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int count = countLines(stream);
	char** array = allocateArray(stream, count);
	insertionSort(array, count);
	for(int i = 0; i < count ;i++){
		printf("%s\n", array[i]);
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
	pid_t child;
	pid_t child2;

	int count = countLines(stream);
	char cmd[100];
	char buffer[20];
	strcpy(cmd, "split all_month.csv -l ");
	sprintf(buffer, "%d", (count/2) + 1);
	strcat(cmd, buffer);
	system(cmd);
	FILE* xaa = fopen("xaa", "r");
	FILE* xab = fopen("xab", "r");
	int size1 = countLines(xaa);
	int size2 = countLines(xab);

	char** array1 = allocateArray();

	for(int id = 0; id < processCount; id++){
		child = fork();
		if(child== 0){//in child{
			printf("child %d\n", id);
			fd
			

			exit(0);
		}else{
			wait(NULL);
		}
	}

	fclose(xaa);
	fclose(xab);
	fclose(stream);
}


#endif //functions