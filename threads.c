#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct data{
	char* argv;
	int id;
};

void* thread(void* vargp){
	struct data* stuff = (struct data*)vargp;
	
	printf("%d-%s\n", stuff->id, stuff->argv);
	free(vargp);
}

void* thread2(void* vargp){
	printf("%d", (*(int*)vargp));
	// free(vargp);
}


int main(){

	pthread_t tid[3];
	struct data* stuff;
	char* fileNames[4] = {"file1", "xab", "xac", "xad"};
	char* mergeFileNames[2] = {"merge1", "merge2"};

	// for(int i = 0; i < 3; i++){
	// 	stuff = (struct data*)malloc(sizeof(struct data));
	// 	stuff->argv = fileNames[i];	
	// 	(*stuff).id = i;
	// 	pthread_create(&tid[i], NULL, thread, (void*)stuff);
	// }

	for(int i = 0; i < 3; i++){ // test
		pthread_create(&tid[i], NULL, thread2, (void*)&i);
	}

	for(int i = 0; i < 3; i++){
		pthread_join(tid[i], NULL);
	}
	printf("done\n");

	pthread_exit(NULL);
}