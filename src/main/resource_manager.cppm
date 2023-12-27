// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import singleton;
import stl;

export module resource_manager;

namespace infinity {

export class ResourceManager : public Singleton<ResourceManager> {
public:
    explicit ResourceManager(u64 total_cpu_count, u64 total_memory) : total_cpu_count_(total_cpu_count), total_memory_(total_memory) {}

    inline u64 GetCpuResource(u64 cpu_count) {
        total_cpu_count_ -= cpu_count;
        return cpu_count;
    }

    inline u64 GetCpuResource() { return GetCpuResource(Thread::hardware_concurrency()); }

    inline u64 GetMemoryResource(u64 memory_size) {
        total_memory_ -= memory_size;
        return memory_size;
    }

    inline u64 GetMemoryResource() {
        // Get 16MB each time
        return GetMemoryResource(16 * 1024 * 1024);
    }

private:
    atomic_u64 total_cpu_count_;
    atomic_u64 total_memory_;
};

} // namespace infinity
