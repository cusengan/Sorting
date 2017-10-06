#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct data{
	char* argv[4];
	int id;
};

void* thread(void* vargp){
	struct data* stuff = (struct data*)vargp;
	
	printf("%d-%s\n", stuff->id, stuff->argv[stuff->id]);
	free(vargp);
}


int main(){

	pthread_t tid[3];
	struct data* stuff;
	// stuff.argv[0] = "hi";
	// stuff.argv[1] = "hello";
	// stuff.argv[2] = "bruh";


	for(int i = 0; i < 3; i++){
		stuff = (struct data*)malloc(sizeof(struct data));
		stuff->argv[0] = "hi";
		stuff->argv[1] = "hello";
		stuff->argv[2] = "bruh";	
		(*stuff).id = i;
		pthread_create(&tid[i], NULL, thread, (void*)stuff);
	}

	for(int i = 0; i < 3; i++){
		pthread_join(tid[i], NULL);
	}
	printf("done\n");

	pthread_exit(NULL);
}