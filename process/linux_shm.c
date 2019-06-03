#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <errno.h>
extern int errno;
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
};
typedef int sem_t;


static char *msg = "hello, shared memory";

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

void destory_sem(sem_t semid)
{
    union semun sem;
    sem.val = 0;
    semctl(semid, 0, IPC_RMID, sem);
}

int main()
{
    key_t key;
    int semid,shmid;
    char i,*shms,*shmc;
    struct semid_ds buf;
    int value = 0;
    char buffer[80];
    pid_t p;

    key = ftok("/ipc/sem", 'a');
    shmid = shmget(key, 1024, IPC_CREAT|0604);//获得共享内存，大小为1024字节

    semid = create_sem(key,0);
    p = fork();
    if(p != 0){ //父进程
        shms = (char*)shmat(shmid,0,0);// 挂接共享内存
        memcpy(shms,msg,strlen(msg)+1);
        sleep(10);
        Sem_P(semid);
        shmdt(shms);                   // 摘除共享内存

        destory_sem(semid);             // 销毁信号量
    }else{      //子进程
        shmc = (char*)shmat(shmid,0,0);// 挂接共享内存
        Sem_V(semid);
        printf("共享内存中的值为%s\n", shmc);
        shmdt(shmc);                   // 摘除共享内存
    }

    return 0;
}

