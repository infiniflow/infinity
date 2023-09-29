//
// Created by jinhai on 23-9-8.
//

#pragma once

#include "common/singleton.h"
#include "common/types/internal_types.h"

namespace infinity {

class Infinity;

class ResourceManager : public Singleton<Infinity> {
public:
    explicit
    ResourceManager(u64 total_cpu_count, u64 total_memory)
            : total_cpu_count_(total_cpu_count),
              total_memory_(total_memory) {}

    inline u64
    GetCpuResource(u64 cpu_count) {
        total_cpu_count_ -= cpu_count;
        return cpu_count;
    }

    inline u64
    GetCpuResource() {
        return GetCpuResource(4);
    }

    inline u64
    GetMemoryResource(u64 memory_size) {
        total_memory_ -= memory_size;
        return memory_size;
    }

    inline u64
    GetMemoryResource() {
        // Get 16MB each time
        return GetMemoryResource(16 * 1024 * 1024);
    }

private:
    au64 total_cpu_count_;
    au64 total_memory_;
};

}
