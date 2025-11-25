
#include <stdio.h>
#include <stdint.h>

#include "pmu.h"
#include "riscv.h"
#include "csr_defs.h"
#include "pmu_defs.h"

void jumparound_benchmark(unsigned long long start[MAX_PMU_COUNT],
                            unsigned long long end[MAX_PMU_COUNT]) {
	volatile int sink = 0;

    volatile int zero = 0;

    #define TARGET_PAD(i) \
            asm volatile ("addi t0, t0, %0" :: "i"(i)); \
            sink += i; \
            if (!zero) { goto target_##i; } \
        target_$##i: \
            asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"); \
        target_##i:

    #define PAD_LIST \
        TARGET_PAD(0)   TARGET_PAD(1)   TARGET_PAD(2)   TARGET_PAD(3)   TARGET_PAD(4)   TARGET_PAD(5)   TARGET_PAD(6)   TARGET_PAD(7)   TARGET_PAD(8)   TARGET_PAD(9) \
        TARGET_PAD(10)  TARGET_PAD(11)  TARGET_PAD(12)  TARGET_PAD(13)  TARGET_PAD(14)  TARGET_PAD(15)  TARGET_PAD(16)  TARGET_PAD(17)  TARGET_PAD(18)  TARGET_PAD(19) \
        TARGET_PAD(20)  TARGET_PAD(21)  TARGET_PAD(22)  TARGET_PAD(23)  TARGET_PAD(24)  TARGET_PAD(25)  TARGET_PAD(26)  TARGET_PAD(27)  TARGET_PAD(28)  TARGET_PAD(29) \
        TARGET_PAD(30)  TARGET_PAD(31)  TARGET_PAD(32)  TARGET_PAD(33)  TARGET_PAD(34)  TARGET_PAD(35)  TARGET_PAD(36)  TARGET_PAD(37)  TARGET_PAD(38)  TARGET_PAD(39) \
        TARGET_PAD(40)  TARGET_PAD(41)  TARGET_PAD(42)  TARGET_PAD(43)  TARGET_PAD(44)  TARGET_PAD(45)  TARGET_PAD(46)  TARGET_PAD(47)  TARGET_PAD(48)  TARGET_PAD(49) \
        TARGET_PAD(50)  TARGET_PAD(51)  TARGET_PAD(52)  TARGET_PAD(53)  TARGET_PAD(54)  TARGET_PAD(55)  TARGET_PAD(56)  TARGET_PAD(57)  TARGET_PAD(58)  TARGET_PAD(59) \
        TARGET_PAD(60)  TARGET_PAD(61)  TARGET_PAD(62)  TARGET_PAD(63)  TARGET_PAD(64)  TARGET_PAD(65)  TARGET_PAD(66)  TARGET_PAD(67)  TARGET_PAD(68)  TARGET_PAD(69) \
        TARGET_PAD(70)  TARGET_PAD(71)  TARGET_PAD(72)  TARGET_PAD(73)  TARGET_PAD(74)  TARGET_PAD(75)  TARGET_PAD(76)  TARGET_PAD(77)  TARGET_PAD(78)  TARGET_PAD(79) \
        TARGET_PAD(80)  TARGET_PAD(81)  TARGET_PAD(82)  TARGET_PAD(83)  TARGET_PAD(84)  TARGET_PAD(85)  TARGET_PAD(86)  TARGET_PAD(87)  TARGET_PAD(88)  TARGET_PAD(89) \
        TARGET_PAD(90)  TARGET_PAD(91)  TARGET_PAD(92)  TARGET_PAD(93)  TARGET_PAD(94)  TARGET_PAD(95)  TARGET_PAD(96)  TARGET_PAD(97)  TARGET_PAD(98)  TARGET_PAD(99) \
        TARGET_PAD(100) TARGET_PAD(101) TARGET_PAD(102) TARGET_PAD(103) TARGET_PAD(104) TARGET_PAD(105) TARGET_PAD(106) TARGET_PAD(107) TARGET_PAD(108) TARGET_PAD(109) \
        TARGET_PAD(110) TARGET_PAD(111) TARGET_PAD(112) TARGET_PAD(113) TARGET_PAD(114) TARGET_PAD(115) TARGET_PAD(116) TARGET_PAD(117) TARGET_PAD(118) TARGET_PAD(119) \
        TARGET_PAD(120) TARGET_PAD(121) TARGET_PAD(122) TARGET_PAD(123) TARGET_PAD(124) TARGET_PAD(125) TARGET_PAD(126) TARGET_PAD(127) TARGET_PAD(128) TARGET_PAD(129) \
        TARGET_PAD(130) TARGET_PAD(131) TARGET_PAD(132) TARGET_PAD(133) TARGET_PAD(134) TARGET_PAD(135) TARGET_PAD(136) TARGET_PAD(137) TARGET_PAD(138) TARGET_PAD(139) \
        TARGET_PAD(140) TARGET_PAD(141) TARGET_PAD(142) TARGET_PAD(143) TARGET_PAD(144) TARGET_PAD(145) TARGET_PAD(146) TARGET_PAD(147) TARGET_PAD(148) TARGET_PAD(149) \
        TARGET_PAD(150) TARGET_PAD(151) TARGET_PAD(152) TARGET_PAD(153) TARGET_PAD(154) TARGET_PAD(155) TARGET_PAD(156) TARGET_PAD(157) TARGET_PAD(158) TARGET_PAD(159) \
        TARGET_PAD(160) TARGET_PAD(161) TARGET_PAD(162) TARGET_PAD(163) TARGET_PAD(164) TARGET_PAD(165) TARGET_PAD(166) TARGET_PAD(167) TARGET_PAD(168) TARGET_PAD(169) \
        TARGET_PAD(170) TARGET_PAD(171) TARGET_PAD(172) TARGET_PAD(173) TARGET_PAD(174) TARGET_PAD(175) TARGET_PAD(176) TARGET_PAD(177) TARGET_PAD(178) TARGET_PAD(179) \
        TARGET_PAD(180) TARGET_PAD(181) TARGET_PAD(182) TARGET_PAD(183) TARGET_PAD(184) TARGET_PAD(185) TARGET_PAD(186) TARGET_PAD(187) TARGET_PAD(188) TARGET_PAD(189) \
        TARGET_PAD(190) TARGET_PAD(191) TARGET_PAD(192) TARGET_PAD(193) TARGET_PAD(194) TARGET_PAD(195) TARGET_PAD(196) TARGET_PAD(197) TARGET_PAD(198) TARGET_PAD(199) \
        TARGET_PAD(200)


    SUPER_BARRIER();
    store_counter(start);
    
    PAD_LIST

    store_counter(end);

	
}

int main(void) {
	// printf("jumparound microbenchmark: stress icache next-line prefetch\n");

    unsigned long long warmup[MAX_PMU_COUNT];

    unsigned long long start_0[MAX_PMU_COUNT];
    unsigned long long end_0[MAX_PMU_COUNT];

    unsigned long long start_1[MAX_PMU_COUNT];
    unsigned long long end_1[MAX_PMU_COUNT];

    printf("Config perf counters\n");
    config();
    
    printf("=====================================\n");
    printf("ICache Prefetch Enabled\n");
    printf("=====================================\n\n");

    WRITE_CUSTOM_CSR(CSR_ICACHE_PREFETCHERS, ENABLE);

	jumparound_benchmark(warmup, warmup);

	jumparound_benchmark(start_0, end_0);


    printf("=====================================\n");
    printf("ICache Prefetch Disabled\n");
    printf("=====================================\n\n");

    WRITE_CUSTOM_CSR(CSR_ICACHE_PREFETCHERS, DISABLE);

    jumparound_benchmark(start_1, end_1);

    printf("Ubenchmark: Jumparound\n");
    printf("KnobConfig: I$ Prefetch Enabled\n");
    dump_counters_stored(4, 4, 8, start_0, end_0); // configs are specific to megaboom

    printf("KnobConfig: I$ Prefetch Disabled\n");
    dump_counters_stored(4, 4, 8, start_1, end_1); // configs are specific to megaboom

	return 0;
}

