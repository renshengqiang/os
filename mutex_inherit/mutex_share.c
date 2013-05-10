#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(void)
{
    pid_t pid;
    int   mid;
    int   bCreate = 0;      // 是否新建mutex
    key_t key = 0x12341235;

    pthread_mutex_t *mut;
    pthread_mutexattr_t attr;

    // shared memory
    mid = shmget(key, 0, 0);

    printf("sizeof mutex: %d\n" ,sizeof(pthread_mutex_t));
    if( mid == -1 )         // 共享内存不存在，新建
    {
        bCreate = 1;
        mid = shmget(key, 64, IPC_CREAT | 0666);
    }
    mut = (pthread_mutex_t *) shmat(mid, 0, 0);

    if( bCreate == 1 )      // 初始化mutex，设置为进程间共享
    {
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(mut, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    printf("lock\n");
    pthread_mutex_lock(mut);

    printf("fork\n");
    pid = fork();
    if( pid == 0 )          // 子进程无限期睡眠
    {
        while( 1 ) pause();
        exit(0);
    }

    printf("unlock\n");
    pthread_mutex_unlock(mut);

    printf("lock again\n");
    pthread_mutex_lock(mut);
    printf("over\n");

    pthread_mutex_unlock(mut);
    pthread_mutex_destroy(mut);
    kill(pid, SIGKILL);     // 杀死子进程
    return(0);
}

