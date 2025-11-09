
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
   
  config();
  dump_config();
  read_start();

  fmadd_s(0.1f, 0.2f, 0.3f);
  fmadd_s(0.1f, 0.2f, 0.3f);

  read_end();
  dump_counters(4, 4, 8);

  read_start();

  for (int i = 0; i < 2048; i++) {
    array[i] = 0;
  }

  read_end();
  dump_counters(4, 4, 8);

}
