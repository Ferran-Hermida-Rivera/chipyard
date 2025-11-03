
#include "pmu_defs.h"

// y = a*b + c  (single-precision)
static inline float fmadd_s(float a, float b, float c) {
  float y;
  asm volatile (
    "fmadd.s %0, %1, %2, %3\n"
    : "=f"(y)                 // output in an FP reg
    : "f"(a), "f"(b), "f"(c)  // inputs in FP regs
  );
  return y;
}

int main(){

  volatile int array[2048];
   
  start_counters();

  fmadd_s(0.1f, 0.2f, 0.3f);
  fmadd_s(0.1f, 0.2f, 0.3f);

  end_counters();

  read_start_counters();

  for (int i = 0; i < 2048; i++) {
    array[i] = 0;
  }

  end_counters();

}
