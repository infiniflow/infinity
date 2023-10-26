#pragma once

#include <cstdint>
#include <iostream>

namespace infinity {

// Encapsulates compiler builtins.
class Builtin {
public:
    // Generates a pause/yield cpu instruction, independently of the hardware.
    static void pause() noexcept {
#if defined(__x86_64__) || defined(__amd64__)
        __builtin_ia32_pause();
#elif defined(__arm__)
        asm("YIELD");
#endif
    }
};

#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

} // namespace infinity