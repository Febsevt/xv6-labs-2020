#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    // 检查传入参数数量是否够两个
    if (argc < 2){
        printf("usage: sleep <ticks>\n");
        exit(1);
    }

    // 检查是否为正数
    if (atoi(argv[1]) < 0){
        printf("error: ticks must be non-negative\n");
        exit(1);
    }

    sleep(atoi(argv[1]));
    exit(0);
}