#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define M 10
#define  NP 6
#define  NC 4

int in = 0;  
int out = 0;

int buff[M] = {0,0,0,0,0,0,0,0,0,0};

sem_t empty_sem;
sem_t full_sem;  
pthread_mutex_t mutex;

void print()
{
	int i;
	for(i = 0; i < M; i++)
		printf("%d ", buff[i]);
	printf("\n");
}


void *produce(void *num)
{

	while(1)
	{

		sem_wait(&empty_sem);
		pthread_mutex_lock(&mutex);

		printf("\n producer%d put a product in buffer[%d].   Now buffer is: ", *((int*)num), in);
		buff[in] = 1;
		print();
		in = (in+1) % M;

		pthread_mutex_unlock(&mutex);
		sem_post(&full_sem);

		sleep(1);   
	}
}

void *consumer(void *num)
{

	while(1)
	{   
		sem_wait(&full_sem);      
		pthread_mutex_lock(&mutex);

		printf("\n consumer%d get a product from buffer[%d]. Now buffer is: ", *((int*)num), out);
		buff[out] = 0;
		print();
		out=(out+1)%M;

		pthread_mutex_unlock(&mutex);
		sem_post(&empty_sem);

		sleep(1);
	}
}

int main()
{
	pthread_t p;
	pthread_t c;
	int i,ini1,ini2,ini3;
	int ret;
	int threadProducerNumber[NP];
	int threadConsumerNumber[NC];

	for(i=0;i<NP;i++)
		threadProducerNumber[i]=i;
	for(i=0;i<NC;i++)
		threadConsumerNumber[i]=i;


	ini1 = sem_init(&empty_sem, 0, M);
	ini2 = sem_init(&full_sem, 0, 0);

	if(ini1 && ini2 != 0)
	{
		printf("sem init failed \n");
		exit(1);
	}

	ini3 = pthread_mutex_init(&mutex, NULL);
	if(ini3 != 0)
	{
		printf("mutex init failed \n");
		exit(1);
	}

	for(i=0;i<NP;i++)
	{
		ret=pthread_create(&p, NULL, produce, (void *) &threadProducerNumber[i]);
		if(ret != 0)
		{
			printf("product%d creation failed \n", i);
			exit(1);
		}
	}

	for(i=0;i<NC;i++)
	{
		ret= pthread_create(&c, NULL, consumer, (void *) &threadConsumerNumber[i]);
		if(ret != 0)
		{
			printf("consumer%d creation failed \n", i);
			exit(1);
		}
	}

	pthread_join(p,NULL);
	pthread_join(c,NULL);

	return 0;
}
