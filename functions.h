#ifndef FUNCTIONS
#define FUNCTIONS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#define STRING_LENGTH 1024

const char* getField(char* line, int num){

	const char* token;
	char* copy = (char*) malloc(STRING_LENGTH*sizeof(char));
	strcpy(copy, line);
	int count = 0;
	for(token = strtok(copy, ","); token && *token; token = strtok(NULL, ",")){
		if(count == num) return token;
		count++;
	}
	free(copy);
	return NULL;
}

void insertionSort(char** array, int size){

	for(int i = 1; i < size; i++){
		int j = i -1;
		double key = atof(getField(array[i],1));
		printf("%f\n", key);
		char* temp = (char*)malloc(STRING_LENGTH*sizeof(char));
		strcpy(temp, array[i]);
		while(j >= 0 && atof(getField(array[j],1)) > key){
		// 	strcpy(array[j+1], array[j]);
		// 	j--;
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
	// for(int i = 0; i < count ;i++){
	// 	printf("%s\n", array[i]);
	// }

	
	freeArray(array, count);
	free(array);
	fclose(stream);
	time = clock() - time;
	printf("Sorting with one process ran in %f seconds\n", (double)time/CLOCKS_PER_SEC);

}


#endif //functions