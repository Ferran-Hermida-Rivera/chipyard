
#include <stdio.h>
#include "pmu.h"
#include "riscv.h"
#include "csr_defs.h"
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
    unsigned long long start_0[MAX_PMU_COUNT];
    unsigned long long end_0[MAX_PMU_COUNT];

    unsigned long long start_1[MAX_PMU_COUNT];
    unsigned long long end_1[MAX_PMU_COUNT];

    printf("Config perf counters\n");
    config();
    
    printf("=====================================\n");
    printf("ICache Prefetch Enabled\n");
    printf("=====================================\n\n");

    WRITE_CUSTOM_CSR(CSR_ICACHE_PREFETCHERS, ENABLE);

    printf("Start adding 500 to itself 2000 times\n");

    store_counter(start_0); // read perf counters

    int sum = 0;
    int constant = 500;
    asm volatile(
        "fence.i\n"
        "li t0, 0\n"
        "mv t1, %1\n"
        THOUSAND_ADDS
        THOUSAND_ADDS
        "mv %0, t0\n"
        : "=r" (sum)
        : "r" (constant)
    );

    store_counter(end_0);

    printf("Result: %d\n", sum);

    printf("=====================================\n");
    printf("ICache Prefetch Disabled\n");
    printf("=====================================\n\n");

    WRITE_CUSTOM_CSR(CSR_ICACHE_PREFETCHERS, DISABLE);

    printf("Start adding 500 to itself 2000 times\n");

    store_counter(start_1); // read perf counters

    sum = 0;
    constant = 500;
    asm volatile(
        "fence.i\n"
        "li t0, 0\n"
        "mv t1, %1\n"
        THOUSAND_ADDS
        THOUSAND_ADDS
        "mv %0, t0\n"
        : "=r" (sum)
        : "r" (constant)
    );

    store_counter(end_1);

    printf("Result: %d\n", sum);

    printf("Ubenchmark: Thousandadds\n");
    printf("KnobConfig: I$ Prefetch Enabled\n");
    dump_counters_stored(4, 4, 8, start_0, end_0); // configs are specific to megaboom

    printf("KnobConfig: I$ Prefetch Disabled\n");
    dump_counters_stored(4, 4, 8, start_1, end_1); // configs are specific to megaboom

}