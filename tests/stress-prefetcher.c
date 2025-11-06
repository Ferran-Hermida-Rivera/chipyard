#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifndef CACHE_BLOCK_BYTES
#define CACHE_BLOCK_BYTES 64UL
#endif

#define NUM_STREAMS      2
#define NUM_BLOCKS       4*64 /*4ways*4sets in Medium Boom*/
#define CACHE_SIZE       NUM_BLOCKS*CACHE_BLOCK_BYTES 
#define CACHE_WORDS      CACHE_SIZE/sizeof(uint32_t)
#define BLOCK_WORDS      CACHE_BLOCK_BYTES/sizeof(uint32_t)
#define WORDS_PER_STREAM 512 /*32 blocks*/

#define S(s) #s
#define XS(s) S(s)
#define FENCE __asm volatile("fence.i")
#define CCSR_WRITE(reg, val) ({    \
        __asm volatile("csrw " XS(reg) ", %0" : : "r"(val)); })
#define CSR_PENABLE 0x830
// Multiple streams to give prefetcher time to prefetch all lines
static volatile uint64_t results[2];
static volatile uint32_t sink;

static inline uint64_t rdcycle(void)
{
    uint64_t c;
    asm volatile("rdcycle %0" : "=r"(c));
    return c;
}

//static void fill_data(void)
//{
//    for (size_t s = 0; s < NUM_STREAMS; ++s) {
//        for (size_t i = 0; i < WORDS_PER_STREAM; ++i) {
//            data[s][i] = (uint32_t)(s + i);
//        }
//    }
//}

//static void evict_l1(void)
//{
//    // Each access should evict one block, therefore LINE_WORDS
//    for (size_t i = 0; i < CACHE_WORDS; i += BLOCK_WORDS) {
//        uint32_t tmp = evict[i];
//        evict[i] = tmp + 1;
//    }
//}

//static uint64_t stream_sweep(void)
//{
//    uint64_t sums[NUM_STREAMS] = {0};
//    uint64_t start = rdcycle();

//    for (size_t base = 0; base < WORDS_PER_STREAM; base += LINE_WORDS) {
//        for (size_t lane = 0; lane < LINE_WORDS; ++lane) {
//            for (size_t s = 0; s < NUM_STREAMS; ++s) {
//                sums[s] += data[s][base + lane];
//            }
//        }
//    }

//    uint64_t end = rdcycle();
//    for (size_t s = 0; s < NUM_STREAMS; ++s) {
//        sink ^= (uint32_t)sums[s];
//    }
//    return end - start;
//}
volatile uint64_t acc;
int main(void)
{
    static volatile uint32_t data_a[NUM_STREAMS][WORDS_PER_STREAM] __attribute__((aligned(CACHE_BLOCK_BYTES)));
    static volatile uint32_t data_b[NUM_STREAMS][WORDS_PER_STREAM] __attribute__((aligned(CACHE_BLOCK_BYTES)));

    uint64_t for_start = rdcycle();
    for (size_t base = 0; base < WORDS_PER_STREAM; ++base) {
        data_a[0][base] = base; //0xCAFE/(base+1);
        data_a[1][base] = base;
    }
    uint64_t for_end = rdcycle();
    printf("With prefetcher: %ld\n", for_end - for_start);

    CCSR_WRITE(CSR_PENABLE, 0x0);
    FENCE;
    for_start = rdcycle();
    for (size_t base = 0; base < WORDS_PER_STREAM; ++base) {
        data_b[0][base] = base; //0xCAFE/(base+1);
        data_b[1][base] = base;
    }
    for_end = rdcycle();
    printf("Without prefetcher: %ld\n", for_end - for_start);

    for (size_t base = 0; base < WORDS_PER_STREAM; ++base) {
        acc += data_a[0][base] + data_b[1][base];
    }

    //fill_data();

    //evict_l1();
    //results[0] = stream_sweep();   /* first measured sweep */

    //evict_l1();
    //results[1] = stream_sweep();   /* second sweep for stability */

    //return 0;
}

