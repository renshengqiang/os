#include <stdio.h>
#include <pthread.h>
#define PRODUCER_NUM 100
#define MAXITEMS 1000000

int buff[MAXITEMS];
struct {
    pthread_mutex_t mutex;
    int             nput;/*next index to store*/
    int             nvalue;/*next value to store*/
} put = {
    PTHREAD_MUTEX_INITIALIZER,
    0,0
};
struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int nready;
}nready={
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    0
};
void *produce(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&put.mutex);
        if(put.nput >= MAXITEMS) /*have produced MAXITEMS product, done*/
        {
            pthread_mutex_unlock(&put.mutex);
            return NULL;
        }
        buff[put.nput] = put.nvalue;
        ++put.nput;
        ++put.nvalue;
        pthread_mutex_unlock(&put.mutex);

        pthread_mutex_lock(&nready.mutex);
        ++nready.nready;
        if(nready.nready == 1)
            pthread_cond_signal(&nready.cond);
        pthread_mutex_unlock(&nready.mutex);

        *((int*)arg) += 1;
    }
    return NULL;
}
void *consume(void *arg)
{
    for(int i=0; i<MAXITEMS; ++i)
    {
        pthread_mutex_lock(&nready.mutex);
        while(nready.nready == 0)
            pthread_cond_wait(&nready.cond, &nready.mutex);
        --nready.nready;
        pthread_mutex_unlock(&nready.mutex);

        if(buff[i] != i)
            printf("buff[%d] = %d\n", i, buff[i]);
    }
    return NULL;
}
int main(int argc, char *argv[])
{
    int count[PRODUCER_NUM];
    pthread_t tid_producer[PRODUCER_NUM], tid_consumer;

    for(int i=0; i<PRODUCER_NUM; ++i)
    {
        count[i] = 0;
        pthread_create(&tid_producer[i], NULL, produce, &count[i]);
    }
    pthread_create(&tid_consumer, NULL, consume, NULL);

    /*wait for all producers and the consumer*/
    for(int i=0; i<PRODUCER_NUM; ++i)
    {
        pthread_join(tid_producer[i], NULL);
        printf("count[%d] = %d\n", i, count[i]);
    }
    pthread_join(tid_consumer, NULL);
    
    return 0;
}
