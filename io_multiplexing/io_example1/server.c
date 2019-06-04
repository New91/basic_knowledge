
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <strings.h>

extern void sig_proccess(int signo);
extern void sig_pipe(int signo);
static int s;
void sig_proccess(int signo)
{
    printf("catch a exit signal\n");
    exit(0);
}

#define PORT 8888
#define BACKLOG 2
/* 服务端对客户端的处理
 * */
void process_conn_server(int s)
{
    ssize_t size = 0;
    char buffer[1024];
    for(;;)
    {
        size = recv(s, buffer, 1024, 0);
        if(0 == size)
            return;
        printf("%s\n", buffer);
        sprintf(buffer, "%d bytes altogether \n", size);
        send(s, buffer, strlen(buffer)+1,0);//发送给客户端
    }
}

int main(int argc, char* argv[])
{
    int ss;//服务器的socket描述符
    int sc;//客户端的socket描述符
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int err;
    pid_t pid;


    signal(SIGINT, sig_proccess);
    signal(SIGPIPE, sig_proccess);

    ss = socket(AF_INET, SOCK_STREAM, 0);
    if(ss < 0){
        printf("socket error\n");
        return -1;
    }

    // 设置服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    err = bind(ss, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(err < 0){
        printf("bind error\n");
        return -1;
    }

    err = listen(ss, BACKLOG);
    if(err < 0){
        printf("listen error\n");
        return -1;
    }

    for(;;)
    {
        int addrlen = sizeof(struct sockaddr);
        // 接收客户端连接
        sc = accept(ss, (struct sockaddr*)&client_addr, &addrlen);
        if(sc < 0)
            continue;

        // 建立一个新的进程处理到来的连接
        pid = fork();
        if(pid == 0){
            close(ss);
            process_conn_server(sc);
        }else
            close(sc);
    }

    return 0;
}

























