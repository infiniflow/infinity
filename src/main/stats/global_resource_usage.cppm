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
import third_party;

export module global_resource_usage;

namespace infinity {

export class GlobalResourceUsage {
public:
    static inline void Init() {
#ifdef INFINITY_STATS
        if (initialized_) {
            return;
        }
        object_count_ = 0;
        raw_memory_count_ = 0;
        initialized_ = true;
#endif
    }

    static inline void UnInit() {
#ifdef INFINITY_STATS
        if (initialized_) {
            object_count_ = 0;
            raw_memory_count_ = 0;
            initialized_ = false;
        }
#endif
    }

    static inline void IncrObjectCount(const String &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        ++object_map_[key];
        ++object_count_;
#endif
    }

    static void DecrObjectCount(const String &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        --object_map_[key];
        --object_count_;
#endif
    }

    static i64 GetObjectCount() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return object_count_;
#else
        return 0;
#endif
    }

    static String GetObjectCountInfo() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return std::to_string(object_count_);
#else
        return "Not activate";
#endif
    }

    static i64 GetObjectCount(const String &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return object_map_[key];
#else
        return 0;
#endif
    }

    static HashMap<String, i64> GetObjectClones() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return object_map_;
#else
        return HashMap<String, i64>();
#endif
    }

    static void IncrRawMemCount(const String &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        ++raw_memory_count_;
        ++raw_memory_map_[key];
#endif
    }

    static void DecrRawMemCount(const String &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        --raw_memory_count_;
        --raw_memory_map_[key];
#endif
    }

    static i64 GetRawMemoryCount() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return raw_memory_count_;
#else
        return 0;
#endif
    }

    static i64 GetRawMemoryCount(const String &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return raw_memory_map_[key];
#else
        return 0;
#endif
    }

    static String GetRawMemoryInfo() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return fmt::format("allocate count: {}, total_size: {}", raw_memory_map_.size(), raw_memory_count_);
#else
        return "Not activate";
#endif
    }

    static HashMap<String, i64> GetRawMemoryClone() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return raw_memory_map_;
#else
        return HashMap<String, i64>();
#endif
    }

private:
    static atomic_bool initialized_;

    static std::mutex object_mutex_;
    static i64 object_count_;
    static HashMap<String, i64> object_map_;

    static std::mutex raw_memory_mutex_;
    static i64 raw_memory_count_;
    static HashMap<String, i64> raw_memory_map_;
};

} // namespace infinity
