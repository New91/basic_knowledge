#include <signal.h>
#include <stdio.h>
#include <errno.h>
typedef void (*sighandler_t)(int);
static void sig_handle(int signo)
{
    if(SIGINT == signo)
    {
        printf("接收到SIGINT信号\n");
    }else if(SIGHUP == signo){
        printf("接收到SIGHUP信号\n");
    }else{
        printf("接收到%d信号\n", signo);
    }
    return;
}

int main(void)
{
    sighandler_t ret;
    ret = signal(SIGHUP,sig_handle);
    if(SIG_ERR == ret){
        printf("为SIGHUP挂在信号处理函数失败 %d\n", errno);
        return -1;
    }

    ret = signal(SIGINT,sig_handle);
    if(SIG_ERR == ret){
        printf("为SIGINT挂在信号处理函数失败 %d\n", errno);
        return -1;
    }


    while(1);
}
