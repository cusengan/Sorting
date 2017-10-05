#include "functions.h"

int main(int argc, char *argv[]){

	// printf("%s\n", argv[1]);
	FILE* file = fopen(argv[1], "r+");
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