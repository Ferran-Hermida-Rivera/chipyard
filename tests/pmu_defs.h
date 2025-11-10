#ifndef PMU_DEFS_H_
#define PMU_DEFS_H_

#include <riscv-pk/encoding.h>

#include "trigger.h"

#include <stdio.h>

#define EN_MASK 0xFFFFFFFF
#define EVT(n) 1UL << (n + 8)
#define MAX_PMU_COUNT 13

// Existing Event Sets
#define EX_EVT 0x0UL
#define UOP_EVT 0x1UL
#define MEM_EVT 0x2UL
// Additional Event Sets
#define TMA_EVT_SINGLE 0x3UL
#define TMA_EVT_COREWIDTH 0x4UL
#define TMA_EVT_RETIRE_WIDTH 0x5UL
#define TMA_EVT_ISSUE_WIDTH 0x6UL

// =============================
// ===== Runtime constants =====
// =============================

unsigned long long start[MAX_PMU_COUNT];
unsigned long long end[MAX_PMU_COUNT];

// ============================
// ===== Helper functions =====
// ============================

#define write_csr_debug(reg, val) ({ \
    asm volatile ("csrw " #reg ", %0" :: "rK"(val)); \
    unsigned long long read_val = read_csr(reg); \
    printf("CSR %s: 0x%lx\n", #reg, read_val); })

void config()
{

    /* Enable counters */
    write_csr(mcounteren, EN_MASK);
    write_csr(scounteren, EN_MASK);

    /* Config events */
    write_csr_debug(mhpmevent3, UOP_EVT | EVT(1));               // "Branch Mispredict"
    write_csr_debug(mhpmevent4, UOP_EVT | EVT(3));               // "Flush"
    write_csr_debug(mhpmevent5, MEM_EVT| EVT(0));                // "I$ Miss"
    write_csr_debug(mhpmevent6, MEM_EVT| EVT(1));                // "D$ Miss"
    write_csr_debug(mhpmevent7, TMA_EVT_SINGLE| EVT(0));         // "I$ Blocked"
    write_csr_debug(mhpmevent8, TMA_EVT_SINGLE| EVT(1));         // "Recovering"
    write_csr_debug(mhpmevent9,  TMA_EVT_RETIRE_WIDTH | EVT(0)); // "Uops Retired"
    write_csr_debug(mhpmevent10, TMA_EVT_RETIRE_WIDTH | EVT(1)); // "Fence Retired"
    write_csr_debug(mhpmevent11, TMA_EVT_RETIRE_WIDTH | EVT(2)); // "D$ blocked"
    write_csr_debug(mhpmevent12, TMA_EVT_COREWIDTH    | EVT(0)); // "Fetch Bubble"
    write_csr_debug(mhpmevent13, TMA_EVT_ISSUE_WIDTH  | EVT(0)); // "Uops Issued
}

void dump_config()
{
    printf("mcounteren: %lu\n", read_csr(mcounteren));
    printf("scounteren: %lu\n", read_csr(scounteren));

    printf("mhpmevent3: 0x%lx\n", read_csr(mhpmevent3));
    printf("mhpmevent4: 0x%lx\n", read_csr(mhpmevent4));
    printf("mhpmevent5: 0x%lx\n", read_csr(mhpmevent5));
    printf("mhpmevent6: 0x%lx\n", read_csr(mhpmevent6));
    printf("mhpmevent7: 0x%lx\n", read_csr(mhpmevent7));
    printf("mhpmevent8: 0x%lx\n", read_csr(mhpmevent8));
    printf("mhpmevent9: 0x%lx\n", read_csr(mhpmevent9));
    printf("mhpmevent10: 0x%lx\n", read_csr(mhpmevent10));
    printf("mhpmevent11: 0x%lx\n", read_csr(mhpmevent11));
    printf("mhpmevent12: 0x%lx\n", read_csr(mhpmevent12));
    printf("mhpmevent13: 0x%lx\n", read_csr(mhpmevent13));
}

/* Read initial values for reference. */
void read_start()
{
    start[0] = read_csr(cycle);
    start[1] = read_csr(instret);
    start[2] = read_csr(hpmcounter3);
    start[3] = read_csr(hpmcounter4);
    start[4] = read_csr(hpmcounter5);
    start[5] = read_csr(hpmcounter6);
    start[6] = read_csr(hpmcounter7);
    start[7] = read_csr(hpmcounter8);
    start[8] = read_csr(hpmcounter9);
    start[9] = read_csr(hpmcounter10);
    start[10] = read_csr(hpmcounter11);
    start[11] = read_csr(hpmcounter12);
    start[12] = read_csr(hpmcounter13);
}

void store_start(unsigned long long *store)
{
    store[0] = read_csr(cycle);
    store[1] = read_csr(instret);
    store[2] = read_csr(hpmcounter3);
    store[3] = read_csr(hpmcounter4);
    store[4] = read_csr(hpmcounter5);
    store[5] = read_csr(hpmcounter6);
    store[6] = read_csr(hpmcounter7);
    store[7] = read_csr(hpmcounter8);
    store[8] = read_csr(hpmcounter9);
    store[9] = read_csr(hpmcounter10);
    store[10] = read_csr(hpmcounter11);
    store[11] = read_csr(hpmcounter12);
    store[12] = read_csr(hpmcounter13);
}


/* Read final values of counters. */
void read_end()
{
    end[0] = read_csr(cycle);
    end[1] = read_csr(instret);
    end[2] = read_csr(hpmcounter3);
    end[3] = read_csr(hpmcounter4);
    end[4] = read_csr(hpmcounter5);
    end[5] = read_csr(hpmcounter6);
    end[6] = read_csr(hpmcounter7);
    end[7] = read_csr(hpmcounter8);
    end[8] = read_csr(hpmcounter9);
    end[9] = read_csr(hpmcounter10);
    end[10] = read_csr(hpmcounter11);
    end[11] = read_csr(hpmcounter12);
    end[12] = read_csr(hpmcounter13);
}

void store_end(unsigned long long *store)
{
    store[0] = read_csr(cycle);
    store[1] = read_csr(instret);
    store[2] = read_csr(hpmcounter3);
    store[3] = read_csr(hpmcounter4);
    store[4] = read_csr(hpmcounter5);
    store[5] = read_csr(hpmcounter6);
    store[6] = read_csr(hpmcounter7);
    store[7] = read_csr(hpmcounter8);
    store[8] = read_csr(hpmcounter9);
    store[9] = read_csr(hpmcounter10);
    store[10] = read_csr(hpmcounter11);
    store[11] = read_csr(hpmcounter12);
    store[12] = read_csr(hpmcounter13);
}

unsigned int nearest_power_of_two(unsigned int x)
{
    unsigned int r = 1;
    while (r < x)
        r <<= 1;
    return r;
}

void dump_counters(unsigned int corewidth, unsigned int retirewidth, unsigned int issuewidth)
{
    unsigned int corewidthbits = nearest_power_of_two(corewidth);
    unsigned int retirewidthbits = nearest_power_of_two(retirewidth);
    unsigned int issuewidthbits = nearest_power_of_two(issuewidth);
    printf("%s: %lu\n", "Cycle",             end[0]-start[0]);
    printf("%s: %lu\n", "Int Ret",           end[1]-start[1]);
    printf("%s: %lu\n", "Branch Mispredict", end[2]-start[2]);
    printf("%s: %lu\n", "Flush",             end[3]-start[3]);
    printf("%s: %lu\n", "I$ Miss",           end[4]-start[4]);
    printf("%s: %lu\n", "D$ Miss",           end[5]-start[5]);
    printf("%s: %lu\n", "I$ Blocked",        end[6]-start[6]);
    printf("%s: %lu\n", "Recovering",        end[7]-start[7]);
    printf("%s: %lu\n", "Uops Retired",     (end[8]-start[8]) * retirewidthbits);
    printf("%s: %lu\n", "Fence Retired",    (end[9]-start[9]) * retirewidthbits);
    printf("%s: %lu\n", "D$ blocked",       (end[10]-start[10]) * retirewidthbits);
    printf("%s: %lu\n", "Fetch Bubble",     (end[11]-start[11]) * corewidthbits);
    printf("%s: %lu\n", "Uops Issued",      (end[12]-start[12]) * issuewidthbits);

}

void dump_counters_stored(unsigned int corewidth, unsigned int retirewidth, unsigned int issuewidth,
                            unsigned long long *start_vals, unsigned long long *end_vals)
{
    unsigned int corewidthbits = nearest_power_of_two(corewidth);
    unsigned int retirewidthbits = nearest_power_of_two(retirewidth);
    unsigned int issuewidthbits = nearest_power_of_two(issuewidth);
    printf("%s: %lu\n", "Cycle",             end_vals[0]-start_vals[0]);
    printf("%s: %lu\n", "Int Ret",           end_vals[1]-start_vals[1]);
    printf("%s: %lu\n", "Branch Mispredict", end_vals[2]-start_vals[2]);
    printf("%s: %lu\n", "Flush",             end_vals[3]-start_vals[3]);
    printf("%s: %lu\n", "I$ Miss",           end_vals[4]-start_vals[4]);
    printf("%s: %lu\n", "D$ Miss",           end_vals[5]-start_vals[5]);
    printf("%s: %lu\n", "I$ Blocked",        end_vals[6]-start_vals[6]);
    printf("%s: %lu\n", "Recovering",        end_vals[7]-start_vals[7]);
    printf("%s: %lu\n", "Uops Retired",     (end_vals[8]-start_vals[8]) * retirewidthbits);
    printf("%s: %lu\n", "Fence Retired",    (end_vals[9]-start_vals[9]) * retirewidthbits);
    printf("%s: %lu\n", "D$ blocked",       (end_vals[10]-start_vals[10]) * retirewidthbits);
    printf("%s: %lu\n", "Fetch Bubble",     (end_vals[11]-start_vals[11]) * corewidthbits);
    printf("%s: %lu\n", "Uops Issued",      (end_vals[12]-start_vals[12]) * issuewidthbits);
}

#endif /*PMU_DEFS_H_*/