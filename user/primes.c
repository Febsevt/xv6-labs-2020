#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 筛选质数，接收左邻居管道为参数
void sieve(int pleft[2]){
    // 从左邻居读
    int p;
    read(pleft[0], &p, sizeof(p));
    if (p == -1){   // 读到-1，表示结束，退出进程
        exit(0);
    }
    printf("prime %d\n", p);

    // 创建新管道，即右邻居，接收左邻居筛选出的质数
    int pright[2];
    pipe(pright);

    if (fork() == 0){       // 右邻居
        close(pright[1]);   // 右邻居时不需要右邻居的写端，关闭
        close(pleft[0]);    // 右邻居时不需要左邻居的读端，关闭
        sieve(pright);      // 递归调用质数筛选函数
    }
    else{                   // 当前进程
        close(pright[0]);   // 不需要右邻居的读端
        // 从左邻居接收剩余数字
        int buf;
        while(read(pleft[0], &buf, sizeof(buf)) && buf != -1){
            if (buf % p != 0){
                write(pright[1], &buf, sizeof(buf));
            }
        }

        // 此时接收到左邻居传来的-1，同样传给右邻居，以结束右邻居进程
        buf = -1;
        write(pright[1], &buf, sizeof(buf));
        wait(0);
        exit(0);
    }
}

int main(int argc, char **argv){    // **argv等同于*argv[]
    // 创建初始管道
    int input_p[2];
    pipe(input_p);

    if (fork() == 0){       // 子进程，即右邻居
        close(input_p[1]);  // 右邻居用不到左邻居写端，关闭
        sieve(input_p);
        exit(0);
    }
    else{
        close(input_p[0]);  // 父进程用不到读端，关闭
        for (int i = 2; i <= 35; i++){
            write(input_p[1], &i, sizeof(i));
        }
        // 写入结束标志
        int i = -1;
        write(input_p[1], &i, sizeof(i));
    }
    // 注意：这里无法等待子进程的子进程，只能等待直接子进程，无法等待间接子进程。
	// 在 sieve() 中再各自执行 wait(0)，形成等待链
    wait(0);
    exit(0);
}
