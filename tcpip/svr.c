#include<stdio.h>
#include<stdlib.h>
#include<strings.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>


#define PORT 8888       // 侦听端口
#define BACKLOG 2       // 侦听队列长度

int main(int argc, int* argv[])
{
    int ss,sc;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    int err;
    pid_t pid;

    // 建立套接字
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

    // 绑定地址到套接字描述符
    err = bind(ss, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if(err < 0)
    {
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
        socklen_t addrlen = sizeof(struct sockaddr);
        sc = accept(ss, (struct sockaddr*)&client_addr, &addrlen);
        if(sc < 0)
            continue;

        pid = fork();
        if(pid == 0)
            close(ss);
        else
            close(sc);
    }


    return 0;
}


