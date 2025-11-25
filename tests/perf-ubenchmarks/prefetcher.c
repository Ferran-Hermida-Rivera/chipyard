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

static void measure_cycles(volatile uint32_t arr[2048],
                              unsigned long long start[MAX_PMU_COUNT],
                            unsigned long long end[MAX_PMU_COUNT])
{
    acc = 0;
    ssize_t f = 0;
    SUPER_BARRIER();
    store_counter(start);
    for (ssize_t i = 0; i < 4096; ++i) {
        f += arr[i];
    }
    store_counter(end);
    SUPER_BARRIER();
    acc = f;
}

int main(void) {

    volatile uint32_t data_0[4096] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_1[4096] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_2[4096] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_3[4096] __attribute__ ((aligned(64))); // Base at 0x80003000
    unsigned long long start_0[MAX_PMU_COUNT];
    unsigned long long end_0[MAX_PMU_COUNT];

    unsigned long long start_1[MAX_PMU_COUNT];
    unsigned long long end_1[MAX_PMU_COUNT];

    unsigned long long start_2[MAX_PMU_COUNT];
    unsigned long long end_2[MAX_PMU_COUNT];

    unsigned long long start_3[MAX_PMU_COUNT];
    unsigned long long end_3[MAX_PMU_COUNT];
    
    ssize_t cycle_start_a, cycle_end_a, cycle_start_b, cycle_end_b;

    config();

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NO_DCACHE_PREFETCHERS);
    measure_cycles(data_0, start_0, end_0);

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NL_DCACHE_PREFETCHERS);
    measure_cycles(data_1, start_1, end_1);

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, MULTINL_DCACHE_PREFETCHERS);
    measure_cycles(data_2, start_2, end_2);

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, LOCALIZEDSTRIDED_DCACHE_PREFETCHERS);
    measure_cycles(data_3, start_3, end_3);

    printf("Ubenchmark: SequentialArraySum\n");

    printf("KnobConfig: D$ Prefetch Disabled\n");
    dump_counters_stored(4, 4, 8, start_0, end_0);
    printf("KnobConfig: D$ NL Prefetch Enabled\n");
    dump_counters_stored(4, 4, 8, start_1, end_1);
    printf("KnobConfig: D$ MultiNL Prefetch Enabled\n");
    dump_counters_stored(4, 4, 8, start_2, end_2);
    printf("KnobConfig: D$ LocalizedStrided Prefetch Enabled\n");
    dump_counters_stored(4, 4, 8, start_3, end_3);

    return 0;
}
