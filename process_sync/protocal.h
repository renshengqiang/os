#ifndef _PROTOCAL_H_
#define _PROTOCAL_H_

#include <pthread.h>
#define MAXITEMS 1000000
#define OBJNAME "shmfile"
struct shm_struct{
    pthread_mutex_t mutex;
    int nput,nvalue;
    int buff[MAXITEMS];
};

#endif
