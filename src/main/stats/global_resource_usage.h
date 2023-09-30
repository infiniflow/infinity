//
// Created by JinHai on 2022/11/25.
//

#pragma once

#include "common/singleton.h"
#include "common/types/alias/primitives.h"

#include <atomic>

namespace infinity {

class GlobalResourceUsage {
public:

    static inline void
    Init() {
        if(initialized_)
            return;
        initialized_ = true;

    }

    static inline void
    UnInit() {
        if(initialized_) {
            object_count_ = 0;
            raw_memory_count_ = 0;
            initialized_ = false;
        }
    }

    static inline void
    IncrObjectCount() {
        ++object_count_;
    }

    static void
    DecrObjectCount() {
        --object_count_;
    }

    static i64
    GetObjectCount() {
        return object_count_;
    }

    static void
    IncrRawMemCount() {
        ++raw_memory_count_;
    }

    static void
    DecrRawMemCount() {
        --raw_memory_count_;
    }

    static i64
    GetRawMemoryCount() {
        return raw_memory_count_;
    }

private:
    static std::atomic<bool> initialized_;
    static std::atomic<i64> object_count_;
    static std::atomic<i64> raw_memory_count_;
};

}
