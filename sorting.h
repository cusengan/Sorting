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
	pid_t child2;

	int count = countLines(stream, 1);
	splitFile(count, processCount);
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
	if(xaa == NULL || xab == NULL || xac == NULL || xad == NULL){
		perror("Failed to open file");
		exit(1);
	}

	int size1 = countLines(xaa, 1);
	int size2 = countLines(xab, 0);
	int size3 = countLines(xac, 0);
	int size4 = countLines(xad, 0);
	char** array = allocateArrayNoFile(count);
	FILE* files[processCount];
	int sizes[processCount];
	sizes[0] = size1;
	sizes[1] = size2;
	sizes[3] = size3;
	sizes[4] = size4;
	files[0]= xaa;
	files[1] = xab;
	files[2] = xac;
	files[3] = xad;
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
	printArray(array, count);
	time = clock() - time;
	printTime(processCount, time);
	fclose(xaa);
	fclose(xab);
	fclose(xac);
	fclose(xad);
	fclose(stream);
	freeArray(array, count);
	free(array);
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
	if(xaa == NULL || xab == NULL || xac == NULL || xad == NULL|| xae == NULL || xaf == NULL || xag == NULL || xah == NULL || xai == NULL || xaj == NULL){
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

	for(int id = 0; id < processCount; id++){//sort files
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
			if(id == 4){
				char** array5 = allocateArray(xae, size5, 0);
				insertionSort(array5, size5);
				writeToFile(xae, array5, size5);
				freeArray(array5, size5);
				free(array5);				
			}
			if(id == 5){
				char** array6 = allocateArray(xaf, size6, 0);
				insertionSort(array6, size6);
				writeToFile(xaf, array6, size6);
				freeArray(array6, size6);
				free(array6);				
			}
			if(id == 6){
				char** array7 = allocateArray(xag, size7, 0);
				insertionSort(array7, size7);
				writeToFile(xag, array7, size7);
				freeArray(array7, size7);
				free(array7);				
			}
			if(id == 7){
				char** array8 = allocateArray(xah, size8, 0);
				insertionSort(array8, size8);
				writeToFile(xah, array8, size8);
				freeArray(array8, size8);
				free(array8);				
			}
			if(id == 8){
				char** array9 = allocateArray(xai, size9, 0);
				insertionSort(array9, size9);
				writeToFile(xai, array9, size9);
				freeArray(array9, size9);
				free(array9);				
			}
			if(id == 9){
				char** array10 = allocateArray(xaj, size10, 0);
				insertionSort(array10, size10);
				writeToFile(xaj, array10, size10);
				freeArray(array10, size10);
				free(array10);

			}

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
				writeToFile(merge1, arrayMerge, size1 + size2);
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

			if(id == 2){
				char** arrayMerge = allocateArrayNoFile(size5 + size6);
				merge(arrayMerge, xae, xaf, size5, size6);
				FILE* merge3 = fopen("merge3", "w+");
				if(merge3 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				writeToFile(merge3, arrayMerge, size5 + size6);
				freeArray(arrayMerge, size5+size6);
				free(arrayMerge);
				fclose(merge3);
			}

			if(id == 3){
				char** arrayMerge = allocateArrayNoFile(size7 + size8);
				merge(arrayMerge, xag, xah, size7, size8);
				FILE* merge4 = fopen("merge4", "w+");
				if(merge4 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				writeToFile(merge4, arrayMerge, size7 + size8);
				freeArray(arrayMerge, size7+size8);
				free(arrayMerge);
				fclose(merge4);					
			
			}

			if(id == 4){
				char** arrayMerge = allocateArrayNoFile(size9 + size10);
				merge(arrayMerge, xai, xaj, size9, size10);
				FILE* merge5 = fopen("merge5", "w+");
				if(merge5 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				writeToFile(merge5, arrayMerge, size9 + size10);
				freeArray(arrayMerge, size9 + size10);
				free(arrayMerge);
				fclose(merge5);					
			
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
		wait(NULL);
		wait(NULL);
		wait(NULL);
	}

	for(int id = 0; id < 2; id++){//use 2 processes to merge merge1/merge2 and merge3/merge4
		child = fork();
		if(child== 0){//in child{
			if(id == 0){
				int newSize = size1 + size2 + size3 + size4;
				char** arrayMerge = allocateArrayNoFile(newSize);
				FILE* merge1 = fopen("merge1", "r+");
				FILE* merge2 = fopen("merge2", "r+");
				FILE* mergeLarge1 = fopen("mergeLarge1", "w+");
				if(merge1 == NULL || merge2 == NULL|| mergeLarge1 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				merge(arrayMerge, merge1, merge2, size1 + size2, size3 + size4);
				writeToFile(mergeLarge1, arrayMerge, newSize);
				freeArray(arrayMerge, newSize);
				free(arrayMerge);
				fclose(merge1);
				fclose(merge2);
				fclose(mergeLarge1);
			}

			if(id == 1){
				int newSize = size5 + size6 + size7 + size8;
				char** arrayMerge = allocateArrayNoFile(newSize);
				FILE* merge3 = fopen("merge3", "r+");
				FILE* merge4 = fopen("merge4", "r+");
				FILE* mergeLarge2 = fopen("mergeLarge2", "w+");
				if(merge3 == NULL || merge4 == NULL || mergeLarge2 == NULL){
					perror("Failed to open file");
					exit(1);
				}
				merge(arrayMerge, merge3, merge4, size5 + size6, size7 + size8);
				writeToFile(mergeLarge2, arrayMerge, newSize);
				freeArray(arrayMerge, newSize);
				free(arrayMerge);
				fclose(merge3);
				fclose(merge4);
				fclose(mergeLarge2);
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
	printArray(array, count);
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
	freeArray(array, count);
	free(array);
	system("rm xaa xab xac xad xae xaf xag xah xai xaj file1 merge1 merge2 merge3 merge4 merge5 mergeLarge1 mergeLarge2 mergeLarge3");
}




#endif //SORTING