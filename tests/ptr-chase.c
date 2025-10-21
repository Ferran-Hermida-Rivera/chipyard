// ptr-chase.c

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

// ----- Tunables (compile-time) -----------------------------------------------
// Number of nodes in the working set. Each node ~64B => working set ~ NODES*64.
#ifndef PC_NODES
#define PC_NODES   64u  // ~16 MiB working set (adjust to taste)
#endif

// Optional: If you're on RISC-V, define PC_RISCV to record cycle counts via rdcycle.
#ifdef PC_RISCV
static inline uint64_t rdcycle(void){
    uint64_t v;
    asm volatile ("rdcycle %0" : "=r"(v));
    return v;
}
#endif

// ----- Node definition --------------------------------------------------------
typedef struct Node {
    struct Node* next;
    char pad[64 - sizeof(void*)];   // pad to a 64B cache line
} __attribute__((aligned(64))) Node;

// ----- Globals visible to the testbench --------------------------------------
volatile uint64_t pc_checksum = 0;     // final reduction over visited pointers
volatile uint64_t pc_cycles   = 0;     // cycles taken (if PC_RISCV defined)

// ----- Simple xorshift64* PRNG (deterministic, no libc) ----------------------
static inline uint64_t xs64(uint64_t *s){
    uint64_t x = *s;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    *s = x;
    return x * 0x2545F4914F6CDD1DULL;
}

// ----- Storage (static, no malloc) -------------------------------------------
static Node nodes[PC_NODES];
static uint32_t perm[PC_NODES];   // index permutation for wiring the ring

// ----- Build a random permutation ring over nodes[] --------------------------
static void build_ring(void) {
    // 1) init indexes
    for (uint32_t i = 0; i < PC_NODES; ++i) {
        perm[i] = i;
        nodes[i].next = 0;  // touch memory; useful on some sims/targets
    }
    // 2) Fisher–Yates with xorshift RNG (fixed seed for repeatability)
    uint64_t seed = 0xdeadbeefcafebabeULL;
    for (uint32_t i = PC_NODES - 1; i > 0; --i) {
        uint64_t r = xs64(&seed);
        uint32_t j = (uint32_t)(r % (uint64_t)(i + 1));
        uint32_t tmp = perm[i]; perm[i] = perm[j]; perm[j] = tmp;
    }
    // 3) Link nodes according to the permutation into a single cycle
    for (uint32_t i = 0; i < PC_NODES; ++i) {
        uint32_t ni = perm[(i + 1) % PC_NODES];
        nodes[perm[i]].next = &nodes[ni];
    }
}

// ----- Main pointer-chase -----------------------------------------------------
int main(void) {
    printf("Building nodes...");
    build_ring();

    // head of the ring
    Node* volatile p = &nodes[perm[0]];

    // Optional cycle timing (RISC-V). Omit on other ISAs.
    printf("Starting execution");
#ifdef PC_RISCV
    uint64_t t0 = rdcycle();
#endif

    // Tight dependency chain: one true dereference per iteration.
    uint64_t acc = 0;
    for (uint32_t i = 0; i < PC_NODES; ++i) {
        p = p->next;                 // dependent load
        // mix pointer bits to keep the result "observable"
        acc ^= (uint64_t)(uintptr_t)p >> 6;
    }

#ifdef PC_RISCV
    uint64_t t1 = rdcycle();
    pc_cycles = t1 - t0;
#endif

    printf("Finished run");
    // publish checksum so the loop can't be optimized away
    pc_checksum = acc ^ (uint64_t)(uintptr_t)p;

    // On bare metal we just return; the testbench can read pc_checksum/pc_cycles.
    return 0;
}

