#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "pmu.h"
#include "riscv.h"
#include "csr_defs.h"
#include "pmu_defs.h"

#define UNITS "cycles"

volatile uint64_t acc;

#define STRIDE_IN_BYTES 192
typedef struct strider
{
    uint32_t m_data [STRIDE_IN_BYTES/sizeof(uint32_t)];
} strider_t;

#define STRIDE2_IN_BYTES 64
typedef struct strider2
{
    uint32_t m_data [STRIDE2_IN_BYTES/sizeof(uint32_t)];
} strider2_t;

static void measure_cycles(volatile strider_t arr[128], volatile strider2_t arr2[128],
                              ssize_t *cycle_start, ssize_t *cycle_end)
{
    acc = 0;
    ssize_t f = 0;
    SUPER_BARRIER();
    *cycle_start = rdcycle();
    for (ssize_t i = 0; i < 128; ++i) {
        f += arr[i].m_data[0];
        f += arr2[i].m_data[0];
    }
    *cycle_end = rdcycle();
    SUPER_BARRIER();
    acc = f;
}

int main(void) {
    printf("Begin execution\n");

    volatile strider_t data_x[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider2_t data2_x[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider_t data_y[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider2_t data2_y[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider_t data_z[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider2_t data2_z[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t pad2[16] __attribute__ ((aligned(64)));
    volatile strider_t data_a[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider2_t data2_a[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t pad1[16] __attribute__ ((aligned(64)));
    volatile strider_t data_b[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile strider2_t data2_b[128] __attribute__ ((aligned(64))); // Base at 0x80003000
    
    ssize_t warm_up, cycle_start_a, cycle_end_a, cycle_start_b, cycle_end_b;

    // warm up 
    measure_cycles(data_x, data2_x, &warm_up, &warm_up);
    measure_cycles(data_y, data2_y, &warm_up, &warm_up);
    measure_cycles(data_z, data2_z, &warm_up, &warm_up);

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, NO_DCACHE_PREFETCHERS);
    measure_cycles(data_a, data2_a, &cycle_start_a, &cycle_end_a);

    WRITE_CUSTOM_CSR(CSR_DCACHE_PREFETCHERS, LOCALIZEDSTRIDED_DCACHE_PREFETCHERS);
    measure_cycles(data_b, data2_b, &cycle_start_b, &cycle_end_b);
    
    printf("Prefetcher.c execution %s: %ld\n", UNITS, (long)(cycle_end_a - cycle_start_a));
    printf("Prefetcher.c execution %s: %ld\n", UNITS, (long)(cycle_end_b - cycle_start_b));

    return 0;
}
