#include <stdint.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

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
        asm volatile("fence");
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
    unsigned long long start_0[MAX_PMU_COUNT];
    unsigned long long end_0[MAX_PMU_COUNT];

    unsigned long long warm_up[MAX_PMU_COUNT];
    
    // warm up icache
    measure_cycles(data_x, warm_up, warm_up);


    #ifdef NOPREFETCH
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NO_DCACHE_PREFETCHERS);
    
    printf("Ubenchmark: StrideArraySum\nKnobConfig: D$ Prefetch Disabled\n");
    #endif

    #ifdef PREFETCH0
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NL_DCACHE_PREFETCHERS);
    
    printf("KnobConfig: D$ NL Prefetch Enabled\n");
    #endif

    #ifdef PREFETCH1
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, MULTINL_DCACHE_PREFETCHERS);
    
    printf("KnobConfig: D$ MultiNL Prefetch Enabled\n");
    #endif

    #ifdef PREFETCH2
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, STRIDED_DCACHE_PREFETCHERS);

    printf("KnobConfig: D$ Strided Prefetch Enabled\n");
    #endif

    #ifdef PREFETCH3
    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, LOCALIZEDSTRIDED_DCACHE_PREFETCHERS);
    
    printf("KnobConfig: D$ LocalizedStrided Prefetch Enabled\n");
    #endif

    measure_cycles(data_a, start_0, end_0);
    dump_counters_stored(4, 4, 8, start_0, end_0);

    return 0;
}
