#ifndef SORTING
#define SORTING
#include "functions.h"


void oneProcessSort(){
	clock_t time = clock();
	int processCount = 1;
	FILE* stream = fopen("all_month.csv", "r");
	FILE* sortedFile = fopen("oneProcessSorted", "w");
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
	printTime(processCount, time);

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
	splitFile(count, processCount);
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

	char* fileNames[2] = {"file1", "xab"};

	for(int id = 0; id < processCount; id++){ //sorting files
		child = fork();
		if(child== 0){//in child{
			char* argv[3];
			argv[0] = "./sorting";
			argv[1] = fileNames[id];
			argv[2] = NULL;

			execvp(argv[0], argv);

			
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

	file1 = fopen("file1", "r+");
	if(file1 == NULL){
		perror("Failed to open file");
		exit(1);
	}
	merge(array, file1, xab, size1, size2);
	// printArray(array, count);
	writeToFile(sortedFile, array, count);
	freeArray(array,count);
	free(array);
	fclose(xaa);
	fclose(xab);
	fclose(file1);
	fclose(sortedFile);
	system("rm xaa; rm xab; rm file1");
	fclose(stream);
	time = clock() - time;
	printTime(processCount, time);
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
	FILE* file1 = fopen("file1", "w"); //creating new file to get rid of first line of xaa
	FILE* sortedFile = fopen("fourProcessSorted", "w");
	if(xaa == NULL || xab == NULL || xac == NULL || xad == NULL || file1 == NULL || sortedFile == NULL){
		perror("Failed to open file");
		exit(1);
	}


	int size1 = countLines(xaa, 1);
	int size2 = countLines(xab, 0);
	int size3 = countLines(xac, 0);
	int size4 = countLines(xad, 0);
	char** array = allocateArrayNoFile(count);

	char** arrayFile = allocateArray(xaa, size1, 1);
	writeToFile(file1, arrayFile, size1);//writing data to file1
	freeArray(arrayFile, size1);
	free(arrayFile);
	fclose(file1);

	char* fileNames[4] = {"file1", "xab", "xac", "xad"};
	char* mergeFileNames[2] = {"merge1", "merge2"};

	for(int id = 0; id < processCount; id++){
		child = fork();
		if(child== 0){//in child{
			char* argv[3];
			argv[0] = "./sorting";
			argv[1] = fileNames[id];
			argv[2] = NULL;

			execvp(argv[0], argv);

			
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
			char* argv[5];
			argv[0] = "./merge";
			argv[1] = fileNames[id*2];
			argv[2] = fileNames[id*2+1];
			argv[3] = mergeFileNames[id];
			argv[4] = NULL;

			execvp(argv[0], argv);

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
	writeToFile(sortedFile, array, count);
	time = clock() - time;
	printTime(processCount, time);
	fclose(xaa);
	fclose(xab);
	fclose(xac);
	fclose(xad);
	fclose(stream);
	freeArray(array, count);
	free(array);
	fclose(sortedFile);
	system("rm xaa xab xac xad file1 merge1 merge2");
	
}//end 4 process sorting

void tenProcessSort(){
	clock_t time = clock();
	pid_t child;
	int processCount = 10;
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
	FILE* xae = fopen("xae", "r+");
	FILE* xaf = fopen("xaf", "r+");
	FILE* xag = fopen("xag", "r+");
	FILE* xah = fopen("xah", "r+");
	FILE* xai = fopen("xai", "r+");
	FILE* xaj = fopen("xaj", "r+");
	FILE* file1 = fopen("file1", "w"); //creating new file to get rid of first line of xaa
	FILE* sortedFile = fopen("tenProcessSorted", "w");
	if(xaa == NULL || xab == NULL || xac == NULL || xad == NULL|| xae == NULL || xaf == NULL || xag == NULL || xah == NULL || xai == NULL || xaj == NULL || file1 == NULL || sortedFile == NULL){
		perror("Failed to open file");
		exit(1);
	}
	int size1 = countLines(xaa, 1);
	int size2 = countLines(xab, 0);
	int size3 = countLines(xac, 0);
	int size4 = countLines(xad, 0);
	int size5 = countLines(xae, 0);
	int size6 = countLines(xaf, 0);
	int size7 = countLines(xag, 0);
	int size8 = countLines(xah, 0);
	int size9 = countLines(xai, 0);
	int size10 = countLines(xaj, 0);
	char** array = allocateArrayNoFile(count);

	char** arrayFile = allocateArray(xaa, size1, 1);
	writeToFile(file1, arrayFile, size1);//writing data to file1
	freeArray(arrayFile, size1);
	free(arrayFile);
	fclose(file1);

	char* fileNames[10] = {"file1", "xab", "xac", "xad", "xae", "xaf", "xag", "xah", "xai", "xaj"};
	char* mergeFileNames[5] = {"merge1", "merge2", "merge3", "merge4", "merge5"};
	char* mergeLargeFileNames[2] = {"mergeLarge1", "mergeLarge2"};

	for(int id = 0; id < processCount; id++){//sort files
		child = fork();
		if(child== 0){//in child{
			char* argv[3];
			argv[0] = "./sorting";
			argv[1] = fileNames[id];
			argv[2] = NULL;

			execvp(argv[0], argv);

			
			exit(0);

			exit(0);
			
		}else if(child < 0){
			perror("fork failed\n");
			exit(1);
		}else{
			continue;
		}

	}//end for loop

	if(child > 0){ //wait for 10 processes to finish
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
	}

	for(int id = 0; id < processCount/2; id++){//use 5 processes to merge files in pairs
		child = fork();
		if(child== 0){//in child{
			char* argv[5];
			argv[0] = "./merge";
			argv[1] = fileNames[id*2];
			argv[2] = fileNames[id*2+1];
			argv[3] = mergeFileNames[id];
			argv[4] = NULL;

			execvp(argv[0], argv);

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
		wait(NULL);
		wait(NULL);
		wait(NULL);
	}

	for(int id = 0; id < 2; id++){//use 2 processes to merge merge1/merge2 and merge3/merge4
		child = fork();
		if(child== 0){//in child{
			
			char* argv[5];
			argv[0] = "./merge";
			argv[1] = mergeFileNames[id*2];
			argv[2] = mergeFileNames[id*2+1];
			argv[3] = mergeLargeFileNames[id];
			argv[4] = NULL;

			execvp(argv[0], argv);

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

	FILE* mergeLarge1 = fopen("mergeLarge1", "r+");
	FILE* mergeLarge2 = fopen("mergeLarge2", "r+");
	FILE* mergeLarge3 = fopen("mergeLarge3", "w+");
	FILE* merge5 = fopen("merge5", "r+");
	int firstHalf = size1+size2+size3+size4;
	int secondHalf = size5+size6+size7+size8;
	merge(array, mergeLarge1, mergeLarge2, firstHalf, secondHalf);
	writeToFile(mergeLarge3, array, firstHalf+secondHalf);
	fclose(mergeLarge3);
	mergeLarge3 = fopen("mergeLarge3", "r+");
	merge(array, mergeLarge3, merge5, firstHalf+secondHalf, size9+size10);
	// printArray(array, count);
	writeToFile(sortedFile, array, count);
	time = clock() - time;
	printTime(processCount, time);

	fclose(xaa);
	fclose(xab);
	fclose(xac);
	fclose(xad);
	fclose(xae);
	fclose(xaf);
	fclose(xag);
	fclose(xah);
	fclose(xai);
	fclose(xaj);
	fclose(mergeLarge1);
	fclose(mergeLarge2);
	fclose(mergeLarge3);
	fclose(merge5);
	fclose(stream);
	fclose(sortedFile);
	freeArray(array, count);
	free(array);
	system("rm xaa xab xac xad xae xaf xag xah xai xaj file1 merge1 merge2 merge3 merge4 merge5 mergeLarge1 mergeLarge2 mergeLarge3");
}




#endif //SORTING