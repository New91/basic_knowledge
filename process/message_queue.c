#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
extern int errno;
/*
 *
 * 查看消息队列，删除消息队列
[root@Master process]# ipcs

------ Message Queues --------
key        msqid      owner      perms      used-bytes   messages
0x62030ede 65536      root       146        0            0
[root@Master process]# ipcrm -q 65536
msgctl的三个命令
IPC_STAT 获取IPC装填
IPC_SET  设置属性
IPC_RMID 删除队列
*/
/* 消息的缓冲区结构 */
struct msgmbuf{
    long mtype;
    char mtext[10];
};
void msg_show_atrr(int msg_id, struct msqid_ds msg_info);
#define MSG_TYPE 0x11 // 消息类型，每个客户端可以设定一个消息类型，与其他客户端区分开
#define MSG_TEST "测试消息"

int main(void)
{
    int ret = -1;
    int msg_flags;
    int msg_id;
    key_t key;
    struct msqid_ds msg_info;
    struct msgmbuf msg_mbuf;
    char *msg_path = "/ipc/msg/";
    key = ftok(msg_path, 'b'); // msg_path , which must refer to an existing, accessible file
    if(key != -1){
        printf("成功建立key\n");
    }else{
        printf("建立key失败\n");
        return -1;
    }

    msg_flags = IPC_CREAT | IPC_EXCL; // IPC_EXCL配合IPC_CREAT使用，保证建立一个新的消息队列，如果不是则报错
    msg_id = msgget(key, msg_flags | 0x0666);
    if( -1 == msg_id){
        printf("建立msg失败\n");
        return -1;
    }
    msg_show_atrr(msg_id, msg_info);

    /* 发送消息 */
    int msg_sflags = IPC_NOWAIT;//如果消息队列已满，则不会再写入。如不指定，则阻塞。
    msg_mbuf.mtype = MSG_TYPE;
    memcpy(msg_mbuf.mtext,MSG_TEST,sizeof(MSG_TEST));
    //printf("%d\n", sizeof(MSG_TEST));//13
    //printf("%d\n", strlen(MSG_TEST));//12
    ret = msgsnd(msg_id, &msg_mbuf, sizeof(MSG_TEST), msg_sflags);
    if(-1 == ret)
        printf("发送消息失败%d\n", errno);
    msg_show_atrr(msg_id, msg_info);

    /* 接受消息 */
    int msg_rflags = IPC_NOWAIT | MSG_NOERROR;
    struct msgmbuf msg_mbuf_rcv = {0};
    ret = msgrcv(msg_id, &msg_mbuf_rcv, 13, MSG_TYPE, msg_rflags);
    if(-1 == ret)
        printf("接受消息失败%d\n", errno);
    else{
        printf("接受消息成功，长度%d\n", ret);
        printf("%s\n", msg_mbuf_rcv.mtext);
    }
    msg_show_atrr(msg_id, msg_info);

    /* 设置消息 */
    msg_info.msg_perm.uid = 8;
    msg_info.msg_perm.gid = 8;
    msg_info.msg_qbytes = 12345;
    ret = msgctl(msg_id, IPC_SET, &msg_info);
    if(-1 == ret)
        printf("设置消息失败\n");
    else
        printf("设置消息成功\n");
    msg_show_atrr(msg_id, msg_info);

    ret = msgctl(msg_id, IPC_RMID, NULL);
    if( -1 == ret){
        printf("删除消息失败\n");
        return ret;
    }


    return ret;
}

void msg_show_atrr(int msg_id, struct msqid_ds msg_info)
{
    int ret = -1;
    sleep(1);
    ret = msgctl(msg_id, IPC_STAT, &msg_info);
    if(-1 == ret){
        printf("获取消息失败\n");
        return ;
    }

    printf("\n");
    printf("队列中的字节数:%ld\n", msg_info.msg_cbytes);
    printf("队列中的消息数:%d\n", (int)msg_info.msg_qnum);
    printf("队列中的最大字节数:%d\n", (int)msg_info.msg_qbytes);
    printf("最后发送消息的进程pid:%d\n", msg_info.msg_lspid);
    printf("最后接收消息的进程pid:%d\n", msg_info.msg_lrpid);
    printf("最后发送消息的时间:%s", ctime(&(msg_info.msg_stime)));
    printf("最后接受消息的时间:%s", ctime(&(msg_info.msg_rtime)));
    printf("最后变化的时间:%s", ctime(&(msg_info.msg_ctime)));
    printf("消息UID是:%d\n", msg_info.msg_perm.uid);
    printf("消息GID是:%d\n", msg_info.msg_perm.gid);

    return;
}
