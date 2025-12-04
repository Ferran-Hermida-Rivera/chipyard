
#include <stdint.h>
#include <stdio.h>
#include "pmu_defs.h"
#include "csr_defs.h"

#include "l2-policy.h"

#define CACHE_SIZE (64 * 1024) // 64 KB
#define ARRAY_SIZE (CACHE_SIZE / sizeof(int))

int bigArray[ARRAY_SIZE];

void * labelAddr;

#define TEN_NOPS "nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;\n"
#define HUNDRED_NOPS TEN_NOPS TEN_NOPS TEN_NOPS TEN_NOPS TEN_NOPS \
                      TEN_NOPS TEN_NOPS TEN_NOPS TEN_NOPS TEN_NOPS

#define KILA_NOP  HUNDRED_NOPS HUNDRED_NOPS HUNDRED_NOPS HUNDRED_NOPS HUNDRED_NOPS \
                   HUNDRED_NOPS HUNDRED_NOPS HUNDRED_NOPS HUNDRED_NOPS HUNDRED_NOPS \
                     TEN_NOPS TEN_NOPS "nop; nop; nop; nop;\n"

#define SIXTEEN_KILA_NOPS KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP \
                          KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP KILA_NOP

void initBigArray()
{
    // This should fill up the cache with data
    for (int i = 0; i < ARRAY_SIZE; i++) {
        bigArray[i] = i;
    }
}

// Tons o code
void muchCode()
{
    // And this should try to fill it up with garbage
    asm volatile (
        SIXTEEN_KILA_NOPS
    );
}

uint64_t bigArraySum()
{
    uint64_t sum = 0;
    for (int i = 0; i < ARRAY_SIZE; i++) {
        sum += bigArray[i];
    }
    return sum;
}

int main(void)
{
    // Need to setup a cache line to have both code and data
    // Then, with data prioritized, go to new code which accesses the same data
    // If data is prioritized, the code should replace old code
    // If code is prioritized, the code should replace old data

    unsigned long long start[MAX_PMU_COUNT];
    unsigned long long end[MAX_PMU_COUNT];
    config();

    ssize_t policy = read_policy();

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NO_DCACHE_PREFETCHERS);
    WRITE_CUSTOM_CSR(CSR_ICACHE_PREFETCHERS, ENABLE);

    // policy0 is all random
    #if defined(POLICY_ZERO) // all random
    printf("Ubenchmark: prioritizeData\n");
    #elif defined(POLICY_ONE) // prioritize code 
    policy = policy + 1;
    #elif defined(POLICY_TWO) // prioritize data
    policy = policy + 2;
    #elif defined(POLICY_THREE) // all random
    policy = policy + 3;
    #endif

    write_policy(policy);
    policy = read_policy();
    printf("Current L2 Policy: %ld\n", policy);

    printf("Begin bench\n");

    initBigArray();
    store_counter(start);
    muchCode();
    uint64_t sum1 = bigArraySum();
    store_counter(end);

    printf("First sum: %lu\n", sum1);

    printf("KnobConfig: Policy %d\n", policy);
    dump_counters_stored(4, 4, 8, start, end); // configs are specific to megaboom

}
