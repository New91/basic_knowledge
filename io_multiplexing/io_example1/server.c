
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

#define V 1
static struct iovec* vs = NULL;
static struct iovec* vc = NULL;

extern void sig_proccess(int signo);
extern void sig_pipe(int signo);
static int s;
void sig_proccess(int signo)
{
    printf("catch a exit signal\n");

#if V
    if(vc)
        free(vc);
    if(vs)
        free(vs);
#endif
    exit(0);
}
void sig_pipe(int signo)
{
    printf("catch a pipe signal\n");

#if V
    if(vc)
        free(vc);
    if(vs)
        free(vs);
#endif
    exit(0);
}

#define PORT 8888
#define BACKLOG 2
/* 服务端对客户端的处理
 * */

#if 1

void process_conn_server(int s)
{
    char buffer[30];
    ssize_t size = 0;

    struct iovec *v = (struct iovec*)malloc(3*sizeof(struct iovec));
    if(!v){
        printf("not enough memory\n");
        return ;
    }
    vs = v; //挂接全局变量，便于释放

    //初始化消息
    struct msghdr msg;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_control = NULL;
    msg.msg_controllen = 0;
    msg.msg_iov = v;
    msg.msg_iovlen = 30;
    msg.msg_flags = 0;

    v[0].iov_base = buffer;
    v[1].iov_base = buffer + 10;
    v[2].iov_base = buffer + 20;
    v[0].iov_len = v[1].iov_len = v[2].iov_len = 10;

    while(1)
    {
        //memset(v,0, sizeof(struct iovec));
        // 从套接字中读取数据放到向量缓冲区中
        //size = readv(s, v, 3);
        size = recvmsg(s, &msg, 0);
        if(size == 0)
            return;

        printf("%s\n", msg.msg_iov->iov_base);
        sprintf(v[0].iov_base, "%d",size);
        sprintf(v[1].iov_base, "bytes alt");
        sprintf(v[2].iov_base, "ogether\n");
        // 写入字符串长度
        v[0].iov_len = strlen(v[0].iov_base);
        v[1].iov_len = strlen(v[1].iov_base);
        v[2].iov_len = strlen(v[2].iov_base);

        //writev(s, v, 3);
        sendmsg(s, &msg, 0);
    }
}

#else
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
#endif

int main(int argc, char* argv[])
{
    int ss;//服务器的socket描述符
    int sc;//客户端的socket描述符
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int err;
    pid_t pid;


    signal(SIGINT, sig_proccess);
    signal(SIGPIPE, sig_pipe);

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

























