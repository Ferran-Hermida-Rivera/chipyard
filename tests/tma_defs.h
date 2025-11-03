#ifndef TMA_DEFS_H_
#define TMA_DEFS_H_

#define EN_MASK 0xFFFFFFFF
#define EVT(n) 1UL << (n + 8)
#define MAX_PMU_COUNT 31

// Existing Event Sets
#define EX_EVT 0x0UL
#define UOP_EVT 0x1UL
#define MEM_EVT 0x2UL
// Additional Event Sets
#define TMA_EVT_SINGLE 0x3UL
#define TMA_EVT_COREWIDTH 0x4UL
#define TMA_EVT_RETIRE_WIDTH 0x5UL
#define TMA_EVT_ISSUE_WIDTH 0x6UL

#define MAX_CORE_WIDTH 8

#ifndef COREWIDTH
#error "Corewidth not defined in BOOM setup"
#endif

#if COREWIDTH > 3
#error "Corewidth is too large for all TMA events"
#endif

#if COREWIDTH == 0
#error "Cannot have corewidth of 0"
#elif (COREWIDTH == 1)
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
    "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", \
    "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", \
    "UNUSED", "UNUSED", "UNUSED" , "UNUSED"  \
};

#elif (COREWIDTH == 2)
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
    "D$ Blocked1", \
    "Uops Retired0", \
    "Uops Retired1", \
    "Fence Retired0", \
    "Fence Retired1", \
    "Fetch Bubble0", \
    "Fetch Bubble1", \
    "Uops Issued0", \
    "Uops Issued1", \
    "Uops Issued2", \
    "Uops Issued3", \
    "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", \
    "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED"\
};
#elif (COREWIDTH == 3)
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
    "D$ Blocked1", \
    "D$ Blocked2", \
    "Uops Retired0", \
    "Uops Retired1", \
    "Uops Retired2", \
    "Fence Retired0", \
    "Fence Retired1", \
    "Fence Retired2", \
    "Fetch Bubble0", \
    "Fetch Bubble1", \
    "Fetch Bubble2", \
    "Uops Issued0", \
    "Uops Issued1", \
    "Uops Issued2", \
    "Uops Issued3", \
    "Uops Issued4", \
    "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED" \
};
#else
#error "Unsupported Corewidth, we cannot use SCALAR=1 with COREWIDTH > 3 as there are not enough counters"
#endif

#endif /*TMA_DEFS_H_*/