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
#ifdef INFINITY_DEBUG
        if (initialized_) {
            return;
        }
        object_count_ = 0;
        raw_memory_count_ = 0;
        initialized_ = true;
#endif
    }

    static inline void UnInit() {
#ifdef INFINITY_DEBUG
        if (initialized_) {
            object_count_ = 0;
            raw_memory_count_ = 0;
            initialized_ = false;
        }
#endif
    }

    static inline void IncrObjectCount() {
#ifdef INFINITY_DEBUG
        ++object_count_;
#endif
    }

    static void DecrObjectCount() {
#ifdef INFINITY_DEBUG
        --object_count_;
#endif
    }

    static i64 GetObjectCount() {
#ifdef INFINITY_DEBUG
        return object_count_;
#endif
    }

    static void IncrRawMemCount() {
#ifdef INFINITY_DEBUG
        ++raw_memory_count_;
#endif
    }

    static void DecrRawMemCount() {
#ifdef INFINITY_DEBUG
        --raw_memory_count_;
#endif
    }

    static i64 GetRawMemoryCount() {
#ifdef INFINITY_DEBUG
        return raw_memory_count_;
#endif
    }

private:
    static atomic_bool initialized_;
    static ai64 object_count_;
    static ai64 raw_memory_count_;
};

} // namespace infinity
