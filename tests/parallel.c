#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef NUM_ELEM
#define NUM_ELEM 2048
#endif

#define CSR_PENABLE 0x830
// Two level helper macros to expand macros before stringifying them
#define S(s) #s
#define XS(s) S(s)

#define CCSR_READ(reg) ({ \
        unsigned long __value;\
        __asm volatile("csrr %0, " XS(reg) \
                       : "=r"(__value));   \
        __value; })

#define CCSR_WRITE(reg, val) ({    \
        __asm volatile("csrw " XS(reg) ", %0" : : "r"(val)); })

volatile static uint32_t sink;

#ifndef CACHE_BLOCK_BYTES
#define CACHE_BLOCK_BYTES 64UL
#endif

#define FENCE __asm volatile("fence.i")

static inline uint64_t rdcycle(void){
    uint64_t v;
    asm volatile ("rdcycle %0" : "=r"(v));
    return v;
}


int main(void)
{
    volatile uint32_t data_B[NUM_ELEM];
    volatile uint32_t data_dealign[32];
    volatile uint32_t data_A[NUM_ELEM];
    uint32_t acc = 0;

    // Access to A with prefetcher active
    uint64_t for_start = rdcycle();
    for (uint32_t i = 0; i < NUM_ELEM; ++i) {
        data_A[i] = 0xCAFE/(i+1);
    }
    uint64_t for_end = rdcycle();
    printf("data_A write cycles: %lu\n", for_end - for_start);

    // Access to B with prefetcher disabled
    CCSR_WRITE(CSR_PENABLE, 0);
    FENCE;
    for_start = rdcycle();
    for (uint32_t i = 0; i < NUM_ELEM; ++i) {
        data_B[i] =  0xCAFE/(i+1);
    }
    for_end = rdcycle();
    printf("data_B write cycles: %lu\n", for_end - for_start);

    // Possibly uselless...
    for (uint32_t i = 0; i < NUM_ELEM; ++i) {
        acc += data_A[i] + data_B[i];
    }

    sink = acc;
    return 0;
}
