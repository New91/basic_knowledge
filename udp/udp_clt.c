#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT_SERV 8888
#define BUFF_LEN 256
void udp_clt_echo(int s, struct sockaddr* to)
{
    int n;
    char buff[BUFF_LEN] = "UDP TEST";
    struct sockaddr_in from;
    socklen_t len = sizeof(*to);
    sendto(s, buff, BUFF_LEN, 0, to, len);
    recvfrom(s, buff, BUFF_LEN, 0, (struct sockaddr*)&from, &len);
    printf("recved:%s\n");
}

int main(int argc, char* argv[])
{
    int s;
    struct sockaddr_in addr_srv;
    s = socket(AF_INET, SOCK_DGRAM, 0); // 建立数据包套接字

    memset(&addr_srv, 0, sizeof(addr_srv));
    addr_srv.sin_family = AF_INET;
    addr_srv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_srv.sin_port = htons(PORT_SERV);

    udp_clt_echo(s, (struct sockaddr*)&addr_srv);
    close(s);
    return 0;
}
