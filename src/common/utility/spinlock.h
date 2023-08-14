#pragma once

#include <atomic>
#include <cstdint>
#include "builtin.h"

namespace infinity {

//Simple spinlock for mutual exclusion.
class SpinLock {
public:
    explicit SpinLock(std::int32_t count = 1024) noexcept
        : spin_count_(count), locked_(false) {
    }

    bool try_lock() noexcept {
        return !locked_.exchange(true, std::memory_order_acquire);
    }

    void lock() noexcept {
        auto counter = spin_count_;
        while (!try_lock()) {
            while (locked_.load(std::memory_order_relaxed)) {
                if (counter-- <= 0) {
                    std::this_thread::yield();
                    counter = spin_count_;
                }
            }
        }
    }

    void unlock() noexcept {
        locked_.store(false, std::memory_order_release);
    }

private:
    std::int32_t spin_count_;
    std::atomic<bool> locked_;
};

class ScopedSpinLock {
public:
    explicit ScopedSpinLock(SpinLock &lock) : lock_(lock) { lock_.lock(); }

    ~ScopedSpinLock() { lock_.unlock(); }

private:
    ScopedSpinLock(const ScopedSpinLock &) = delete;
    ScopedSpinLock &operator=(const ScopedSpinLock &) = delete;    

    SpinLock &lock_;
};

struct ScopedTryLock {
    ScopedTryLock(SpinLock &lock) : lock_(lock) {
        locked_ = lock_.try_lock();
    }

    ~ScopedTryLock() {
        if (locked_)
            lock_.unlock();
    }

    bool is_locked() const {
        return locked_;
    }

    bool locked_;
    SpinLock &lock_;
};

} // namespace infinity
