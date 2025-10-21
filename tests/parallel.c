#include <stdint.h>
#include <stdio.h>

#ifndef NUM_ELEM
#define NUM_ELEM 1024*2
#endif

static uint32_t A[NUM_ELEM];

int main(void) {
    for (uint32_t i = 0; i < NUM_ELEM; ++i) A[i] = i;

    return 0;
}
    

