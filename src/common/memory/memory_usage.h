#pragma once

#include "common/types/internal_types.h"

#include <atomic>

namespace infinity {

class MemoryUsage {
public:
    i64 GetCurrentBytes() const {
        return current_bytes_.load(std::memory_order_relaxed);
    }

    void SetCurrentBytes(i64 size) {
        current_bytes_.store(size, std::memory_order_relaxed);
        auto prev_max_bytes = max_bytes_.load(std::memory_order_relaxed);
        if(size > prev_max_bytes) {
            max_bytes_.store(size, std::memory_order_relaxed);
        }
    }

    i64 GetMaxBytes() const {
        return max_bytes_.load(std::memory_order_relaxed);
    }

    void IncrCurrentBytes(i64 size) {
        SetCurrentBytes(GetCurrentBytes() + size);
    }

private:
    std::atomic<i64> current_bytes_{0};
    std::atomic<i64> max_bytes_{0};
};

}