// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

#pragma once

#include <string>
#include <mutex>
#include <unordered_map>
#include <atomic>

namespace infinity {

class GlobalResourceUsage {
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

    static inline void IncrObjectCount(const std::string &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        ++object_map_[key];
        ++object_count_;
#endif
    }

    static void DecrObjectCount(const std::string &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        --object_map_[key];
        --object_count_;
#endif
    }

    static int64_t GetObjectCount() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return object_count_;
#else
        return 0;
#endif
    }

    static std::string GetObjectCountInfo() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return std::to_string(object_count_);
#else
        return "Not activate";
#endif
    }

    static int64_t GetObjectCount(const std::string &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return object_map_[key];
#else
        return 0;
#endif
    }

    static std::unordered_map<std::string, int64_t> GetObjectClones() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(object_mutex_);
        return object_map_;
#else
        return std::unordered_map<std::string, int64_t>();
#endif
    }

    static void IncrRawMemCount(const std::string &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        ++raw_memory_count_;
        ++raw_memory_map_[key];
#endif
    }

    static void DecrRawMemCount(const std::string &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        --raw_memory_count_;
        --raw_memory_map_[key];
#endif
    }

    static int64_t GetRawMemoryCount() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return raw_memory_count_;
#else
        return 0;
#endif
    }

    static int64_t GetRawMemoryCount(const std::string &key) {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return raw_memory_map_[key];
#else
        return 0;
#endif
    }

    static std::string GetRawMemoryInfo() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return "allocate count: " + std::to_string(raw_memory_map_.size()) + ", total_size: " + std::to_string(raw_memory_count_);
#else
        return "Not activate";
#endif
    }

    static std::unordered_map<std::string, int64_t> GetRawMemoryClone() {
#ifdef INFINITY_STATS
        std::unique_lock<std::mutex> unique_locker(raw_memory_mutex_);
        return raw_memory_map_;
#else
        return std::unordered_map<std::string, int64_t>();
#endif
    }

private:
    static std::atomic_bool initialized_;

    static std::mutex object_mutex_;
    static int64_t object_count_;
    static std::unordered_map<std::string, int64_t> object_map_;

    static std::mutex raw_memory_mutex_;
    static int64_t raw_memory_count_;
    static std::unordered_map<std::string, int64_t> raw_memory_map_;
};

} // namespace infinity


