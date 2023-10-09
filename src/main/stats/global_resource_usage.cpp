//
// Created by JinHai on 2022/11/25.
//

#include "global_resource_usage.h"

namespace infinity {

std::atomic<bool> GlobalResourceUsage::initialized_ = false;

std::atomic<i64> GlobalResourceUsage::object_count_ = 0;

std::atomic<i64> GlobalResourceUsage::raw_memory_count_ = 0;

} // namespace infinity
