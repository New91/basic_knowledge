#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static int run = 8;
static int retvalue ;

void *start_routine(void *arg)
{
    int *running = arg;
    printf("子线程初始化完毕,传入参数为:%d\n", *running);
    while(*running)
    {
        printf("子线程正在运行\n");
        usleep(1);
    }
    printf("子线程初始化完毕,传入参数为:%d\n", *running);
    printf("子线程退出\n");
    retvalue = 8;
    pthread_exit((void*)&retvalue);
}

int main(void)
{
    pthread_t pt;
    int ret = -1;
    int times = 8;
    int i = 0;
    int *ret_join = NULL;
    ret = pthread_create(&pt, NULL,(void*)start_routine,&run);
    if(ret != 0)
    {
        printf("建立线城失败\n");
        return 1;
    }

    usleep(1);
    for(;i<times;i++)
    {
        printf("主线程打印\n");
        usleep(10);
    }

    run = 0;
    pthread_join(pt, (void*)&ret_join);

    printf("线程返回值为%d\n", *ret_join);
    return 0;
}
