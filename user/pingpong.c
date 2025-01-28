#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    // p2c: parent -> child    c2p: child -> parent
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);

    if (fork() != 0){       // 父进程
        // 父向子写
        write(p2c[1], ".", 1);
        close(p2c[1]);

        // 父从子读
        char buf;
        read(c2p[0], &buf, 1);
        printf("%d: received pong\n", getpid());
        // 等子进程结束
        wait(0);
    }
    else{                   // 子进程
        // 子从父读
        char buf;
        read(p2c[0], &buf, 1);
        printf("%d: received ping\n", getpid());

        // 子向父写
        write(c2p[1], &buf, 1);
        close(c2p[1]);
    }

    // 关闭读端
    close(c2p[0]);
    close(p2c[0]);

    exit(0);
}