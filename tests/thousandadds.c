
#include <stdio.h>

#define TEN_ADDS "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n" \
    "add t0, t0, t1\n"

#define HUNDRED_ADDS TEN_ADDS TEN_ADDS TEN_ADDS TEN_ADDS TEN_ADDS \
    TEN_ADDS TEN_ADDS TEN_ADDS TEN_ADDS TEN_ADDS

#define THOUSAND_ADDS HUNDRED_ADDS HUNDRED_ADDS HUNDRED_ADDS HUNDRED_ADDS HUNDRED_ADDS \
    HUNDRED_ADDS HUNDRED_ADDS HUNDRED_ADDS HUNDRED_ADDS HUNDRED_ADDS

int main(void)
{
    printf("Start adding 500 to itself 1000 times\n");

    int sum = 0;
    int constant = 500;
    asm volatile(
        "fence.i\n"
        "li t0, 0\n"
        "mv t1, %1\n"
        THOUSAND_ADDS
        "mv %0, t0\n"
        : "=r" (sum)
        : "r" (constant)
    );

    printf("Result: %d\n", sum);

    int read = -1;
    int write = 0;
    asm volatile(
        "csrw 0x830, %1\n"
        "csrr %0, 0x830"
        : "=r" (read)
        : "r" (write)
    );
    printf("read back from the CSR: %x\n", read);

    sum = 0;
    constant = 500;
    asm volatile(
        "fence.i\n"
        "li t0, 0\n"
        "mv t1, %1\n"
        THOUSAND_ADDS
        "mv %0, t0\n"
        : "=r" (sum)
        : "r" (constant)
    );

    printf("Result: %d\n", sum);

}