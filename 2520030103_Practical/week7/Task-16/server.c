#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#define MAX_CLIENTS 3
#define SIZE 100

char c2s[MAX_CLIENTS][30] = {
    "client1_to_server",
    "client2_to_server",
    "client3_to_server"
};

char s2c[MAX_CLIENTS][30] = {
    "server_to_client1",
    "server_to_client2",
    "server_to_client3"
};

pid_t children[MAX_CLIENTS];

volatile sig_atomic_t shutdown_server = 0;

void handle_sigusr1(int sig)
{
    printf("\nServer received SIGUSR1\n");
}

void handle_sigint(int sig)
{
    shutdown_server = 1;
}

void handle_sigchld(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        printf("A client process terminated.\n");
    }
}

void client_handler(int client_no)
{
    int read_fd, write_fd;
    char message[SIZE];
    char response[SIZE];

    read_fd = open(c2s[client_no], O_RDONLY);

    if (read_fd == -1) {
        perror("open client_to_server");
        exit(1);
    }

    write_fd = open(s2c[client_no], O_WRONLY);

    if (write_fd == -1) {
        perror("open server_to_client");
        close(read_fd);
        exit(1);
    }

    printf("Handler %d started for Client %d\n",
           client_no + 1, client_no + 1);

    while (1) {

        memset(message, 0, SIZE);

        int n = read(read_fd, message, SIZE - 1);

        if (n <= 0)
            break;

        message[n] = '\0';

        printf("Client %d: %s\n",
               client_no + 1, message);

        if (strcmp(message, "exit") == 0) {

            snprintf(response, SIZE,
                     "Server: Client %d disconnected",
                     client_no + 1);

            write(write_fd, response, strlen(response) + 1);

            break;
        }

        snprintf(response, SIZE,
                 "Server received from Client %d: %s",
                 client_no + 1, message);

        write(write_fd, response, strlen(response) + 1);
    }

    close(read_fd);
    close(write_fd);

    printf("Handler %d terminated.\n", client_no + 1);

    exit(0);
}

int main()
{
    struct sigaction sa1, sa2, sa3;

    printf("SERVER STARTED\n");

    /* Create FIFOs */
    for (int i = 0; i < MAX_CLIENTS; i++) {

        unlink(c2s[i]);
        unlink(s2c[i]);

        if (mkfifo(c2s[i], 0666) == -1) {
            perror("mkfifo");
            exit(1);
        }

        if (mkfifo(s2c[i], 0666) == -1) {
            perror("mkfifo");
            exit(1);
        }
    }

    /* SIGUSR1 */
    memset(&sa1, 0, sizeof(sa1));
    sa1.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa1, NULL);

    /* SIGINT */
    memset(&sa2, 0, sizeof(sa2));
    sa2.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa2, NULL);

    /* SIGCHLD */
    memset(&sa3, 0, sizeof(sa3));
    sa3.sa_handler = handle_sigchld;
    sigaction(SIGCHLD, &sa3, NULL);

    printf("Six FIFOs created.\n");
    printf("Waiting for clients...\n");

    /* Create 3 handler processes */
    for (int i = 0; i < MAX_CLIENTS; i++) {

        children[i] = fork();

        if (children[i] == -1) {
            perror("fork");
            exit(1);
        }

        if (children[i] == 0) {
            client_handler(i);
        }
    }

    printf("All client handlers created.\n");
    printf("Server PID: %d\n", getpid());

    /* Main server process */
    while (!shutdown_server) {
        sleep(1);
    }

    printf("\nSIGINT received.\n");
    printf("Shutting down server...\n");

    /* Terminate handler processes */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        kill(children[i], SIGTERM);
    }

    /* Wait for children */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        waitpid(children[i], NULL, 0);
    }

    /* Remove FIFOs */
    for (int i = 0; i < MAX_CLIENTS; i++) {
        unlink(c2s[i]);
        unlink(s2c[i]);
    }
    printf("All FIFOs removed.\n");
    printf("Server terminated.\n");

    return 0;
}
