#include "kernel/types.h"
#include "user/user.h"

// 递归函数：从 fd 读取数字，过滤并传递给下一个进程
void primes(int fd_read)
{
    int first, n;
    // 读取第一个数（必须是素数）
    if (read(fd_read, &first, sizeof(int)) == 0) {
        close(fd_read);
        exit(0);
    }
    printf("prime %d\n", first);

    int p[2];
    pipe(p);
    int pid = fork();
    if (pid == 0) {
        // 子进程：继续过滤
        close(p[1]);        // 关闭写端
        primes(p[0]);       // 递归调用
        close(p[0]);
        exit(0);
    } else {
        // 当前进程：过滤并发送剩余数字给子进程
        close(p[0]);        // 关闭读端
        while (read(fd_read, &n, sizeof(int)) > 0) {
            if (n % first != 0) {
                write(p[1], &n, sizeof(int));
            }
        }
        // 读完所有数，关闭写端，通知子进程结束
        close(fd_read);
        close(p[1]);
        wait(0);            // 等待子进程结束
        exit(0);
    }
}

int main(void)
{
    int p[2];
    pipe(p);

    int pid = fork();
    if (pid == 0) {
        // 子进程：执行素数筛
        close(p[1]);        // 关闭写端
        primes(p[0]);
        close(p[0]);
        exit(0);
    } else {
        // 父进程：生成数字 2~35
        close(p[0]);        // 关闭读端
        for (int i = 2; i <= 35; i++) {
            write(p[1], &i, sizeof(int));
        }
        close(p[1]);        // 关闭写端，通知子进程开始
        wait(0);            // 等待子进程结束
        exit(0);
    }
}
