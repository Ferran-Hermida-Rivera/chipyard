#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "pmu.h"
#include "riscv.h"
#include "csr_defs.h"
#include "pmu_defs.h"

#ifdef __linux__
#include <time.h>
#else
#endif

volatile uint64_t acc;
#define SIZE 2048

static void measure_cycles(volatile uint32_t arr[SIZE],
                              unsigned long long start[MAX_PMU_COUNT],
                            unsigned long long end[MAX_PMU_COUNT],
                        ssize_t arr_size)
{
    acc = 0;
    ssize_t f = 0;
    SUPER_BARRIER();
    store_counter(start);
    for (ssize_t i = 0; i < arr_size; ++i) {
        f += arr[i];
    }
    store_counter(end);
    SUPER_BARRIER();
    acc = f;
}

int main(void) {

    volatile uint32_t data_0[SIZE] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_1[SIZE] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_2[SIZE] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_3[SIZE] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_warmup[SIZE] __attribute__ ((aligned(64))); // Base at 0x80003000
    
    unsigned long long warmup[MAX_PMU_COUNT];
    
    unsigned long long start_0[MAX_PMU_COUNT];
    unsigned long long end_0[MAX_PMU_COUNT];

    unsigned long long start_1[MAX_PMU_COUNT];
    unsigned long long end_1[MAX_PMU_COUNT];

    unsigned long long start_2[MAX_PMU_COUNT];
    unsigned long long end_2[MAX_PMU_COUNT];

    unsigned long long start_3[MAX_PMU_COUNT];
    unsigned long long end_3[MAX_PMU_COUNT];

    config();

    measure_cycles(data_warmup, warmup, warmup, 1);

    #ifdef P0
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NO_DCACHE_PREFETCHERS);
    
    printf("Ubenchmark: SequentialArraySum\nKnobConfig: D$ Prefetch Disabled\n");
    #endif

    #ifdef P1
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NL_DCACHE_PREFETCHERS);
    
    printf("KnobConfig: D$ NL Prefetch Enabled\n");
    #endif

    #ifdef P2
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, MULTINL_DCACHE_PREFETCHERS);
    
    printf("KnobConfig: D$ MultiNL Prefetch Enabled\n");
    #endif

    #ifdef P3
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, LOCALIZEDSTRIDED_DCACHE_PREFETCHERS);
    
    printf("KnobConfig: D$ LocalizedStrided Prefetch Enabled\n");
    #endif

    measure_cycles(data_0, start_0, end_0, SIZE);
    dump_counters_stored(4, 4, 8, start_0, end_0);

    return 0;
}
