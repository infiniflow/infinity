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

import stl;

export module global_resource_usage;

namespace infinity {

export class GlobalResourceUsage {
public:
    static inline void Init() {
        if (initialized_) {
            return;
        }
        object_count_ = 0;
        raw_memory_count_ = 0;
        initialized_ = true;
    }

    static inline void UnInit() {
        if (initialized_) {
            object_count_ = 0;
            raw_memory_count_ = 0;
            initialized_ = false;
        }
    }

    static inline void IncrObjectCount() { ++object_count_; }

    static void DecrObjectCount() { --object_count_; }

    static i64 GetObjectCount() { return object_count_; }

    static void IncrRawMemCount() { ++raw_memory_count_; }

    static void DecrRawMemCount() { --raw_memory_count_; }

    static i64 GetRawMemoryCount() { return raw_memory_count_; }

private:
    static atomic_bool initialized_;
    static ai64 object_count_;
    static ai64 raw_memory_count_;
};

} // namespace infinity
