#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *target)
{
    int fd;
    struct stat st;
    char buf[512], *p;
    struct dirent de;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type == T_FILE) {
        // ----- 手动提取文件名（代替 strrchr）-----
        char *base = path;
        for (char *q = path; *q; q++) {
            if (*q == '/') base = q + 1;
        }
        if (strcmp(base, target) == 0) {
            printf("%s\n", path);
        }
        close(fd);
        return;
    }

    if (st.type == T_DIR) {
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) continue;
            if (strcmp(de.name, ".") == 0) continue;
            if (strcmp(de.name, "..") == 0) continue;

            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;          // 确保字符串结束
            find(buf, target);
        }
        close(fd);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(2, "Usage: find <directory> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
