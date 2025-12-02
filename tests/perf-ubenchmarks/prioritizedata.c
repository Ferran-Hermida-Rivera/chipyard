
#include <stdint.h>
#include <stdio.h>
#include "pmu_defs.h"

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

    printf("Begin bench\n");

    store_counter(start);
    initBigArray();
    muchCode();
    uint64_t sum1 = bigArraySum();
    store_counter(end);

    printf("First sum: %lu\n", sum1);

    printf("Ubenchmark: prioritizeData\n");
    printf("KnobConfig: Prioritize Data\n");
    dump_counters_stored(4, 4, 8, start, end); // configs are specific to megaboom

    // printf("KnobConfig: Prioritize Code\n");
    // dump_counters_stored(4, 4, 8, start_1, end_1); // configs are specific to megaboom


    // Then when the new code accesses the old data, performance should be different

}
