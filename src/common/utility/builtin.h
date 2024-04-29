#pragma once

namespace infinity {

#if defined(__x86_64__) || defined(__amd64__)
#define PAUSE __builtin_ia32_pause()
#elif defined(__arm__) || defined(__aarch64__)
#define PAUSE asm("YIELD")
#endif

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

class Builtin {
public:
    static int msb(unsigned int v);
    static int msb(unsigned long v);
    static int msb(unsigned long long v);
    static int lsb(unsigned int v);
    static int lsb(unsigned long v);
    static int lsb(unsigned long long v);
};

#ifdef __x86_64__
inline int Builtin::msb(unsigned int v) {
    unsigned int result;
    __asm __volatile("bsrl %0,%0" : "=r"(result) : "0"(v));
    return result;
}
inline int Builtin::lsb(unsigned int v) {
    unsigned int result;
    __asm __volatile("bsfl %0,%0" : "=r"(result) : "0"(v));
    return result;
}
inline int Builtin::msb(unsigned long v) {
    unsigned long result;
    __asm __volatile("bsrq %0,%0" : "=r"(result) : "0"(v));
    return result;
}
inline int Builtin::lsb(unsigned long v) {
    unsigned long result;
    __asm __volatile("bsfq %0,%0" : "=r"(result) : "0"(v));
    return result;
}
inline int Builtin::msb(unsigned long long v) {
    unsigned long long result;
    __asm __volatile("bsrq %0,%0" : "=r"(result) : "0"(v));
    return result;
}
inline int Builtin::lsb(unsigned long long v) {
    unsigned long long result;
    __asm __volatile("bsfq %0,%0" : "=r"(result) : "0"(v));
    return result;
}
#else
inline int Builtin::msb(unsigned int v) { return v ? sizeof(unsigned int) * 8 - 1 - __builtin_clz(v) : 0; }
inline int Builtin::msb(unsigned long v) { return v ? sizeof(unsigned long) * 8 - 1 - __builtin_clzl(v) : 0; }
inline int Builtin::msb(unsigned long long v) { return v ? sizeof(unsigned long long) * 8 - 1 - __builtin_clzll(v) : 0; }
inline int Builtin::lsb(unsigned int v) { return v ? __builtin_ctz(v) : 0; }
inline int Builtin::lsb(unsigned long v) { return v ? __builtin_ctzl(v) : 0; }
inline int Builtin::lsb(unsigned long long v) { return v ? __builtin_ctzll(v) : 0; }
#endif

} // namespace infinity