#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"
#include "../kernel/param.h"
#define MAXLEN 100  // 定义每个参数的长度最大值

int main(int argc, char* argv[]) {
    char* command = argv[1];
    char ch;
    char param[MAXARG][MAXLEN];  // MAXARG 表示调用exec的最大参数个数
    char* arg[MAXARG];

    while (1) {
        int count = argc - 1;
        memset(param, 0, MAXARG * MAXLEN);
        // 先将xargs后的参数拷贝 (command作为第一个参数)
        for (int i = 1; i < argc; ++i) {
            strcpy(param[i - 1], argv[i]);
        }
        int index = 0;  // 拷贝字符时的下标
        int flag = 0;   // 判断空格前是否有参数
        int result;
        // 拷贝之前的标准输入参数（管道可以将左侧的标准输出转化为标准输入）
        // 如果遇到换行符，则将当前行作为参数进行调用，然后再重新拷贝下一行
        while ((result = read(0, &ch, 1)) > 0 && ch != '\n') {
            // 若遇到空格，且前面已经有一个参数了，则拷贝下一个参数
            if (ch == ' ' && flag == 1) {
                count++;
                index = 0;
                flag = 0;
            }
            else if (ch != ' ') {
                param[count][index++] = ch;
                flag = 1;
            }
        }
        if (result <= 0)
            break;
        for (int i = 0; i < MAXARG - 1; ++i) {
            arg[i] = param[i];
        }
        arg[MAXARG - 1] = 0; 
        if (fork() == 0) {
            exec(command, arg);
            exit(0);
        }
        wait(0);
    }
    exit(0);
}

