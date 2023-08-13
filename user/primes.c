#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

#define PrimeNum 35

void primes(int *parent_pipe)
{
    close(parent_pipe[1]);

    // 首先读取第一个数字，如果read返回0，说明已经筛选完全部数字，那么关闭所有文件描述符，并退出即可
    // 因为上一个进程在发送完全部数据后，就将写端关闭了，当前进程也将写端关闭了，此时通过管道读的话，如果管道中已经没有数据了，那么read就会返回0
    int n;
    if (read(parent_pipe[0], &n, sizeof(int)) == 0)
    {
        close(parent_pipe[0]);
        exit(0);
    }

    int child_pipe[2];
    pipe(child_pipe);
    int tid = fork();
    if (tid < 0)
    {
        exit(1);
    }
    else if (tid == 0)
    {
        close(parent_pipe[0]); // 子进程的子进程已经用不到祖父进程的管道了，所以关掉
        primes(child_pipe);
    }
    else
    {
        close(child_pipe[0]);
        printf("prime %d \n", n);
        int temp = n;
        while (read(parent_pipe[0], &n, sizeof(int)) != 0)
        {
            if (n % temp != 0)
            {
                write(child_pipe[1], &n, sizeof(int));
            }
        }
        close(child_pipe[1]);
        wait(0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    int parent_pipe[2];
    pipe(parent_pipe);

    int tid = fork();
    if (tid < 0)
    {
        exit(1);
    }
    else if (tid == 0)
    {
        primes(parent_pipe);
    }
    else
    {
        close(parent_pipe[0]);
        for (int i = 2; i <= PrimeNum; ++i)
        {
            write(parent_pipe[1], &i, sizeof(int));
        }
        close(parent_pipe[1]);
        wait(0);
    }
    exit(0);
}
