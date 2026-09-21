#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define SIZE 100

void handle_sigusr1(int sig)
{
    printf("\nClient 1 received SIGUSR1\n");
}

int main()
{
    int write_fd, read_fd;
    char message[SIZE];
    char response[SIZE];

    signal(SIGUSR1, handle_sigusr1);

    printf("Client 1 started. PID = %d\n", getpid());

    write_fd = open("client1_to_server", O_WRONLY);

    if (write_fd == -1) {
        perror("open write FIFO");
        exit(1);
    }

    read_fd = open("server_to_client1", O_RDONLY);

    if (read_fd == -1) {
        perror("open read FIFO");
        exit(1);
    }

    while (1) {

        printf("Client 1 > ");
        fgets(message, SIZE, stdin);

        message[strcspn(message, "\n")] = '\0';

        write(write_fd, message, strlen(message) + 1);

        memset(response, 0, SIZE);

        read(read_fd, response, SIZE - 1);

        printf("%s\n", response);

        if (strcmp(message, "exit") == 0)
            break;
    }

    close(write_fd);
    close(read_fd);

    printf("Client 1 terminated.\n");

    return 0;
}
