#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include <errno.h>
extern int errno;

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};
typedef int sem_t;

sem_t create_sem(key_t key, int value)
{
    union semun sem;
    sem_t semid;
    sem.val = value;

    semid= semget(key,1,IPC_CREAT|0666);
    if(-1 == semid)
    {
        printf("create semaphore error:%d\n", errno);
        return -1;
    }

    semctl(semid, 0 , SETVAL, sem);
    return semid;
}

// PV操作与信号量的处理相关，P表示通过的意思，V表示释放的意思。
// 荷兰文中，通过叫passeren，释放叫vrijgeven
int Sem_P(sem_t semid)
{
    struct sembuf sops = {0,+1,IPC_NOWAIT};
    return (semop(semid, &sops, 1));
}

int Sem_V(sem_t semid)
{
    struct sembuf sops = {0,-1,IPC_NOWAIT};
    return (semop(semid, &sops, 1));
}

void setvalue_sem(sem_t semid, int value)
{
    union semun sem;
    sem.val = value;
    semctl(semid, 0 , SETVAL, sem);
}
int getvalue_sem(sem_t semid)
{
    union semun sem;
    return semctl(semid, 0, GETVAL, sem);
}

void destory_sem(sem_t semid)
{
    union semun sem;
    sem.val = 0;
    semctl(semid, 0, IPC_RMID, sem);
}

int main()
{
    key_t key;
    int semid;
    int i,value;
    key = ftok("/ipc/sem/",'a');

    semid = create_sem(key,111);

    for(i=0; i<=3 ; i++)
    {
        Sem_P(semid);
        Sem_V(semid);
    }
    value = getvalue_sem(semid);
    printf("信号量值为：%d\n", value);

    sleep(10);
    destory_sem(semid);
    return 0;
}
