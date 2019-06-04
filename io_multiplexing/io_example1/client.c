
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
void sig_proccess_client(int signo)
{
    printf("catch a exit signal\n");
    _exit(0);
}
void sig_pipe(int signo)
{
    printf("catch a pipe signal\n");
}

#define PORT 8888
// 客户端的处理过程
void process_conn_client(int s)
{
    ssize_t size = 0;
    char buffer[1024];
    for(;;)
    {
        size = read(0, buffer, 1024);
        if(size > 0){
            send(s, buffer, size, 0);
            size = recv(s, buffer, 1024, 0);
            write(1, buffer, size);
        }
    }
}

int main(int argc, char* argv[])
{
    struct sockaddr_in server_addr;
    int err;

    if(argc == 1){
        printf("please input server addr\n");
        return -1;
    }

    signal(SIGINT, sig_proccess_client);
    signal(SIGPIPE, sig_pipe);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s < 0){
        printf("socket error\n");
        return -1;
    }

    // 设置服务器地址
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);
    // 将用户输入的字符串类型IP转换成整型
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connect(s, (struct sockaddr*)&server_addr, sizeof(struct sockaddr));

    process_conn_client(s);
    return 0;
}

























