#ifndef PMU_DEFS_H_
#define PMU_DEFS_H_

#include <riscv-pk/encoding.h>

#include "tma_defs.h"
#include "trigger.h"
#include "encoding.h"

#include <stdio.h>

// ===================================
// ===== Compile-time parameters =====
// ===================================

#ifndef COREWIDTH
#define COREWIDTH 1
#endif

// =============================
// ===== Runtime constants =====
// =============================

unsigned long long start[MAX_PMU_COUNT];
unsigned long long end[MAX_PMU_COUNT];
const char *name[MAX_PMU_COUNT];

// ============================
// ===== Helper functions =====
// ============================

#define write_csr_debug(reg, val) ({ \
    asm volatile ("csrw " #reg ", %0" :: "rK"(val)); \
    unsigned long long read_val = read_csr(reg); \
    printf("CSR %s: 0x%lx\n", #reg, read_val); })

/* Forward definitions. */
int config_pmu_counters();

/* Enable CSR collection in core. */
void enable_csr()
{
    for (int i = 0; i < MAX_PMU_COUNT; ++i)
    {
        name[i] = "";
    }
    write_csr(mcounteren, EN_MASK);
    write_csr(scounteren, EN_MASK);
    printf("mcounteren: %lu\n", read_csr(mcounteren));
    printf("scounteren: %lu\n", read_csr(scounteren));
}

/* Read initial values for reference. */
void read_start_counters()
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
    start[13] = read_csr(hpmcounter14);
    start[14] = read_csr(hpmcounter15);
    start[15] = read_csr(hpmcounter16);
    start[16] = read_csr(hpmcounter17);
    start[17] = read_csr(hpmcounter18);
    start[18] = read_csr(hpmcounter19);
    start[19] = read_csr(hpmcounter20);
    start[20] = read_csr(hpmcounter21);
    start[21] = read_csr(hpmcounter22);
    start[22] = read_csr(hpmcounter23);
    start[23] = read_csr(hpmcounter24);
    start[24] = read_csr(hpmcounter25);
    start[25] = read_csr(hpmcounter26);
    start[26] = read_csr(hpmcounter27);
    start[27] = read_csr(hpmcounter28);
    start[28] = read_csr(hpmcounter29);
    start[29] = read_csr(hpmcounter30);
    start[30] = read_csr(hpmcounter31);
}

/* Wrapper to enable and configure counters and read initial values. */
void start_counters()
{
    enable_csr();
    int n = config_pmu_counters();
    read_start_counters();
    // FIRESIM_START_TRIGGER();
}

/* Read final values of counters. */
void collect_counters(unsigned long long cnt[MAX_PMU_COUNT])
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
    end[13] = read_csr(hpmcounter14);
    end[14] = read_csr(hpmcounter15);
    end[15] = read_csr(hpmcounter16);
    end[16] = read_csr(hpmcounter17);
    end[17] = read_csr(hpmcounter18);
    end[18] = read_csr(hpmcounter19);
    end[19] = read_csr(hpmcounter20);
    end[20] = read_csr(hpmcounter21);
    end[21] = read_csr(hpmcounter22);
    end[22] = read_csr(hpmcounter23);
    end[23] = read_csr(hpmcounter24);
    end[24] = read_csr(hpmcounter25);
    end[25] = read_csr(hpmcounter26);
    end[26] = read_csr(hpmcounter27);
    end[27] = read_csr(hpmcounter28);
    end[28] = read_csr(hpmcounter29);
    end[29] = read_csr(hpmcounter30);
    end[30] = read_csr(hpmcounter31);

    for(unsigned int i = 0; i < MAX_PMU_COUNT; ++i)
    {
        cnt[i] = end[i] - start[i];
    }
}

void end_counters()
{
    // FIRESIM_END_TRIGGER();
    unsigned long long cnt[MAX_PMU_COUNT];
    collect_counters(cnt);

    printf("Core: Boom\n");

    printf("CoreWidth: %d\n", COREWIDTH);
    for (unsigned int i = 0; i < MAX_PMU_COUNT; ++i)
    {
        printf("%s: %lu\n", name[i], cnt[i]);

    }
}

int config_pmu_counters()
{
    /* Events shared by all configs*/
    name[0] = "Cycle";
    name[1] = "Int Ret";
    write_csr_debug(mhpmevent3, UOP_EVT | EVT(1)); name[2] = "Branch Mispredict";
    write_csr_debug(mhpmevent4, UOP_EVT | EVT(3)); name[3] = "Flush";
    write_csr_debug(mhpmevent5, MEM_EVT| EVT(0)); name[4] = "I$ Miss";
    write_csr_debug(mhpmevent6, MEM_EVT| EVT(1)); name[5] = "D$ Miss";
    write_csr_debug(mhpmevent7, TMA_EVT_SINGLE| EVT(0)); name[6] = "I$ Blocked";
    write_csr_debug(mhpmevent8, TMA_EVT_SINGLE| EVT(1)); name[7] = "Recovering";
    /* End events shared by all configs*/
#if (COREWIDTH == 1)
    write_csr_debug(mhpmevent9,  TMA_EVT_RETIRE_WIDTH | EVT(0));  name[8]  = "Uops Retired0";
    write_csr_debug(mhpmevent10, TMA_EVT_RETIRE_WIDTH | EVT(1));  name[9]  = "Fence Retired0";
    write_csr_debug(mhpmevent11, TMA_EVT_RETIRE_WIDTH | EVT(2));  name[10] = "D$ blocked0";
    write_csr_debug(mhpmevent12, TMA_EVT_COREWIDTH    | EVT(0));  name[11] = "Fetch Bubble0";
    write_csr_debug(mhpmevent13, TMA_EVT_ISSUE_WIDTH  | EVT(0));  name[12] = "Uops Issued0";
    write_csr_debug(mhpmevent14, TMA_EVT_ISSUE_WIDTH  | EVT(1));  name[13] = "Uops Issued1";
    write_csr_debug(mhpmevent15, TMA_EVT_ISSUE_WIDTH  | EVT(2));  name[14] = "Uops Issued2";

#elif (COREWIDTH == 2)
    write_csr_debug(mhpmevent9,  TMA_EVT_RETIRE_WIDTH | EVT(0));  name[8]  = "Uops Retired0";
    write_csr_debug(mhpmevent10, TMA_EVT_RETIRE_WIDTH | EVT(3));  name[9]  = "Uops Retired1";
    write_csr_debug(mhpmevent11, TMA_EVT_RETIRE_WIDTH | EVT(1));  name[10] = "Fence Retired0";
    write_csr_debug(mhpmevent12, TMA_EVT_RETIRE_WIDTH | EVT(4));  name[11] = "Fence Retired1";
    write_csr_debug(mhpmevent13, TMA_EVT_RETIRE_WIDTH | EVT(2));  name[12] = "D$ Blocked0";
    write_csr_debug(mhpmevent14, TMA_EVT_RETIRE_WIDTH | EVT(5));  name[13] = "D$ Blocked1";
    write_csr_debug(mhpmevent15, TMA_EVT_COREWIDTH    | EVT(0));  name[14] = "Fetch Bubble0";
    write_csr_debug(mhpmevent16, TMA_EVT_COREWIDTH    | EVT(2));  name[15] = "Fetch Bubble1";
    write_csr_debug(mhpmevent17, TMA_EVT_ISSUE_WIDTH  | EVT(0));  name[16] = "Uops Issued0";
    write_csr_debug(mhpmevent18, TMA_EVT_ISSUE_WIDTH  | EVT(1));  name[17] = "Uops Issued1";
    write_csr_debug(mhpmevent19, TMA_EVT_ISSUE_WIDTH  | EVT(2));  name[18] = "Uops Issued2";
    write_csr_debug(mhpmevent20, TMA_EVT_ISSUE_WIDTH  | EVT(3));  name[19] = "Uops Issued3";

#elif (COREWIDTH == 3)
    write_csr_debug(mhpmevent9,  TMA_EVT_RETIRE_WIDTH | EVT(0));  name[8]  = "Uops Retired0";
    write_csr_debug(mhpmevent10, TMA_EVT_RETIRE_WIDTH | EVT(3));  name[9]  = "Uops Retired1";
    write_csr_debug(mhpmevent11, TMA_EVT_RETIRE_WIDTH | EVT(6));  name[10] = "Uops Retired2";
    write_csr_debug(mhpmevent12, TMA_EVT_RETIRE_WIDTH | EVT(1));  name[11] = "Fence Retired0";
    write_csr_debug(mhpmevent13, TMA_EVT_RETIRE_WIDTH | EVT(4));  name[12] = "Fence Retired1";
    write_csr_debug(mhpmevent14, TMA_EVT_RETIRE_WIDTH | EVT(7));  name[13] = "Fence Retired2";
    write_csr_debug(mhpmevent15, TMA_EVT_RETIRE_WIDTH | EVT(2));  name[14] = "D$ Blocked0";
    write_csr_debug(mhpmevent16, TMA_EVT_RETIRE_WIDTH | EVT(5));  name[15] = "D$ Blocked1";
    write_csr_debug(mhpmevent17, TMA_EVT_RETIRE_WIDTH | EVT(8));  name[16] = "D$ Blocked2";
    write_csr_debug(mhpmevent18, TMA_EVT_COREWIDTH    | EVT(0));  name[17] = "Fetch Bubble0";
    write_csr_debug(mhpmevent19, TMA_EVT_COREWIDTH    | EVT(2));  name[18] = "Fetch Bubble1";
    write_csr_debug(mhpmevent20, TMA_EVT_COREWIDTH    | EVT(4));  name[19] = "Fetch Bubble2";
    write_csr_debug(mhpmevent21, TMA_EVT_ISSUE_WIDTH  | EVT(0));  name[20] = "Uops Issued0";
    write_csr_debug(mhpmevent22, TMA_EVT_ISSUE_WIDTH  | EVT(1));  name[21] = "Uops Issued1";
    write_csr_debug(mhpmevent23, TMA_EVT_ISSUE_WIDTH  | EVT(2));  name[22] = "Uops Issued2";
    write_csr_debug(mhpmevent24, TMA_EVT_ISSUE_WIDTH  | EVT(3));  name[23] = "Uops Issued3";
    write_csr_debug(mhpmevent25, TMA_EVT_ISSUE_WIDTH  | EVT(4));  name[24] = "Uops Issued4";
#else
#error ("Unsupported Corewidth for scalar")
#endif
    // printf("MPHMEVENT3: 0x%lx\n", read_csr(mhpmevent3));
    // printf("MPHMEVENT4: 0x%lx\n", read_csr(mhpmevent4));
    // printf("MPHMEVENT5: 0x%lx\n", read_csr(mhpmevent5));
    // printf("MPHMEVENT6: 0x%lx\n", read_csr(mhpmevent6));
    // printf("MPHMEVENT7: 0x%lx\n", read_csr(mhpmevent7));
    // printf("MPHMEVENT8: 0x%lx\n", read_csr(mhpmevent8));
    // printf("MPHMEVENT9: 0x%lx\n", read_csr(mhpmevent9));
    // printf("MPHMEVENT10: 0x%lx\n", read_csr(mhpmevent10));
    // printf("MPHMEVENT11: 0x%lx\n", read_csr(mhpmevent11));
    // printf("MPHMEVENT12: 0x%lx\n", read_csr(mhpmevent12));
    // printf("MPHMEVENT13: 0x%lx\n", read_csr(mhpmevent13));
    // printf("MPHMEVENT14: 0x%lx\n", read_csr(mhpmevent14));
    // printf("MPHMEVENT15: 0x%lx\n", read_csr(mhpmevent15));
    // printf("MPHMEVENT16: 0x%lx\n", read_csr(mhpmevent16));
    // printf("MPHMEVENT17: 0x%lx\n", read_csr(mhpmevent17));
    // printf("MPHMEVENT18: 0x%lx\n", read_csr(mhpmevent18));
    // printf("MPHMEVENT19: 0x%lx\n", read_csr(mhpmevent19));
    // printf("MPHMEVENT20: 0x%lx\n", read_csr(mhpmevent20));
    // printf("MPHMEVENT21: 0x%lx\n", read_csr(mhpmevent21));
    // printf("MPHMEVENT22: 0x%lx\n", read_csr(mhpmevent22));
    // printf("MPHMEVENT23: 0x%lx\n", read_csr(mhpmevent23));
    // printf("MPHMEVENT24: 0x%lx\n", read_csr(mhpmevent24));
    // printf("MPHMEVENT25: 0x%lx\n", read_csr(mhpmevent25));
}

#endif /*PMU_DEFS_H_*/