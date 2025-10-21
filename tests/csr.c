
#include <stdio.h>

int main(void)
{

    printf("Start CSR Test.\n");

    int read = 0;
    asm volatile(
        "csrr %0, 0x810"
        : "=r" (read)
    );
    printf("Value read: %x\n", read);

    int write = 0xdcba;
    asm volatile(
        "csrw 0x810, %1\n"
        "csrr %0, 0x810"
        : "=r" (read)
        : "r" (write)
    );

    printf("Value written: %x\n", write);
    printf("Value read: %x\n", read);


    printf("End CSR Test.\n");


    


}