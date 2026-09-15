#include<stdio.h>
#include<fcntl.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>

int main(){
      char bfr[100];
      printf("receiving msg from writer\n");
      int fd = open("/tmp/poojitha_fifo",O_RDONLY);
      read(fd,bfr,sizeof(bfr));
      printf("received message:%s", bfr);
      close(fd);
      return 0;
}
