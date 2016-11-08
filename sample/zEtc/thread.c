#include <stdio.h>
#include <pthread.h>

int var;

void *add(void *n) {
	while(1) {
		var++;
		printf("add thread (%d)\n",var);
	}
}

void *sub(void *n) {
	while(1) {
		var++;
		printf("sub thread (%d)\n",var);
	}
}

int main()
{
	pthread_t p_thread[2];

	pthread_create(&p_thread[0], NULL, add, NULL);
	pthread_create(&p_thread[1], NULL, sub, NULL);

	pthread_join(p_thread[0],NULL);
	pthread_join(p_thread[1],NULL);

	return 0;
}

