#ifndef CSR_DEFS_H_
#define CSR_DEFS_H_

#include "riscv.h"
#include "pmu.h"

/* Custom CSRs define --------------------------------------------------------*/

#define CSR_INTIQ_SLOTS 0x814
#define CSR_FPIQ_SLOTS 0x818
#define CSR_MEMIQ_SLOTS 0x81c
#define CSR_FETCH_WIDTH 0x820
#define CSR_INTIQ_WIDTH 0x824
#define CSR_FPIQ_WIDTH 0x828
#define CSR_MEMIQ_WIDTH 0x82c
#define CSR_ICACHE_PREFETCHERS 0x830
#define CSR_DCACHE_PREFETCHERS 0x834

/* CSR_ICACHE_PREFETCHERS define -----------------------------------------------*/
#define ENABLE 1
#define DISABLE 0

/* CSR_DCACHE_PREFETCHERS define -----------------------------------------------*/

#define NO_DCACHE_PREFETCHERS 0
#define NL_DCACHE_PREFETCHERS 1
#define MULTINL_DCACHE_PREFETCHERS 2
#define STRIDED_DCACHE_PREFETCHERS 4
#define LOCALIZEDSTRIDED_DCACHE_PREFETCHERS 8

/* Functions --------------------------------------------------------*/
#define SUPER_BARRIER() ({ \
    asm volatile("fence iorw, iorw" ::: "memory"); \
    asm volatile("fence.i" ::: "memory"); \
})


#define WRITE_CUSTOM_CSR(reg, val) ({    \
        __asm volatile("csrw " STRINGIFY(reg) ", %0" :: "r"(val)); \
    })


#define READ_CYCLE() ({                    \
  unsigned long __tmp;                            \
  asm volatile ("rdcycle %0" : "=r"(__tmp));  \
  __tmp; })



#endif /* CSR_DEFS_H_ */