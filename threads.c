#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *thread(void *vargp){
	int* myid = (int*) vargp;
	printf("I am a thread number %d\n", *myid);
	free(vargp);
}

int main(){

	pthread_t tid[3];

	for(int i = 0; i < 3; i++){
		int* id = malloc(sizeof(int));
		*id = i;
		pthread_create(&tid[i], NULL, thread, id);
	}

	for(int i = 0; i < 3; i++){
		pthread_join(tid[i], NULL);
	}
	printf("done\n");

	pthread_exit(NULL);
}