#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t producer, consumer;

    // Create anonymous pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create Producer process
    producer = fork();

    if (producer == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (producer == 0) {
        // Producer: ls -l

        // Close unused read end
        close(pipefd[0]);

        // Redirect stdout to pipe write end
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Close original write descriptor
        close(pipefd[1]);

        // Replace process image with ls -l
        execlp("ls", "ls", "-l", (char *)NULL);

        // Only reached if execlp fails
        perror("execlp ls");
        exit(EXIT_FAILURE);
    }

    // Create Consumer process
    consumer = fork();

    if (consumer == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (consumer == 0) {
        // Consumer: grep ".c"

        // Close unused write end
        close(pipefd[1]);

        // Redirect stdin to pipe read end
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(EXIT_FAILURE);
        }

        // Close original read descriptor
        close(pipefd[0]);

        // Replace process image with grep ".c"
        execlp("grep", "grep", ".c", (char *)NULL);

        // Only reached if execlp fails
        perror("execlp grep");
        exit(EXIT_FAILURE);
    }

    // Parent/Supervisor closes both pipe ends
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for producer
    waitpid(producer, NULL, 0);

    // Wait for consumer
    waitpid(consumer, NULL, 0);

    printf("Producer and Consumer processes completed.\n");

    return 0;
}
