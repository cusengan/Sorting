#include "functions.h"

int main(int argc, char *argv[]){
	
	printf("%s-%s-%s\n", argv[1], argv[2], argv[3] );
	FILE* file1 = fopen(argv[1], "r+");
	FILE* file2 = fopen(argv[2], "r+");
	FILE* mergeFile = fopen(argv[3], "w+");
	if(file1 == NULL || file2 == NULL){ 
		perror("Failed to open file");
		exit(1);
	}
	int size1 = countLines(file1, 0);
	int size2 = countLines(file2, 0);
	char** arrayMerge = allocateArrayNoFile(size1+size2);
	merge(arrayMerge, file1, file2, size1, size2);
	writeToFile(mergeFile, arrayMerge, size1 + size2);
	freeArray(arrayMerge, size1+size2);
	free(arrayMerge);
	fclose(file1);
	fclose(file2);
	fclose(mergeFile);
}