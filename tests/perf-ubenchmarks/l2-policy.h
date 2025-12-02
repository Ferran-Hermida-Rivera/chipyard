
#ifndef L2_POLICY_H
#define L2_POLICY_H

#include <stdint.h>
#include <stdio.h>

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

#endif // L2_POLICY_H