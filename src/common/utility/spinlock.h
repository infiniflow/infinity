#pragma once

#include <atomic>
#include <cstdint>
#include "builtin.h"

namespace infinity {

//Simple spinlock for mutual exclusion.
class Spinlock {
public:
    constexpr Spinlock() noexcept = default;
    ~Spinlock() = default;

    void lock() noexcept {
        while (true) {
            while (flag_.load(std::memory_order_relaxed)) {
                Builtin::pause();
            }

            if (try_lock()) {
                return;
            }
        }
    }

    //return true, when successfully locked.
    bool try_lock() noexcept {
        bool expected = false;
        return flag_.compare_exchange_weak(expected, true, std::memory_order_acquire);
    }

    void unlock() noexcept {
        flag_.store(false, std::memory_order_acquire);
    }

    [[nodiscard]] bool is_locked() const noexcept {
        return flag_.load(std::memory_order_relaxed);
    }

private:
    std::atomic_bool flag_{false};
};
} // namespace infinity
