#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <riscv-pk/encoding.h>
#include "marchid.h"

#define OBUS_OFFSET (0x1L << 32)

#define STRINGIFY_HELPER(s) #s
#define STRINGIFY(s) STRINGIFY_HELPER(s)
#define BASE 0x2010000
#define OFFSET 0x100

static inline void write_policy(uint64_t val) {
    
     asm volatile (
          "sw %[val], %[offset](%[base])"
          :
          : [val] "r" (val),
            [offset] "i" (OFFSET),
            [base] "r" ((uint64_t)BASE)
          : "memory");
}

static inline uint64_t read_policy() {
    ssize_t val;
    asm volatile (
        "lw %[val], %[offset](%[base])"
        : [val] "=r" (val)
        : [offset] "i" (OFFSET),
          [base] "r" ((uint64_t)BASE)
        : "memory");
    return val;
}

static inline void super_barrier() {
    asm volatile("fence iorw, iorw" ::: "memory");
    asm volatile("fence.i" ::: "memory");
}

int main(void) {
    /***************************************
    So far, the implemented policies are:
0: Random victim selection
1: Code priority. Implemented pseudo-randomly
2: Code priority. Really randomly data victim selection
any other value will fall to default, random victim sel
    ***************************************/

    ssize_t policy = read_policy();
    ssize_t new_policy;
    printf("policy value %d\n", policy);
    super_barrier();
    new_policy = policy+1;
    write_policy(new_policy);
    super_barrier();
    policy = read_policy();
    super_barrier();
    printf("new policy value should be %d and is %d\n", new_policy, policy);
 
    return 0;
}
