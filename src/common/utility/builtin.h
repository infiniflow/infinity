

namespace infinity {

#if defined(__x86_64__) || defined(__amd64__)
#define PAUSE __builtin_ia32_pause()
#elif defined(__arm__)
#define PAUSE asm("YIELD")
#endif

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

} // namespace infinity