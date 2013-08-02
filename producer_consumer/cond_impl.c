#include <pthread.h>
#include <unistd.h> // sleep
#include <stdio.h>

#define RESOURCE_NUM 100
#define PRODUCER_NUM 10
#define CONSUMER_NUM 3

int resourceNum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full_cond = PTHREAD_COND_INITIALIZER, empty_cond = PTHREAD_COND_INITIALIZER;

void *ProducerFunc(void *pNum)
{
    int num = (int)(pNum);

    while(1)
    {
        sleep(3);
        pthread_mutex_lock(&mutex);
        while(resourceNum == RESOURCE_NUM) pthread_cond_wait(&empty_cond, &mutex);

        resourceNum++;
        printf("Producer %d create %d\n", num, resourceNum);

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&full_cond);
    }
}

void *ConsumerFunc(void *pNum)
{
    int num = (int)(pNum);

    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(resourceNum == 0) pthread_cond_wait(&full_cond, &mutex);

        printf("Consumer %d consume %d\n", num, resourceNum);
        resourceNum--;

        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&empty_cond);
        sleep(1);
    }
}

int main()
{
    int i;
    pthread_t thread;
    for(i=0; i<PRODUCER_NUM; ++i)
        pthread_create(&thread, NULL, ProducerFunc, (void *)i);

    for(i=0; i<CONSUMER_NUM; ++i)
        pthread_create(&thread, NULL, ConsumerFunc, (void *)i);

    pthread_join(thread, NULL);
    return 0;
}
