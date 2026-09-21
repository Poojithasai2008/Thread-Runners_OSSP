#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main()
{
    int fd1, fd2;
    int *marks1, *marks2;

    // Create shared memory for Student 1
    fd1 = shm_open("/student1_marks", O_CREAT | O_RDWR, 0666);

    // Create shared memory for Student 2
    fd2 = shm_open("/student2_marks", O_CREAT | O_RDWR, 0666);

    // Set size for both shared memory regions
    ftruncate(fd1, 5 * sizeof(int));
    ftruncate(fd2, 5 * sizeof(int));

    // Map shared memory into Teacher's address space
    marks1 = mmap(NULL, 5 * sizeof(int),
                  PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);

    marks2 = mmap(NULL, 5 * sizeof(int),
                  PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);

    // Enter marks for Student 1
    printf("Enter marks for Student 1:\n");

    for (int i = 0; i < 5; i++)
    {
        printf("Subject %d: ", i + 1);
        scanf("%d", &marks1[i]);
    }

    // Enter marks for Student 2
    printf("\nEnter marks for Student 2:\n");

    for (int i = 0; i < 5; i++)
    {
        printf("Subject %d: ", i + 1);
        scanf("%d", &marks2[i]);
    }

    printf("\nMarks successfully written to shared memory.\n");
    printf("Student processes can now read their marks.\n");

    // Wait for both students to finish
    printf("\nPress Enter after both students finish...");
    getchar();
    getchar();

    // Unmap shared memory
    munmap(marks1, 5 * sizeof(int));
    munmap(marks2, 5 * sizeof(int));

    // Close shared memory
    close(fd1);
    close(fd2);

    // Delete shared memory objects
    shm_unlink("/student1_marks");
    shm_unlink("/student2_marks");

    printf("Shared memory cleaned up.\n");

    return 0;
}
