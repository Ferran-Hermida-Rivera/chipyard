#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#ifdef __linux__
#include <time.h>
#else
#endif

#ifdef __linux__
#define UNITS "ns"

static inline void write_csr_834(uint64_t val) { (void)val; }
static inline ssize_t read_csr_834(void) { return -1; }
static inline uint64_t rdcycle(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
}
//static inline void fence_i(void) { asm volatile ("" ::: "memory"); }
//static inline void super_barrier(void) { asm volatile ("" ::: "memory"); }
#else
#define UNITS "cycles"

static inline void write_csr_834(uint64_t val) {
    asm volatile ("csrw 0x834, %0" : : "r"(val));
}

static inline ssize_t read_csr_834(void) {
    ssize_t read = -1;
    asm volatile("csrr %0, 0x834" : "=r" (read));
    return read;
}

static inline uint64_t rdcycle(void)
{
    uint64_t value;
    asm volatile ("rdcycle %0" : "=r"(value));
    return value;
}
#endif

static inline void fence_i(void) {
    asm volatile ("fence.i");
}

static inline void super_barrier(void) {
    asm volatile("fence iorw, iorw" ::: "memory");
    asm volatile("fence.i" ::: "memory");
}

volatile uint64_t acc;

static void measure_cycles(volatile uint32_t arr[2048],
                              ssize_t *cycle_start, ssize_t *cycle_end)
{
    acc = 0;
    ssize_t f = 0;
    super_barrier();
    *cycle_start = rdcycle();
    for (ssize_t i = 0; i < 2048; ++i) {
        f += arr[i];
    }
    *cycle_end = rdcycle();
    super_barrier();
    acc = f;
}

int main(void) {

    volatile uint32_t data_x[2048] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_y[2048] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t data_z[2048] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t pad2[16] __attribute__ ((aligned(64)));
    volatile uint32_t data_a[2048] __attribute__ ((aligned(64))); // Base at 0x80003000
    volatile uint32_t pad1[16] __attribute__ ((aligned(64)));
    
    ssize_t cycle_start_a, cycle_end_a, cycle_start_b, cycle_end_b;

    //initialize_vector(data_x, 2048);
    //initialize_vector(data_y, 2048);
    //initialize_vector(data_z, 2048);
    //initialize_vector(data_a, 2048);

    measure_cycles(data_x, &cycle_start_a, &cycle_end_a);
    measure_cycles(data_y, &cycle_start_a, &cycle_end_a);
    measure_cycles(data_z, &cycle_start_a, &cycle_end_a);

    measure_cycles(data_a, &cycle_start_a, &cycle_end_a);
    printf("Prefetcher.c execution %s: %ld\n", UNITS, (long)(cycle_end_a - cycle_start_a));

    return 0;
}
