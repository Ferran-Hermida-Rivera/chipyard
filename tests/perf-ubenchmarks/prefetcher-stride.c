#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "pmu.h"
#include "riscv.h"
#include "csr_defs.h"
#include "pmu_defs.h"

#define UNITS "cycles"

volatile uint64_t acc;

#define STRIDE_IN_BYTES 256
typedef struct strider
{
    uint32_t m_data [STRIDE_IN_BYTES/sizeof(uint32_t)];
} strider_t;

static void measure_cycles(volatile strider_t arr[256],
                            unsigned long long start[MAX_PMU_COUNT],
                            unsigned long long end[MAX_PMU_COUNT])
{
    acc = 0;
    ssize_t f = 0;
    SUPER_BARRIER();
    store_counter(start);
    for (ssize_t i = 0; i < 256; ++i) {
        f += arr[i].m_data[0];
    }
    store_counter(end);
    SUPER_BARRIER();
    acc = f;
}

int main(void) {
    printf("Begin execution\n");

    config(); 
    // store_counter take 300-400 cycles, need to amortize it over data size
    static volatile strider_t data_x[256] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t pad2[16] __attribute__ ((aligned(64)));
    static volatile strider_t data_a[256] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t pad1[16] __attribute__ ((aligned(64)));
    static volatile strider_t data_b[256] __attribute__ ((aligned(64))); // Base at 0x80003000
    unsigned long long start_0[MAX_PMU_COUNT];
    unsigned long long end_0[MAX_PMU_COUNT];

    unsigned long long start_1[MAX_PMU_COUNT];
    unsigned long long end_1[MAX_PMU_COUNT];
    
    ssize_t warm_up, cycle_start_a, cycle_end_a, cycle_start_b, cycle_end_b;

    // warm up icache
    measure_cycles(data_x, &warm_up, &warm_up);


    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NO_DCACHE_PREFETCHERS);
    measure_cycles(data_a, start_0, end_0);

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, STRIDE_DCACHE_PREFETCHERS);
    measure_cycles(data_b, start_1, end_1);

    printf("Ubenchmark: Stride Prefetcher\n");
    printf("KnobConfig: D$ Prefetch Disabled\n");
    dump_counters_stored(4, 4, 8, start_0, end_0);
    printf("KnobConfig: D$ Stride Prefetch Enabled\n");
    dump_counters_stored(4, 4, 8, start_1, end_1);

    return 0;
}
