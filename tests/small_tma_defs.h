#ifndef TMA_DEFS_H_
#define TMA_DEFS_H_



#define read_csr_safe(reg) ({ long __tmp; \
    asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
    __tmp; })


#define EN_MASK 0xFFFFFFFF
#define EVT(n) 1UL << (n + 8)
#define MAX_PMU_COUNT 31

#define UNUSED "UNUSED"





// Existing Event Sets
#define EX_EVT 0x0UL
#define UOP_EVT 0x1UL
#define MEM_EVT 0x2UL
// Additional Event Sets
#define TMA_EVT_SINGLE 0x3UL
#define TMA_EVT_COREWIDTH 0x4UL
#define TMA_EVT_RETIRE_WIDTH 0x5UL
#define TMA_EVT_ISSUE_WIDTH 0x6UL
#define COREWIDTH 1

#define MAX_CORE_WIDTH 8

const char* counter_names[MAX_PMU_COUNT] = { \
    "Cycle", \
    "Int Ret", \
    "Branch Mispredict", \
    "Flush", \
    "I$ Miss", \
    "D$ Miss", \
    "I$ Blocked", \
    "Recovering", \
    "D$ Blocked0", \
    "Uops Retired0", \
    "Fence Retired0", \
    "Fetch Bubble0", \
    "Uops Issued0", \
    "Uops Issued1", \
    "Uops Issued2", \
    UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, \
    UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, UNUSED, \
    UNUSED, UNUSED, UNUSED , UNUSED  \
};

#endif /*TMA_DEFS_H_*/