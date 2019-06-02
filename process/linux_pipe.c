#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define K 1024
#define WRITELEN (128*K)

/*
 * pipe default size is 512*8bytes = 4kb 实际测试是8k
 * ulimit -a
 * pipe size            (512 bytes, -p) 8
 * */
/*
 * 这个例子说明，管道操作是阻塞的，写进去的数据，如果不读，会占用管道，可能写不进去。
 * */
int main(void)
{
    int result = -1;
    int fd[2],nbytes;
    pid_t pid;
    char hello_info[WRITELEN] = "hello,pipe ,this is a test program";
    char read_buffer[10*K] = {0};
    int read_buffer_size = sizeof(read_buffer);
    int *write_fd = &fd[1];
    int *read_fd = &fd[0];

    result = pipe(fd);
    if(result == -1)
    {
        printf("create pipe failed %d", result);
        return -1;
    }

    pid = fork();
    if(result == -1)
    {
        printf("create progress failed %d", result);
        return -1;
    }

    if(0 == pid)//0 is returned in the child
    {
        int write_size = WRITELEN;
        result = 0;
        close(*read_fd);
        while(write_size >= 0)
        {
            result = write(*write_fd, hello_info, write_size);

            if(result > 0){
                write_size -= result;
                printf("写入%d个数据，剩余%d个数据\n", result, write_size);
            }else{
                sleep(10);
            }
        }

        return 0;
    }else{
        close(*write_fd);
        while(1)
        {
            nbytes = read(*read_fd, read_buffer, read_buffer_size);
            //nbytes = read(*read_fd, read_buffer, 8);
            // 返回 min(read_buffer_size, 管道缓冲区实际数据大小)
            if(nbytes <= 0){
                printf("没有数据写入了\n");
                break;
            }
            printf("get %d data, is %s\n", nbytes, read_buffer);
        }

        return 0;
    }
    return 0;
}
