//
// Created by jinhai on 23-10-17.
//

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
