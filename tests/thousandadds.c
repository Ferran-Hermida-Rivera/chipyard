
#include <stdio.h>
#include "pmu_defs.h"

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
    printf("Config perf counters\n");

    config();
    dump_config();
    
    printf("=====================================\n");
    printf("ICache Prefetch Enabled\n");
    printf("=====================================\n\n");

    printf("Start adding 500 to itself 1000 times\n");

    read_start(); // read perf counters

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

    read_end();
    dump_counters(4, 4, 8); // configs are specific to megaboom

    printf("Result: %d\n", sum);

    printf("=====================================\n");
    printf("ICache Prefetch Disabled\n");
    printf("=====================================\n\n");

    int read = -1;
    int write = 0;
    asm volatile(
        "csrw 0x830, %1\n"
        "csrr %0, 0x830"
        : "=r" (read)
        : "r" (write)
    );
    printf("read back from the CSR: %x\n", read);

    read_start(); // read perf counters

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

    read_end();
    dump_counters(4, 4, 8); // configs are specific to megaboom

    printf("Result: %d\n", sum);

}