#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

// 分割字符串：按空格/制表符分割，返回 token 个数，token 存储在 argv 中
int split(char *str, char **argv)
{
    int i = 0;
    while (*str) {
        // 跳过空白
        while (*str == ' ' || *str == '\t') str++;
        if (*str == '\0') break;
        argv[i++] = str;  // 记录 token 起始
        // 跳过非空白字符
        while (*str && *str != ' ' && *str != '\t') str++;
        if (*str) {
            *str = '\0';  // 替换为字符串结束符
            str++;
        }
    }
    return i;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args...]\n");
        exit(1);
    }

    // 保存固定参数（命令名 + 初始参数）
    char *fixed_args[MAXARG];
    int fixed_count = argc - 1;  // 不包括 xargs 本身
    for (int i = 0; i < fixed_count; i++) {
        fixed_args[i] = argv[i+1];  // argv[0] 是 "xargs"
    }

    char line[512];
    int pos = 0;
    char ch;
    while (read(0, &ch, 1) > 0) {
        if (ch == '\n') {
            line[pos] = '\0';
            // 构造完整的 argv 数组
            char *cmd_argv[MAXARG];
            // 复制固定参数
            for (int i = 0; i < fixed_count; i++) {
                cmd_argv[i] = fixed_args[i];
            }
            // 分割行
            int extra = split(line, cmd_argv + fixed_count);
            // 如果行非空，执行命令
            if (extra > 0) {
                int pid = fork();
                if (pid == 0) {
                    exec(cmd_argv[0], cmd_argv);
                    fprintf(2, "xargs: exec failed\n");
                    exit(1);
                } else {
                    wait(0);
                }
            }
            pos = 0;
        } else {
            if (pos < sizeof(line) - 1) {
                line[pos++] = ch;
            }
        }
    }
    // 处理文件末尾没有换行的情况（一般不会）
    if (pos > 0) {
        line[pos] = '\0';
        char *cmd_argv[MAXARG];
        for (int i = 0; i < fixed_count; i++) {
            cmd_argv[i] = fixed_args[i];
        }
        int extra = split(line, cmd_argv + fixed_count);
        if (extra > 0) {
            int pid = fork();
            if (pid == 0) {
                exec(cmd_argv[0], cmd_argv);
                fprintf(2, "xargs: exec failed\n");
                exit(1);
            } else {
                wait(0);
            }
        }
    }
    exit(0);
}
