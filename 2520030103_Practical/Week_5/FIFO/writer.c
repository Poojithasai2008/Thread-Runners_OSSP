#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
int main() {

    char *path = "/tmp/poojitha_fifo";
     mkfifo(path, 0666);
     printf("waiting for reader connection \n");
    char msg[] = "hello recevier";
    int fd = open(path, O_WRONLY);
    write(fd, msg, strlen(msg));
    printf("writer sent a message\n");
    close(fd);
    return 0;
}
