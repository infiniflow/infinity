/*
 * configreport.h
 *
 */

#ifndef INCLUDE_MISC_CONFIGREPORT_H_
#define INCLUDE_MISC_CONFIGREPORT_H_

#include <stddef.h>  // for size_t
#include <stdint.h>
#include <stdio.h>

#include <roaring/portability.h>

#ifdef __cplusplus
extern "C" { namespace roaring { namespace misc {
#endif

#ifdef CROARING_IS_X64
// useful for basic info (0)
static inline void native_cpuid(unsigned int *eax, unsigned int *ebx,
                                unsigned int *ecx, unsigned int *edx) {
#ifdef CROARING_INLINE_ASM
    __asm volatile("cpuid"
                   : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
                   : "0"(*eax), "2"(*ecx));
#endif /* not sure what to do when inline assembly is unavailable*/
}

// CPUID instruction takes no parameters as CPUID implicitly uses the EAX
// register.
// The EAX register should be loaded with a value specifying what information to
// return
static inline void cpuinfo(int code, int *eax, int *ebx, int *ecx, int *edx) {
#ifdef CROARING_INLINE_ASM
    __asm__ volatile("cpuid;"  //  call cpuid instruction
                     : "=a"(*eax), "=b"(*ebx), "=c"(*ecx),
                       "=d"(*edx)  // output equal to "movl  %%eax %1"
                     : "a"(code)   // input equal to "movl %1, %%eax"
                     //:"%eax","%ebx","%ecx","%edx"// clobbered register
                     );
#endif /* not sure what to do when inline assembly is unavailable*/
}

static inline int computecacheline() {
    int eax = 0, ebx = 0, ecx = 0, edx = 0;
    cpuinfo((int)0x80000006, &eax, &ebx, &ecx, &edx);
    return ecx & 0xFF;
}

// this is quite imperfect, but can be handy
static inline const char *guessprocessor() {
    unsigned eax = 1, ebx = 0, ecx = 0, edx = 0;
    native_cpuid(&eax, &ebx, &ecx, &edx);
    const char *codename;
    switch (eax >> 4) {
        case 0x506E:
            codename = "Skylake";
            break;
        case 0x406C:
            codename = "CherryTrail";
            break;
        case 0x306D:
            codename = "Broadwell";
            break;
        case 0x306C:
            codename = "Haswell";
            break;
        case 0x306A:
            codename = "IvyBridge";
            break;
        case 0x206A:
        case 0x206D:
            codename = "SandyBridge";
            break;
        case 0x2065:
        case 0x206C:
        case 0x206F:
            codename = "Westmere";
            break;
        case 0x106E:
        case 0x106A:
        case 0x206E:
            codename = "Nehalem";
            break;
        case 0x1067:
        case 0x106D:
            codename = "Penryn";
            break;
        case 0x006F:
        case 0x1066:
            codename = "Merom";
            break;
        case 0x0066:
            codename = "Presler";
            break;
        case 0x0063:
        case 0x0064:
            codename = "Prescott";
            break;
        case 0x006D:
            codename = "Dothan";
            break;
        case 0x0366:
            codename = "Cedarview";
            break;
        case 0x0266:
            codename = "Lincroft";
            break;
        case 0x016C:
            codename = "Pineview";
            break;
        default:
            codename = "UNKNOWN";
            break;
    }
    return codename;
}

static inline void tellmeall() {
    printf("x64 processor:  %s\t", guessprocessor());

#ifdef __VERSION__
    printf(" compiler version: %s\t", __VERSION__);
#endif
    uint32_t config =  croaring_detect_supported_architectures();
    if((config & CROARING_NEON) == CROARING_NEON) {
        printf(" NEON detected\t");
    }
 #ifdef __AVX2__
    printf(" Building for AVX2\t");
 #endif
    if(croaring_avx2()) {
        printf( "AVX2 usable\t");
    }
    if((config & CROARING_AVX2) == CROARING_AVX2) {
        printf( "AVX2 detected\t");
       if(!croaring_avx2()) {
         printf( "AVX2 not used\t");
       }
     }
    if((config & CROARING_SSE42) == CROARING_SSE42) {
        printf(" SSE4.2 detected\t");
    }
    if((config & CROARING_BMI1) == CROARING_BMI1) {
        printf(" BMI1 detected\t");
    }
    if((config & CROARING_BMI2) == CROARING_BMI2) {
        printf(" BMI2 detected\t");
    }
    printf("\n");
    if ((sizeof(int) != 4) || (sizeof(long) != 8)) {
        printf("number of bytes: int = %lu long = %lu \n",
               (long unsigned int)sizeof(size_t),
               (long unsigned int)sizeof(int));
    }
#if __LITTLE_ENDIAN__
// This is what we expect!
// printf("you have little endian machine");
#endif
#if __BIG_ENDIAN__
    printf("you have a big endian machine");
#endif
#if __CHAR_BIT__
    if (__CHAR_BIT__ != 8) printf("on your machine, chars don't have 8bits???");
#endif
    if (computecacheline() != 64)
        printf("cache line: %d bytes\n", computecacheline());
}
#else

static inline void tellmeall() {
    printf("Non-X64  processor\n");
#ifdef __arm__
    printf("ARM processor detected\n");
#endif
#ifdef __VERSION__
    printf(" compiler version: %s\t", __VERSION__);
#endif
    if ((sizeof(int) != 4) || (sizeof(long) != 8)) {
        printf("number of bytes: int = %lu long = %lu \n",
               (long unsigned int)sizeof(size_t),
               (long unsigned int)sizeof(int));
    }
#if __LITTLE_ENDIAN__
// This is what we expect!
// printf("you have little endian machine");
#endif
#if __BIG_ENDIAN__
    printf("you have a big endian machine");
#endif
#if __CHAR_BIT__
    if (__CHAR_BIT__ != 8) printf("on your machine, chars don't have 8bits???");
#endif
}

#endif

#ifdef __cplusplus
} } }  // extern "C" { namespace roaring { namespace misc {
#endif

#endif /* INCLUDE_MISC_CONFIGREPORT_H_ */
