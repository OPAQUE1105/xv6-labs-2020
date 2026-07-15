#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
    int p1[2], p2[2];  
    char buf[1];

    // 1. 创建两个管道
    pipe(p1);
    pipe(p2);

    int pid = fork();
    if (pid == 0) {
        // ----- 子进程 -----
        close(p1[1]);   // 关闭 p1 的写端
        close(p2[0]);   // 关闭 p2 的读端

        // 从 p1 读取父进程发来的字节
        read(p1[0], buf, 1);
        printf("%d: received ping\n", getpid());

        // 向 p2 写入一个字节给父进程
        write(p2[1], "x", 1);   // 内容无所谓，传一个字节就行

        // 关闭剩余端口
        close(p1[0]);
        close(p2[1]);
        exit(0);
    } else {
        // ----- 父进程 -----
        close(p1[0]);   // 关闭 p1 的读端
        close(p2[1]);   // 关闭 p2 的写端

        // 向 p1 写入一个字节给子进程
        write(p1[1], "x", 1);

        // 从 p2 读取子进程发来的字节
        read(p2[0], buf, 1);
        printf("%d: received pong\n", getpid());

        // 关闭剩余端口
        close(p1[1]);
        close(p2[0]);

        wait(0);
        exit(0);
    }
}
