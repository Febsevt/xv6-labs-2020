#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 递归查找path路径下是否有target文件
void find(char *path, char *target){
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // 以只读(0)方式打开路径，失败则报错并返回
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    // 获取状态信息，失败则报错并返回
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 分别处理目录和文件
    switch(st.type){
    // 若是文件，检查文件名与目标文件是否匹配
    case T_FILE:
        if (strcmp(path + strlen(path) - strlen(target), target) == 0){
            printf("%s\n", path);
        }
        break;
    // 若是目录
    case T_DIR:
        // 检查路径长度是否超过缓冲区大小(路径 + "/" + 最大目录项长度 + "\0")
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';             // 在路径后添加"/"
        // 读取目录项
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            // 获取目录项状态信息
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            // 排除"."和".."目录
            if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0) {
                // 递归查找
                find(buf, target);
            }
        }
        break;
    }
    close(fd);
}

int main(int argc, char **argv){
    if (argc < 3){
        printf("usage: find <path> <target>\n");
        exit(0);
    }
    
    find(argv[1], argv[2]);
    exit(0);
}
