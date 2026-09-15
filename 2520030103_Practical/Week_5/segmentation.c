#include<stdio.h>

int main() {
    int *ptr = NULL;
    int x = 25;
    ptr = &x;
    printf("x = %d \n", x);
    printf("address of x = %u \n", &x);
    printf("value of ptr = %u \n", ptr);
    printf("value at ptr = %d \n", *ptr);
    printf("address of ptr = %u \n", &ptr);
    return 0;
}
