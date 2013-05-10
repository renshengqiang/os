#include "protocal.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>

int main()
{
    int         fd,flags;
    off_t       length;
    void        *ptr;
    struct shm_struct  *pshm;
    pthread_mutexattr_t attr;

    flags = O_RDWR | O_CREAT;
    fd = shm_open(OBJNAME, flags, 644);
    if( -1 == fd )
    {
        fprintf(stderr, "create shm object error, errno %d\n", errno);
        return -1;
    }
    ptr = mmap(NULL, sizeof(struct shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == ptr)
    {
        fprintf(stderr, "memory map error\n");
        return -1;
    }
    pshm = (struct shm_struct*)ptr;
    pshm->nput = pshm->nvalue = 0;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&(pshm->mutex), &attr);
    pthread_mutexattr_destroy(&attr);

    while(1)
    {
        pthread_mutex_lock(&(pshm->mutex));
        if(pshm->nput == MAXITEMS){
            pthread_mutex_unlock(&(pshm->mutex));
            return 0;
        }
        pshm->buff[pshm->nput] = pshm->nvalue;
        ++pshm->nput;
        ++pshm->nvalue;
        pthread_mutex_unlock(&(pshm->mutex));
    } 
    //shm_unlink(OBJNAME);
    return 0;
}
