
#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../user/user.h"


int main(int argc, char const *argv[])
{
    /* code */
    if (argc >1)
    {
        /* code */
        fprintf(2,"only one parameter is required");
        exit(1);
    }
    int pid;
    int parent_fd[2];
    int child_fd[2];
    char buf[20];
    //为父子进程建立管道
    pipe(child_fd); 
    pipe(parent_fd);

    // 子进程
    if((pid = fork()) == 0){
        close(parent_fd[1]); //要读，故把写端关闭
        read(parent_fd[0],buf, 4); 
        printf("%d: received %s\n",getpid(), buf);

        close(child_fd[0]);//子进程要写，故把读关了
        write(child_fd[1], "pong", sizeof(buf));//然后去写pong
        exit(0);
    }
    // 父进程
    else{
        close(parent_fd[0]); //父进程要发送信息，把读端关闭
        write(parent_fd[1], "ping",4); //写，发送ping
        close(child_fd[1]);


        read(child_fd[0], buf, sizeof(buf));
        printf("%d: received %s\n", getpid(), buf);
        exit(0);

    }
    return 0;
}

