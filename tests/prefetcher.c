#include <stdint.h>
#include <stdio.h>  // For printf
#include <stddef.h> // For ssize_t
#include "pmu_defs.h"

/*
 * --- Global Data Definitions (Inferred from Assembly) ---
 *
 * The disassembly implies three global variables:
 * 1. s0 (callee-saved) is set to 0x80003000, labeled <data_a.1>
 * 2. s1 (callee-saved) is set to 0x80002000, labeled <data_b.0>
 * 3. A 64-bit variable at 0x80001f80 (labeled <acc>) is accessed
 * relative to the global pointer (gp).
 *
 * The access pattern [i] and [i + 512] strongly suggests
 * a 2D array of [2][512].
 */

/*
 * --- Inline Assembly Helper Functions (Inferred) ---
 */

// 8000024c: c00025f3 rdcycle a1
//static inline uint64_t rdcycle(void) {
//    uint64_t v;
//    asm volatile ("rdcycle %0" : "=r"(v));
//    return v;
//}

// 800002a0: 83479073 csrw 0x834,a5
static inline void write_csr_834(uint64_t val) {
    asm volatile ("csrw 0x834, %0" : : "r"(val));
}

static inline ssize_t read_csr_834() {
    ssize_t read = -1;
     asm volatile(
        "csrr %0, 0x834"
        : "=r" (read)
    );
     return read;
}

// 800002a4: 0000100f fence.i
static inline void fence_i(void) {
    asm volatile ("fence.i");
}

/* * The code calls <iprintf> at 8000037a.
 * We'll use a standard printf as the C-level equivalent.
 */

static inline void super_barrier() {
    asm volatile("fence iorw, iorw" ::: "memory");
    asm volatile("fence.i" ::: "memory");
}
// Helper to measure stores to a 2D array with PMU
static void measure_store_init(volatile uint32_t arr[2][2048],
                              unsigned long long start[], unsigned long long end[]) {
    super_barrier();
    store_start(start);
    for (ssize_t i = 0; i < 2048; ++i) {
        arr[0][i] = i;
        arr[1][i] = i;
    }
    store_end(end);
    super_barrier();
}

/*
 * --- Reconstructed main() Function ---
 */
int main(void) {

    static volatile uint32_t data_x[2][2048] __attribute__ ((aligned(64))); // Base at 0x80003000
    static volatile uint32_t data_a[2][2048] __attribute__ ((aligned(64))); // Base at 0x80003000
    static volatile uint32_t data_b[2][2048] __attribute__ ((aligned(64))); // Base at 0x80002000
    static volatile int64_t  acc;            // Base at 0x80001f80
    uint64_t cycle_i, cycle_e;

    unsigned long long start_x[MAX_PMU_COUNT];
    unsigned long long end_x[MAX_PMU_COUNT];
    unsigned long long start_a[MAX_PMU_COUNT];
    unsigned long long end_a[MAX_PMU_COUNT];
    unsigned long long start_b[MAX_PMU_COUNT];
    unsigned long long end_b[MAX_PMU_COUNT];
    //fence_i();
    //printf("prefethcer value: %ld\n", read_csr_834());
    //printf("prefethcer value: %ld\n", read_csr_834());
    //printf("prefethcer value: %ld\n", read_csr_834());
    //fence_i();
    config();
    //dump_config();
    measure_store_init(data_x, start_x, end_x);
    
    write_csr_834(0);
    measure_store_init(data_a, start_a, end_a);

    write_csr_834(1);
    measure_store_init(data_b, start_b, end_b);


    printf("=====================================\n");
    printf("DCache Prefetch Disabled\n");
    printf("=====================================\n\n");
    dump_counters_stored(2, 2, 4, start_a, end_a);


    printf("=====================================\n");
    printf("DCache Prefetch Disabled\n");
    printf("=====================================\n\n");
    dump_counters_stored(2, 2, 4, start_b, end_b);

    //fence_i();
    //
    //printf("Execution cycles (data_b init): %ld\n", (uint64_t)(cycle_e - cycle_i));

    //for (ssize_t i = 0; i < 512; ++i) {
    //    int32_t val_a = data_a[0][i];
    //    int32_t val_b = data_b[1][i];
    //    int64_t current_acc = acc;
    //    int32_t sum_32bit = val_a + val_b;
    //    int64_t sum_64bit = (int64_t)sum_32bit;
    //    acc = current_acc + sum_64bit;
    //}

    return 0;
}
