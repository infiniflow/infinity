#pragma once

#include "common/types/internal_types.h"
#include "memory_usage.h"

#include <memory>

namespace infinity {

constexpr i64 kMaxMemory = std::numeric_limits<i64>::max();

class MemoryTracker
    : public std::enable_shared_from_this<MemoryTracker> {
public:
    static SharedPtr<MemoryTracker>
    Create(i64 max_memory = kMaxMemory) {
        return Create(nullptr,max_memory);
    }

    ~MemoryTracker(){}

    SharedPtr<MemoryTracker> AddChild() {
        return Create(shared_from_this());
    }

    i64 Capacity() const {
        return parent_ != nullptr ? parent_->Capacity() : max_memory_;
    }

    i64 Consumption() const {
        return memory_usage_.GetCurrentBytes();
    }

    void Consume(i64 bytes) {
        memory_usage_.IncrCurrentBytes(bytes);
    }

    bool TryConsume(i64 bytes) {
        if((Consumption() + bytes) > Capacity()) {
            return false;
        }
        memory_usage_.IncrCurrentBytes(bytes);
        return true;
    }

    void Release(i64 bytes) {
        memory_usage_.IncrCurrentBytes(bytes);
    }

private:
    static SharedPtr<MemoryTracker>
    Create(
        const SharedPtr<MemoryTracker>& parent,
        i64 max_memory = kMaxMemory);

    MemoryTracker(
        const SharedPtr<MemoryTracker>& parent,
        i64 max_memory)
        : parent_(parent), max_memory_{max_memory} {}

    SharedPtr<MemoryTracker> parent_;
    i64 max_memory_;
    MemoryUsage memory_usage_;
};

}