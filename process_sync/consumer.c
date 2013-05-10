#include "protocal.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    int         fd,flags;
    off_t       length;
    void        *ptr;
    struct shm_struct  *pshm;

    flags = O_RDWR | O_CREAT;
    fd = shm_open(OBJNAME, flags, 644);
    if( -1 == fd )
    {
        fprintf(stderr, "create shm object error\n");
        return -1;
    }
    ptr = mmap(NULL, sizeof(struct shm_struct), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == ptr)
    {
        fprintf(stderr, "memory map error\n");
        return -1;
    }
    pshm = (struct shm_struct*)ptr;

    for(int i=0; i<MAXITEMS; ++i)
    {
        pthread_mutex_lock(&(pshm->mutex));
        if(i>=pshm->nput)
        {
            pthread_mutex_unlock(&(pshm->mutex));
            --i;
            continue;
        }
        if(pshm->buff[i] != i)
            printf("buffer[%d] = %d\n", i, pshm->buff[i]);
        pthread_mutex_unlock(&(pshm->mutex));
    }
    //shm_unlink(OBJNAME);
    return 0;
}
