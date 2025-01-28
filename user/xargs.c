#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// 在子进程中运行指定程序
void run(char *program, char **args){
    if (fork() == 0){
        exec(program, args);
        exit(0);
    }
    return;
}

int main(int argc, char **argv){
    char buf[2048];             // 存储标准输入读取的数据
    char *p = buf;              // p, last_p用于遍历和标记当前位置
    char *last_p = buf;
    char *argsbuf[128];         // 存储最终要传递给命令的参数
    char **args = argsbuf;

    // 先将xargs的参数复制到argsbuf
    for (int i = 1; i < argc; i++){
        *args = argv[i];
        args++;
    }

    // 记录当前参数位置
    char **pa = args;

    // 从标准输入读取数据，存储在buf中
    while(read(0, p, 1) != 0){
        if (*p == ' ' || *p == '\n'){
            // 若遇到空格或换行符，则用字符串结束符\0代替，表示一个参数结束
            *p = '\0';
            
            // 将参数加入参数缓冲区argsbuf中
            *(pa++) = last_p;      // last_p记录这个参数的起始位置，将last_p所指字符串赋予*pa
            last_p = p + 1;        // last_p指向下一个参数起始位置

            // 根据要求，若遇到'\n'，则表示一组参数读取完毕，调用run执行命令
            if (*p == '\n'){
                *pa = 0;    // 在参数列表的末尾添加一个NULL终止符，确保 exec 函数能够正确识别参数列表的结束位置
                run(argv[1], argsbuf);
                pa = args;  // 使pa其指向参数缓冲区的起始位置，从而可以开始构建下一批参数
            }
        }
        // 继续读取数据
        p++;
    }

    // 若最后一行非空，处理最后一组参数
    if (pa != args){
        *p = '\0';
        *(pa++) = last_p;
        *pa = 0;

        run(argv[1], argsbuf);
    }

    // 等待所有子进程结束
    while(wait(0) != -1);
    exit(0);
}
