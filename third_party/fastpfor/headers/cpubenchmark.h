/**
 * This code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 */

#ifndef CPUBENCHMARK_H_
#define CPUBENCHMARK_H_

#include "common.h"

namespace FastPForLib {

#if defined(__corei7__) // __amd64__ is untested
// start and stop are as recommended by
// Gabriele Paoloni, How to Benchmark Code Execution Times on Intel� IA-32 and
// IA-64 Instruction Set Architectures
// September 2010
// http://edc.intel.com/Link.aspx?id=3954

static __inline__ unsigned long long startRDTSC(void) {
  unsigned cycles_low, cycles_high;
  asm volatile("CPUID\n\t"
               "RDTSC\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  return (static_cast<unsigned long long>(cycles_high) << 32) | cycles_low;
}

static __inline__ unsigned long long stopRDTSCP(void) {
  unsigned cycles_low, cycles_high;
  /// This should work fine on most machines, if the RDTSCP thing
  /// fails for you, use the  rdtsc() call instead.
  asm volatile("RDTSCP\n\t"
               "mov %%edx, %0\n\t"
               "mov %%eax, %1\n\t"
               "CPUID\n\t"
               : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "%rcx",
                 "%rdx");
  return (static_cast<unsigned long long>(cycles_high) << 32) | cycles_low;
}
#elif (defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_AMD64)))

static inline unsigned long long startRDTSC(void) { return __rdtsc(); }

static inline unsigned long long stopRDTSCP(void) { return __rdtsc(); }
#elif defined(_MSC_VER) && defined(_M_ARM64)
  // oriented by zeromq implementation for msc arm/arm64
  // https://github.com/zeromq/libzmq/blob/master/src/clock.cpp
  inline unsigned long long rdtsc() {
    const int64_t pmccntr_el0 = (((3 & 1) << 14) |  // op0
                                 ((3 & 7) << 11) |  // op1
                                 ((9 & 15) << 7) |  // crn
                                 ((13 & 15) << 3) | // crm
                                 ((0 & 7) << 0));   // op2

    return _ReadStatusReg (pmccntr_el0);
  }

  static inline unsigned long long startRDTSC(void) { return rdtsc(); }

  static inline unsigned long long stopRDTSCP(void) { return rdtsc(); }
#elif  (defined(_MSC_VER) && (defined(_M_ARM64)))
// Taken from microsoft documentation (see
// https://learn.microsoft.com/en-us/cpp/build/overview-of-arm-abi-conventions?view=msvc-170

static inline unsigned long long startRDTSC(void) { return __rdpmccntr64(); }

static inline unsigned long long stopRDTSCP(void) { return __rdpmccntr64(); }

#elif defined(__i386__) || defined(__x86_64__)

// Taken from stackoverflow (see
// http://stackoverflow.com/questions/3830883/cpu-cycle-count-based-profiling-in-c-c-linux-x86-64)
// Can give nonsensical results on multi-core AMD processors.
inline unsigned long long rdtsc() {
  unsigned int lo, hi;
  asm volatile("cpuid \n" /* serializing */
               "rdtsc"
               : "=a"(lo), "=d"(hi) /* outputs */
               : "a"(0)             /* inputs */
               : "%ebx", "%ecx");   /* clobbers*/
  return (static_cast<unsigned long long>(lo)) |
         ((static_cast<unsigned long long>(hi)) << 32);
}
static __inline__ unsigned long long startRDTSC(void) { return rdtsc(); }

static __inline__ unsigned long long stopRDTSCP(void) { return rdtsc(); }
#elif (defined(__GNUC__) && (defined(__aarch64__)))
    inline uint64_t rdtsc() {
        uint64_t cycles;
        asm volatile("mrs %0, cntvct_el0"
                : "=r"(cycles)); /* output */
        return cycles;
    }

    static __inline__ uint64_t startRDTSC(void) { return rdtsc(); }

    static __inline__ uint64_t stopRDTSCP(void) { return rdtsc(); }
#elif(defined(__arm__) || defined(__ppc__) || defined(__ppc64__)) || (defined(_MSC_VER) && defined(_M_ARM64))

// for PPC we should be able to use tbl, but I could not find
// an equivalent to rdtsc for ARM.

inline uint64_t rdtsc() { return 0; }
static __inline__ uint64_t startRDTSC(void) { return 0; }
static __inline__ uint64_t stopRDTSCP(void) { return 0; }
#else
#error Unknown architecture
#endif

class CPUBenchmark {
public:
  CPUBenchmark() : ticktime(0) { start(); }

  unsigned long long ticktime;

  void start() { ticktime = startRDTSC(); }

  unsigned long long stop() { return stopRDTSCP() - ticktime; }
};

} // namespace FastPForLib

#endif /* CPUBENCHMARK_H_ */
