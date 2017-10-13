#ifndef THREADS
#define THREADS
#include "functions.h"

struct mergeStruct{
	char* file1;
	char* file2;
	char* mergeFileName;
	int id;
};
pthread_mutex_t lock;

void* sort(void* stream){
	pthread_mutex_lock(&lock);
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
	pthread_mutex_unlock(&lock);
	return NULL;
}

void* mergeThread(void* info){
	pthread_mutex_lock(&lock);
	struct mergeStruct* data = (struct mergeStruct*)info;

	FILE* file1 = fopen(data->file1, "r+");
	FILE* file2 = fopen(data->file2, "r+");
	FILE* mergeFile = fopen(data->mergeFileName, "w+");
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
	free(info);
	pthread_mutex_unlock(&lock);
}

void oneThreadSort(){
	wait(NULL);
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
	FILE* sortedFile = fopen("twoThreadSorted", "w");
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
	// pthread_mutex_destroy(&lock);
	printTimeThreads(threadCount, time);
}

void fourThreadSort(){
	clock_t time = clock();
	struct mergeStruct* info;
	int threadCount = 4;
	pthread_t tid[threadCount];
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int count = countLines(stream, 1);
	splitFile(count, threadCount);
	FILE* xaa = fopen("xaa", "r+");
	FILE* xab = fopen("xab", "r+");
	FILE* xac = fopen("xac", "r+");
	FILE* xad = fopen("xad", "r+");
	FILE* file1 = fopen("file1", "w"); //creating new file to get rid of first line of xaa
	FILE* sortedFile = fopen("fourThreadSorted", "w");
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

	for(int i = 0; i < threadCount; i++){
		pthread_create(&tid[i], NULL, sort, (void*)fileNames[i]);
	}

	for(int i = 0; i < threadCount; i++){
		pthread_join(tid[i], NULL);
	}

	for(int id = 0; id < threadCount/2; id++){//use two processes to merge files in pairs
		info = (struct mergeStruct*)malloc(sizeof(struct mergeStruct));
		info-> file1 = fileNames[id*2];
		info->file2 = fileNames[id*2+1];
		info->mergeFileName = mergeFileNames[id];
		pthread_create(&tid[id], NULL, mergeThread, (void*)info);
	}

	for(int id = 0; id < threadCount/2; id++){
		pthread_join(tid[id], NULL);
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
	printTimeThreads(threadCount, time);
	fclose(xaa);
	fclose(xab);
	fclose(xac);
	fclose(xad);
	fclose(stream);
	freeArray(array, count);
	free(array);
	fclose(sortedFile);
	// pthread_mutex_destroy(&lock);
	system("rm xaa xab xac xad file1 merge1 merge2");
}

void tenThreadSort(){
	clock_t time = clock();
	pid_t child;
	int threadCount = 10;
	pthread_t tid[threadCount];
	struct mergeStruct* info;
	FILE* stream = fopen("all_month.csv", "r");
	if(stream == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int count = countLines(stream, 1);
	splitFile(count, threadCount);
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
	FILE* sortedFile = fopen("tenThreadSorted", "w");
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

	for(int i = 0; i < threadCount; i++){
		pthread_create(&tid[i], NULL, sort, (void*)fileNames[i]);
	}

	for(int i = 0; i < threadCount; i++){
		pthread_join(tid[i], NULL);
	}

	for(int id = 0; id < threadCount/2; id++){//use two processes to merge files in pairs
		info = (struct mergeStruct*)malloc(sizeof(struct mergeStruct));
		info-> file1 = fileNames[id*2];
		info->file2 = fileNames[id*2+1];
		info->mergeFileName = mergeFileNames[id];
		pthread_create(&tid[id], NULL, mergeThread, (void*)info);
	}

	for(int id = 0; id < threadCount/2; id++){
		pthread_join(tid[id], NULL);
	}

	for(int id = 0; id < 2; id++){//use 2 processes to merge merge1/merge2 and merge3/merge4
		info = (struct mergeStruct*)malloc(sizeof(struct mergeStruct));
		info-> file1 = mergeFileNames[id*2];
		info->file2 = mergeFileNames[id*2+1];
		info->mergeFileName = mergeLargeFileNames[id];
		pthread_create(&tid[id], NULL, mergeThread, (void*)info);
	}

	for(int id = 0; id < 2; id++){
		pthread_join(tid[id], NULL);
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
	printTimeThreads(threadCount, time);

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
	// pthread_mutex_destroy(&lock);
	system("rm xaa xab xac xad xae xaf xag xah xai xaj file1 merge1 merge2 merge3 merge4 merge5 mergeLarge1 mergeLarge2 mergeLarge3");
}

#endif //THREADS