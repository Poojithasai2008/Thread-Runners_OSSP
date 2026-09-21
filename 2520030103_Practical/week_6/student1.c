#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main()
{
    int fd;
    int *marks;
    int total = 0;
    float average;

    // Open Student 1 shared memory
    fd = shm_open("/student1_marks", O_RDONLY, 0666);

    // Map shared memory for reading
    marks = mmap(NULL, 5 * sizeof(int),
                 PROT_READ, MAP_SHARED, fd, 0);

    printf("\nStudent 1\n");
    printf("--------------------------------\n");

    // Read and display marks
    for (int i = 0; i < 5; i++)
    {
        printf("Subject %d : %d\n", i + 1, marks[i]);
        total = total + marks[i];
    }

    // Calculate average
    average = total / 5.0;

    printf("\nTotal   : %d\n", total);
    printf("Average : %.2f\n", average);

    // Unmap shared memory
    munmap(marks, 5 * sizeof(int));

    // Close shared memory
    close(fd);

    return 0;
}
