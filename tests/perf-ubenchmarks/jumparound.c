
#include <stdio.h>
#include <stdint.h>

static inline uint64_t rdcycle() {
	uint64_t cycle;
	asm volatile ("rdcycle %0" : "=r"(cycle));
	return cycle;
}

#define NUM_JUMPS 32

int jumparound_benchmark() {
	volatile int sink = 0;
	uint64_t start, end;
	void *targets[NUM_JUMPS];

    // int read = -1;
    // int write = 0;
    // asm volatile(
    //     "csrw 0x830, %1\n"
    //     "csrr %0, 0x830"
    //     : "=r" (read)
    //     : "r" (write)
    // );
    // printf("read back from the CSR: %x\n", read);

	// #define TARGET_ADDR(i) &&target_##i
	// for (int i = 0; i < NUM_JUMPS; ++i) {
	// 	switch(i) {
	// 		case 0: targets[i] = TARGET_ADDR(0); break;
	// 		case 1: targets[i] = TARGET_ADDR(1); break;
	// 		case 2: targets[i] = TARGET_ADDR(2); break;
	// 		case 3: targets[i] = TARGET_ADDR(3); break;
	// 		case 4: targets[i] = TARGET_ADDR(4); break;
	// 		case 5: targets[i] = TARGET_ADDR(5); break;
	// 		case 6: targets[i] = TARGET_ADDR(6); break;
	// 		case 7: targets[i] = TARGET_ADDR(7); break;
	// 		case 8: targets[i] = TARGET_ADDR(8); break;
	// 		case 9: targets[i] = TARGET_ADDR(9); break;
	// 		case 10: targets[i] = TARGET_ADDR(10); break;
	// 		case 11: targets[i] = TARGET_ADDR(11); break;
	// 		case 12: targets[i] = TARGET_ADDR(12); break;
	// 		case 13: targets[i] = TARGET_ADDR(13); break;
	// 		case 14: targets[i] = TARGET_ADDR(14); break;
	// 		case 15: targets[i] = TARGET_ADDR(15); break;
	// 		case 16: targets[i] = TARGET_ADDR(16); break;
	// 		case 17: targets[i] = TARGET_ADDR(17); break;
	// 		case 18: targets[i] = TARGET_ADDR(18); break;
	// 		case 19: targets[i] = TARGET_ADDR(19); break;
	// 		case 20: targets[i] = TARGET_ADDR(20); break;
	// 		case 21: targets[i] = TARGET_ADDR(21); break;
	// 		case 22: targets[i] = TARGET_ADDR(22); break;
	// 		case 23: targets[i] = TARGET_ADDR(23); break;
	// 		case 24: targets[i] = TARGET_ADDR(24); break;
	// 		case 25: targets[i] = TARGET_ADDR(25); break;
	// 		case 26: targets[i] = TARGET_ADDR(26); break;
	// 		case 27: targets[i] = TARGET_ADDR(27); break;
	// 		case 28: targets[i] = TARGET_ADDR(28); break;
	// 		case 29: targets[i] = TARGET_ADDR(29); break;
	// 		case 30: targets[i] = TARGET_ADDR(30); break;
	// 		case 31: targets[i] = TARGET_ADDR(31); break;
	// 	}
	// }

	start = rdcycle();

    volatile int zero = 0;

    #define TARGET_PAD(i) \
            asm volatile ("addi t0, t0, %0" :: "i"(i)); \
            sink += i; \
            if (!zero) { goto target_##i; } \
        target_$##i: \
            asm volatile ("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;"); \
        target_##i:

    TARGET_PAD(0)
    TARGET_PAD(1)
    TARGET_PAD(2)
    TARGET_PAD(3)
    TARGET_PAD(4)
    TARGET_PAD(5)
    TARGET_PAD(6)
    TARGET_PAD(7)
    TARGET_PAD(8)
    TARGET_PAD(9)
    TARGET_PAD(10)
    TARGET_PAD(11)
    TARGET_PAD(12)
    TARGET_PAD(13)
    TARGET_PAD(14)
    TARGET_PAD(15)
    TARGET_PAD(16)
    TARGET_PAD(17)
    TARGET_PAD(18)
    TARGET_PAD(19)
    TARGET_PAD(20)
    TARGET_PAD(21)
    TARGET_PAD(22)
    TARGET_PAD(23)
    TARGET_PAD(24)
    TARGET_PAD(25)
    TARGET_PAD(26)
    TARGET_PAD(27)
    TARGET_PAD(28)
    TARGET_PAD(29)
    TARGET_PAD(30)
    TARGET_PAD(31)
    TARGET_PAD(32)
    TARGET_PAD(33)
    TARGET_PAD(34)
    TARGET_PAD(35)
    TARGET_PAD(36)
    TARGET_PAD(37)
    TARGET_PAD(38)
    TARGET_PAD(39)
    TARGET_PAD(40)
    TARGET_PAD(41)
    TARGET_PAD(42)
    TARGET_PAD(43)
    TARGET_PAD(44)
    TARGET_PAD(45)
    TARGET_PAD(46)
    TARGET_PAD(47)
    TARGET_PAD(48)
    TARGET_PAD(49)
    TARGET_PAD(50)
    TARGET_PAD(51)
    TARGET_PAD(52)
    TARGET_PAD(53)
    TARGET_PAD(54)
    TARGET_PAD(55)
    TARGET_PAD(56)
    TARGET_PAD(57)
    TARGET_PAD(58)
    TARGET_PAD(59)
    TARGET_PAD(60)
    TARGET_PAD(61)
    TARGET_PAD(62)
    TARGET_PAD(63)
    TARGET_PAD(64)
    TARGET_PAD(65)
    TARGET_PAD(66)
    TARGET_PAD(67)
    TARGET_PAD(68)
    TARGET_PAD(69)
    TARGET_PAD(70)
    TARGET_PAD(71)
    TARGET_PAD(72)
    TARGET_PAD(73)
    TARGET_PAD(74)
    TARGET_PAD(75)
    TARGET_PAD(76)
    TARGET_PAD(77)
    TARGET_PAD(78)
    TARGET_PAD(79)
    TARGET_PAD(80)
    TARGET_PAD(81)
    TARGET_PAD(82)
    TARGET_PAD(83)
    TARGET_PAD(84)
    TARGET_PAD(85)
    TARGET_PAD(86)
    TARGET_PAD(87)
    TARGET_PAD(88)
    TARGET_PAD(89)
    TARGET_PAD(90)
    TARGET_PAD(91)
    TARGET_PAD(92)
    TARGET_PAD(93)
    TARGET_PAD(94)
    TARGET_PAD(95)
    TARGET_PAD(96)
    TARGET_PAD(97)
    TARGET_PAD(98)
    TARGET_PAD(99)
    TARGET_PAD(100)
	

	end = rdcycle();
    return end - start;
	// printf("jumparound: %ld cycles for 100 jumps\n", (long)(end - start));
	// if (sink == 42) printf("sink\n");
}

int main(void) {
	// printf("jumparound microbenchmark: stress icache next-line prefetch\n");

    
    int read = -1;
    int write = 0;
    asm volatile(
        "fence.i\n"
        
        "csrw 0x830, %1\n"
        "csrr %0, 0x830"
        : "=r" (read)
        : "r" (write)
    );
    // printf("read back from the CSR: %x\n", read);

	int warmup = jumparound_benchmark();

    asm volatile(
        "csrw 0x830, %1\n"
        "csrr %0, 0x830"
        : "=r" (read)
        : "r" (write)
    );

    asm volatile(
        "fence.i\n"
        );
	int res1 = jumparound_benchmark();
    asm volatile(
        "fence.i\n"
        );
	int res2 = jumparound_benchmark();
    asm volatile(
        "fence.i\n"
        );
	int res3 = jumparound_benchmark();
    asm volatile(
        "fence.i\n"
        );


    read = -1;
    write = 1;
    asm volatile(
        "csrw 0x830, %1\n"
        "csrr %0, 0x830"
        : "=r" (read)
        : "r" (write)
    );
	
    int res10 = jumparound_benchmark();
    asm volatile(
        "fence.i\n"
        );	
    int res11 = jumparound_benchmark();
    asm volatile(
        "fence.i\n"
        );	
    int res12 = jumparound_benchmark();
    asm volatile(
        "fence.i\n"
        );

    printf("Res1: %d\n", res1);
    printf("Res2: %d\n", res2);
    printf("Res3: %d\n", res3);

    printf("Res10: %d\n", res10);
    printf("Res11: %d\n", res11);
    printf("Res12: %d\n", res12);

	return 0;
}

